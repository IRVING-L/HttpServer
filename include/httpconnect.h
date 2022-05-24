#ifndef __HTTPCONNECT_H__
#define __HTTPCONNECT_H__
#include <arpa/inet.h> //sockaddr_in
#include <sys/uio.h>   //readv/writev
#include <iostream>
#include <sys/types.h>
#include <assert.h>
#include <atomic>
#include "httprequest.h"
#include "httpresponse.h"
#include "buffer.h"
/*
httpconnect.h 是整个服务器代码中，事件处理的核心
其主要功能在于能够完成对客户端发来的http进行解析并
作出响应
*/

class Httpconnection
{
public:
    Httpconnection();
    ~Httpconnection();
    
    void initHTTPConn(int socketFd, const sockaddr_in &addr);

    //每个连接中定义的对缓冲区的读写接口
    ssize_t readBuffer(int *saveErrno);
    ssize_t writeBuffer(int *saveErrno);

    //关闭HTTP连接的接口
    void closeHTTPConn();
    //定义处理该HTTP连接的接口，主要分为request的解析和response的生成
    bool handleHTTPConn();

    //其他方法
    const char *getIP() const;
    int getPort() const;
    int getFd() const;
    sockaddr_in getAddr() const;
    int writeBytes();
    bool isKeepAlive() const;
    bool isGetRightResource;
    bool isHttpDone;

    static bool isET;
    static const char *srcDir;
    static std::atomic<int> userCount;

private:
    int m_fd;
    struct sockaddr_in m_addr;
    bool isClose;

    int m_iovCnt;
    struct iovec m_iov[2]; // 用于分散写writev的结构体

    Buffer m_readBuf;
    Buffer m_writeBuf;

    Httprequest m_request;
    Httpresponse m_response;
};

#endif