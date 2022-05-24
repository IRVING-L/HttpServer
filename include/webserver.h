#ifndef WEBSERVER_H
#define WEBSERVER_H
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
#include "log.h"
// #include "threadpool.h"
// #include "threadpoolV2.h"
#include "threadpoolV5.h"
// #include "threadpoolV6.h"
#include "timer.h"

class Webserver
{
public:
    Webserver(int port, int t,  int trigMode, int threadNum);
    ~Webserver();

    void start(); // 程序启动

    
private:
    bool initSocket();
    void initEventMode(int trigMode);
    void addClientConnect(int fd, struct sockaddr_in addr); // 添加一个连接
    void closeConn(Httpconnection *client);
    void delClient(Httpconnection *client);

    void handleListen();
    bool handleWrite(Httpconnection *client);
    bool handleRead(Httpconnection *client);

    void onRead(Httpconnection *client);
    void onWrite(Httpconnection *client);
    void onProcess(Httpconnection *client);

    void sendError(int fd, const char *info);
    //void extentTime(Httpconnection *client);

    // 静态成员
    static const int MAX_FD = 65535;
    static int setNONBLOCKING(int fd);
    int m_port;
    int m_timeoutMs;
    bool m_isclose;
    int m_listenFd;
    bool m_openLinger;
    char m_srcDir[108];

    uint32_t m_listenEvent;
    uint32_t m_connectEvent;

    std::unique_ptr<TimeManager> m_timer;
    
    //std::unique_ptr<ThreadPool<Httpconnection>> m_pool;
    // std::unique_ptr<Threadpool> m_poolV2;
    //  std::unique_ptr<ThreadPool> m_poolV3;
    std::unique_ptr<ThreadPool> m_poolV4;
    std::unique_ptr<Epoller> m_epoller;
    // 用存储位置为栈区的哈哈希表保存客户端的连接信息，这合理吗？
    // 理论上最大支持65535个连接，栈区的内存大小能够满足吗？
    // 破案了。哈希表对象是存放在栈区的，但是对象里的value都是保存在堆区的
    std::unordered_map<int, Httpconnection> m_usrs;

};

#endif