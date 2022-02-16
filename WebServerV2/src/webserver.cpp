#include "../lib/webserver.h"

Webserver::Webserver(int port, int trigMode, int timeoutMS, bool optLinger, int threadNum)
{
    m_port = port;
    initEventMode(trigMode);
    m_timeoutMs = timeoutMS;
    m_openLinger = optLinger;
    m_pool = std::make_unique<Threadpool>(threadNum);
    m_timer = std::make_unique<TimeManager>();
    m_epoller = std::make_unique<Epoller>();
    m_isclose = (initSocket() ? false : true);
}
Webserver::~Webserver()
{
    close(m_listenFd);
    m_isclose = true;
}

void Webserver::start() // 程序启动
{
    int timeMs = -1;
    if (!m_isclose)
    {
        std::cout << "============================";
        std::cout << "Server Start!";
        std::cout << "============================";
        std::cout << std::endl;
    }
    while (!m_isclose)
    {
        if (m_timeoutMs > 0)
        {
            /* 
            这里要弄清楚几个点：
                - 为什么要将传递给epoll_wait的时间参数，通过getNextHandle修改？
                    - 让epoll_wait更加socket通信的状态，灵活调整等待时间
                        - 意义：epoll在项目中在监听socket的文件描述符--》哪些文件描述符--》新socket进来、某个socket上的读或者写事件
                            感觉这样设定意义不大（2022年2月16日19:47:01）
                            处理过期的socket节点
                - getNextHandle会做一件什么事情？
                    - 获取到第一个快过期的结点的剩余时间（ms）
            */
            timeMs = m_timer->getNextHandle();
        }
        int eventCnt = m_epoller->wait(timeMs);
        for (int i = 0; i < eventCnt; ++i)
        {
            int currfd = m_epoller->getSockFd(i);
            uint32_t events = m_epoller->getFdEvent(i);

            if (currfd == m_listenFd)
            {
                handleListen();
            }
            else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                if (m_usrs.count(currfd) > 0)
                {
                    closeConn(&m_usrs[currfd]);
                }
            }
            else if (events & EPOLLIN)
            {
                if (m_usrs.count(currfd) > 0)
                {
                    handleRead(&m_usrs[currfd]);
                }
            }
            else if (events & EPOLLOUT)
            {
                if (m_usrs.count(currfd) > 0)
                {
                    handleWrite(&m_usrs[currfd]);
                }
            }
            else
            {
                std::cout << "unexpected event" << std::endl;
            }
        }
    }
}

bool Webserver::initSocket()
{
    struct sockaddr_in addr;
    if (m_port > 65535 || m_port < 1024)
    {
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(m_port);
    struct linger optLinger = {0};
    if (m_openLinger)
    {
        /* 优雅关闭: 直到所剩数据发送完毕或超时 */
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenFd < 0)
    {
        return false;
    }
    // LINGER
    int ret = setsockopt(m_listenFd, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if (ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    int reuse = 1;
    ret = setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    ret = bind(m_listenEvent, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    ret = listen(m_listenFd, 6);
    if (ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    ret = m_epoller->add(m_listenFd, m_listenEvent | EPOLLIN);
    if (ret = 0)
    {
        close(m_listenFd);
        return false;
    }
    setNONBLOCKING(m_listenFd);
    return true;
}
void Webserver::initEventMode(int trigMode)
{
    // EPOLLRDHUP宏，底层处理socket连接断开的情况
    m_listenEvent = EPOLLRDHUP;
    /* EPOLLONESHOT 和 ET模式不尽相同：
    前者是防止一个客户端发送的数据被多个线程分散读取；
    后者是避免多次调用epoll_wait提高epoll效率的一种模式
    */
    m_connectEvent = EPOLLONESHOT | EPOLLRDHUP;
    switch (trigMode)
    {
    case 1:
        // 0000 0001
        m_connectEvent |= EPOLLET;
        break;
    case 2:
        // 0000 0010
        m_listenEvent |= EPOLLET;
        break;
    case 3:
        // 0000 0011
        m_connectEvent |= EPOLLET;
        m_listenEvent |= EPOLLET;
        break;
    default:
        // 其他就是都LT
        break;
    }
    Httpconnection::isET = m_connectEvent & EPOLLET;
}
void Webserver::addClientConnect(int fd, struct sockaddr_in addr) // 添加一个连接
{
    /*
    在webServer类对象中，添加一个新的客户端连接的过程：
        - 初始化Httpconnection对象：哈希表m_usrs中新建一个对象（Httpconnection），调用Httpconnection对象的init函数，初始化其对象的变量
        - 添加定时器：在定时器管理类中，添加新的socket，并且设置超时时间 == m_timeoutMs
        - 添加到epoll中：将socket的文件描述符添加到epoll中，设置事件 == EPOLL | m_connectEvent
        - 将socket连接这个fd设置为非阻塞状态。目的：在调用read/recv函数时，发送方如果没有发送数据，并不会阻塞在那里
    */
    if (fd < 0)
    {
        std::cout << "location:webserver.cpp:addClient,"
                  << "error:invalid fd" << std::endl;
        return;
    }
    // 参数addr用的是拷贝，而不是引用或者指针，这是因为传入的addr下一次会被更新
    // 哈希表中没有fd这个key时，会以fd为key，调用value的构造函数，创建出这样一个键值对
    m_usrs[fd].initHTTPConn(fd, addr);
    if (m_timeoutMs > 0)
    {
        m_timer->addTimer(fd, m_timeoutMs, std::bind(&Webserver::closeConn, this, &m_usrs[fd]));
    }
    m_epoller->add(fd, EPOLLIN | m_connectEvent);
    setNONBLOCKING(fd);
}
void Webserver::closeConn(Httpconnection *client)
{
    /*
    关闭一个socket连接的步骤：
        - 从epoll中删除：调用epoller类对象的del函数删除
        - 释放Httpconnection对象：调用Httpconnection对象的closeHttpConn函数（释放内存映射、关闭文件描述符）
        - 是否要从定时器管理类中释放对象：
            - 不需要：一段时间后没有发生改变就会被删掉了
                - 定时器管理类能否正确删除
    */
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:closeConn,"
                  << "error:invalid client" << std::endl;
        return;
    }
    m_epoller->del(client->getFd());
    client->closeHTTPConn();
}

void Webserver::handleListen()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do
    {
        int cfd = accept(m_listenFd, (struct sockaddr *)&addr, &len);
        if (cfd < 0)
        {
            std::cout << "location:webserver.cpp:handleListen,"
                      << "error:invalid cfd" << std::endl;
            return;
        }
        else if (Httpconnection::userCount >= MAX_FD)
        {
            sendError(cfd, "Server Busy");
            return;
        }
        addClientConnect(cfd, addr);
    } while (m_listenEvent & EPOLLET); 
    // 如果不是ET模式，该条件为0，退出循环。不过一般来说不会让listenFd设置为ET模式
    // 如果设置为ET模式了，什么时候会退出循环？似乎不会结束循环？当没有新的客户端进来后，accept是否会被阻塞在这里？
}
void Webserver::handleWrite(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:handleWrite,"
                  << "error:invalid client" << std::endl;
        return;
    }
    extentTime(client);
    // 向线程池中提交任务
    // 在线程池中，通过异步函数，读取数据？
    m_pool->append(std::bind(&Webserver::onWrite, this, client));
}
void Webserver::handleRead(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:handleWrite,"
                  << "error:invalid client" << std::endl;
        return;
    }
    // 这个socket上有读事件到来，需要在定时器类中更新该socket的时间
    extentTime(client);
    // 向线程池中提交任务
    // 在线程池中，通过异步函数，读取数据？
    m_pool->append(std::bind(&Webserver::onRead, this, client));
}

void Webserver::onRead(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:handleWrite,"
                  << "error:invalid client" << std::endl;
        return;
    }
    int ret = -1;
    int readErrno = 0;
    ret = client->readBuffer(&readErrno);
    if (ret <= 0 && readErrno != EAGAIN)
    {
        closeConn(client);
        return;
    }
    onProcess(client);
}
void Webserver::onWrite(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:handleWrite,"
                  << "error:invalid client" << std::endl;
        return;
    }
    int ret = -1;
    int writeErrno = 0;
    ret = client->writeBuffer(&writeErrno);
    if(client->writeBytes() == 0)
    {
        if(client->isKeepAlive())
        {
            // 这一步是什么意思？
            onProcess(client);
            return;
        }
    }
    else if(ret < 0)
    {
        if(writeErrno == EAGAIN)
        {
            m_epoller->mod(client->getFd(), m_connectEvent | EPOLLOUT);
            return;
        }
    }
    else
    {
        closeConn(client);
    }
}
void Webserver::onProcess(Httpconnection *client)
{
    if (client->handleHTTPConn())
    {
        m_epoller->mod(client->getFd(), m_connectEvent | EPOLLOUT);
    }
    else
    {
        m_epoller->mod(client->getFd(), m_connectEvent | EPOLLIN);
    }
}

void Webserver::sendError(int fd, const char *info)
{
    if (fd < 0)
    {
        std::cout << "location:webserver.cpp:sendErr,"
                  << "error:invalid fd" << std::endl;
        return;
    }
    int ret = send(fd, info, strlen(info)+1, 0);
    if(ret < 0)
    {
        std::cout << "error\n"; 
    }
    close(fd);
}
void Webserver::extentTime(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:extentTime,"
                  << "error:invalid client" << std::endl;
        return;
    }
    if(m_timeoutMs > 0)
    {
        m_timer->updateTimer(client->getFd(), m_timeoutMs);
    }
}