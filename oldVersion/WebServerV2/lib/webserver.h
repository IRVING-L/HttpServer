#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__
#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "httpconnect.h"
#include "threadpool.h"
#include "timer.h"

class Webserver
{
public:
    Webserver(int port,int trigMode,int timeoutMS,bool optLinger,int threadNum);
    ~Webserver();

    void start(); // 程序启动
private:
    bool initSocket();
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

    // 静态成员
    static const int MAX_FD = 65535;
    static int setNONBLOCKING(int fd);
    int m_port;
    int m_timeoutMs;
    bool m_isclose;
    int m_listenFd;
    bool m_openLinger;
    char *m_srcDir;

    uint32_t m_listenEvent;
    uint32_t m_connectEvent;

    std::unique_ptr<TimeManager> m_timer;
    std::unique_ptr<Threadpool> m_pool;
    std::unique_ptr<Epoller> m_epoller;
    std::unordered_map<int, Httpconnection> m_usrs;
};

#endif