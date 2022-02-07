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
    struct linger optLinger = { 0 };
    if(m_openLinger) {
        /* 优雅关闭: 直到所剩数据发送完毕或超时 */
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_listenFd < 0)
    {
        return false;
    }
    // LINGER
    int ret = setsockopt(m_listenFd, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if(ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    int reuse = 1;
    ret = setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if(ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    ret = bind(m_listenEvent, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    ret = listen(m_listenFd, 6);
    if(ret < 0)
    {
        close(m_listenFd);
        return false;
    }
    ret = m_epoller->add(m_listenFd, m_listenEvent | EPOLLIN);
    if(ret = 0)
    {
        close(m_listenFd);
        return false;
    }
    setNONBLOCKING(m_listenFd);
    return true;
}
void initEventMode(int trigMode);
void addClientConnect(int fd, struct sockaddr_in addr); // 添加一个连接
void closeConn(Httpconnection *client);

void handleListen();
void handleWrite(Httpconnection *client);
void handleRead(Httpconnection *client);

void onRead(Httpconnection *client);
void onWrite(Httpconnection *client);
void onProcess(Httpconnection *client);

void sendError(int fd, const char *info);
void extentTime(Httpconnection *client);