#include "../lib/webserver.h"

// 构造函数：使用初始化列表对成员变量进行初始化
Webserver::Webserver(int port, int t, int trigMode, int threadNum)
    : m_port(port), m_timeoutMs(t), m_timer(new TimeManager()), m_poolV4(new ThreadPool(8,200)), m_epoller(new Epoller())
{
    // std::cout << "m_timeout:" << m_timeoutMs  << std::endl;
    getcwd(m_srcDir, sizeof(m_srcDir));
    strcat(m_srcDir, "/resources");
    // std::cout << "m_srcDir:" <<m_srcDir << std::endl;
    Httpconnection::srcDir = m_srcDir;
    initEventMode(trigMode); // 设置服务器的epoll工作模式：LT or ET？
    m_isclose = (initSocket() ? false : true);
}
// 对服务器程序进行初始化的函数
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
bool Webserver::initSocket()
{
    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenFd < 0)
    {
        std::cout << "m_listenFd error\n";
        return false;
    }
    // bind
    struct sockaddr_in saddr;
    //初始化结构体addr
    saddr.sin_family = AF_INET;
    // inet_pton(AF_INET, "10.0.12.8", (void *)&saddr.sin_addr.s_addr);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(m_port);
    // 在bind之前设置【端口复用】
    int reuse = 1;
    int ret = setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0)
    {
        std::cout << "error setsockopt\n";
        close(m_listenFd);
        return false;
    }
    ret = bind(m_listenFd, (struct sockaddr *)&saddr, 16);
    if (ret < 0)
    {
        std::cout << "error bind\n";
        close(m_listenFd);
        return false;
    }
    // listen
    ret = listen(m_listenFd, 5);
    if (ret < 0)
    {
        std::cout << "error listen\n";
        close(m_listenFd);
        return false;
    }
    // 将m_listenFd添加到epoll中
    ret = m_epoller->add(m_listenFd, m_listenEvent | EPOLLIN);
    if (ret == 0)
    {
        std::cout << "error epoll_add\n";
        close(m_listenFd);
        return false;
    }
    return true;
}
// 析构函数
Webserver::~Webserver() {}

// 成员函数
void Webserver::start()
{
    if (!m_isclose)
    {
        std::cout << "============================";
        std::cout << "Server Start!";
        std::cout << "============================";
        std::cout << std::endl;
    }
    while (!m_isclose)
    {
        // int waitTime = m_timer->getNextHandle();
        // std::cout << "waitTime: " << waitTime << std::endl;
        // 调用epoll监听
        int eventCnt = m_epoller->wait(-1);
        for (int i = 0; i < eventCnt; ++i)
        {
            // std::cout << "监听到事件\n";
            // 获取fd
            int currfd = m_epoller->getSockFd(i);
            // 获取fd对应的event
            uint32_t events = m_epoller->getFdEvent(i);

            // 判断事件类型：新连接到来？读？写？
            if (currfd == m_listenFd)
            {
                handleListen(); // 处理新连接
            }
            else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                // 对端关闭了连接
                closeConn(&m_usrs[currfd]);
            }
            else if (events & EPOLLIN)
            {
                // 读事件
                // 现在主线程中读取数据到缓冲区中；
                // 再由线程池完成业务逻辑
                // 读取http请求
                handleRead(&m_usrs[currfd]);
            }
            else if (events & EPOLLOUT)
            {
                // 写事件
                handleWrite(&m_usrs[currfd]);
            }
            else
            {
                std::cout << "unexpected event\n";
            }
        }
    }
}

// 核心成员函数：处理新连接情况
void Webserver::handleListen()
{

    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    // int i = 1;
    do
    {
        // std::cout << "handleListen:" << i++ << std::endl; 
        int cfd = accept(m_listenFd, (struct sockaddr *)&caddr, &len);
        if (cfd < 0)
        {
            std::cout << "location:webserver.cpp:handleListen,"
                      << "error:invalid cfd" << std::endl;
            return;
        }
        // 如果客户端数量超过上限
        else if (Httpconnection::userCount >= MAX_FD)
        {
            // 调用send函数向客户端发送错误信息
            // 更加严谨的应该是返回一个html网页
            sendError(cfd, "Server Busy");
            return;
        }
        //  调用封装函数，将新来的客户端的cfd和caddr加入到服务器中
        char clientIP[20] = {0};
        int clientPort = -1;
        // fflush(stdout);
        inet_ntop(AF_INET, &caddr.sin_addr, clientIP, sizeof(clientIP)); // 网络IP地址的大端转小端
        clientPort = ntohs(caddr.sin_port);
        printf("client cfd: %d, ip:%s, port:%d\n", cfd, clientIP, clientPort);

        addClientConnect(cfd, caddr);

    } while (m_listenEvent & EPOLLET);
}

// 往服务器中添加客户端的函数
void Webserver::addClientConnect(int cfd, struct sockaddr_in caddr)
{
    m_usrs[cfd].initHTTPConn(cfd, caddr);
    m_epoller->add(cfd, m_connectEvent | EPOLLIN);
    // m_timer->addTimer(cfd, m_timeoutMs, std::bind(&Webserver::delClient, this, &m_usrs[cfd]));
    setNONBLOCKING(cfd);
}
// 将客户端fd设置为非阻塞
int Webserver::setNONBLOCKING(int fd)
{
    int oldflag = fcntl(fd, F_GETFL);
    int newflag = oldflag | O_NONBLOCK;
    fcntl(fd, F_SETFL, newflag);
    return newflag;
}

// 核心成员函数：在主线程关闭客户端连接
void Webserver::closeConn(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:closeConn,"
                  << "error:invalid client" << std::endl;
        return;
    }
    std::cout << "closeConn() 关闭客户端连接. fd :" << client->getFd() << std::endl;
    m_epoller->del(client->getFd());
    // m_timer->delFd(client->getFd());
    client->closeHTTPConn();
}
// 提供给定时器管理类的删除客户端函数
void Webserver::delClient(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:delClient,"
                  << "error:invalid client" << std::endl;
        return;
    }
    std::cout << "delClient() 关闭客户端连接. fd :" << client->getFd() << std::endl;
    m_epoller->del(client->getFd());
    client->closeHTTPConn();
}

// 核心成员函数：处理读事件
bool Webserver::handleRead(Httpconnection *client)
{
    // printf("function:Webserver::handleRead(Httpconnection *client)\n");
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:handleWrite,"
                  << "error:invalid client" << std::endl;
        return false;
    }
    // 在主线程中，读取数据
    int errorNum = 0;
    int ret = client->readBuffer(&errorNum);
    if (ret <= 0 && errorNum != EAGAIN)
    {
        // 读取数据出现错误，关闭客户端
        closeConn(client);
        return false;
    }

    // 把业务处理逻辑代码交给线程池去处理
    // m_poolV3->AddTask(std::bind(&Webserver::onRead, this, client));
    m_poolV4->append(std::bind(&Webserver::onRead, this, client));
    return true;
}

void Webserver::onRead(Httpconnection *client)
{
    onProcess(client);
}
bool Webserver::handleWrite(Httpconnection *client)
{
    if (client == nullptr)
    {
        std::cout << "location:webserver.cpp:handleWrite,"
                  << "error:invalid client" << std::endl;
        return false;
    }
    // 调用Httpconnection对象的write函数
    int errnoNum = 0;
    ssize_t ret = client->writeBuffer(&errnoNum);
    // 根据发送数据的长度ret来判断服务器发送数据是否正常
    if (ret > 0)
    {
        if (client->isKeepAlive())
        {
            return true;
        }
        else
        {
            closeConn(client);
            return false;
        }
    }
    else if (client->writeBytes() == 0)
    {
        // 保存的客户端属性中，此时没有数据
        if (client->isKeepAlive())
        {
            // 对于长连接，如果没有数据，那就重新更新一个客户端中数据
            onProcess(client);
            return true;
        }
        return false;
    }
    else if (ret < 0)
    {
        if (errnoNum == EAGAIN)
        {
            m_epoller->mod(client->getFd(), m_connectEvent | EPOLLOUT);
            return true;
        }
        return false;
    }
    return true;
}

// 并非核心的辅助函数
void Webserver::sendError(int sockfd, const char *info)
{
    if (sockfd < 0)
    {
        std::cout << "location:webserver.cpp:sendError,"
                  << "error:invalid cfd" << std::endl;
        return;
    }
    int ret = send(sockfd, info, sizeof(info) + 1, 0);
    if (ret < 0)
    {
        std::cout << "location:webserver.cpp:sendError,"
                  << "error:send" << std::endl;
        return;
    }
    // 关闭这个客户端的fd
    close(sockfd);
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
    // std::cout << client->getFd() << std::endl;
    // m_timer->updateTimer(client->getFd(), m_timeoutMs);
}