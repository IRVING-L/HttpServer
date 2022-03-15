#ifndef __HTTP_CLIENT_INFO_H__
#define __HTTP_CLIENT_INFO_H__

// 这个封装类的功能是保存客户端的IP、sockaddr等属性

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/uio.h>
#include <string.h>

class ClientInfo
{
public:
    //构造函数和析构函数
    ClientInfo();
    ClientInfo(char *ip, int port, int cfd);
    ClientInfo(char *ip, int port, int cfd, struct sockaddr_in *addr);
    ~ClientInfo();

    // epollfd
    static int m_epollfd;
    // 用户数量
    static int m_userCount;

    // 成员函数
    // 存储IP等信息
    void set(char *ip, int port);

    // 对外传递cfd
    int getFd();
    

private:
    // IP
    char m_IP[20];
    // port
    int m_port;
    // sockaddr
    struct sockaddr_in m_sockaddr;
    // fd，socket文件描述符
    int m_cfd;
};

#endif