#ifndef __HTTP_CONNECT_H__
#define __HTTP_CONNECT_H__

// 定义一个线程池的事件类
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
#include <string>
#include <stdarg.h>
#include "http_client_info.h"
#include "epoll_function.h"
#include "log.h"

// 在此处，我们定义Http_Conn类会用到的常量

// 用于初始化存放url的内存m_url
#define MAX_URL_LEN 100
// 指定了存放请求报文首部字段的内存的大小：不得超过1KB
#define MAX_HEADLER_LEN 1024
// 指定了读取数据的大小：不得超过1KB
#define MAX_READ_BUFSIZE 1024
#define MAX_WRITE_BUFSIZE 1024
// 这个是块的意思，因为我觉得1KB也许会太小了，我就以1KB为单位，BLOCKS为块数，指定readBuf的大小
#define BLOCKS 10


/*
Http_Conn类实现的功能：
1. 从内核缓冲区中读取数据（HTTP请求报文）
2. 完成HTTP请求报文的解析、请求资源查找、填写响应报文
3. 向内核缓冲区中写入数据（HTTP响应报文）
// 辅助函数
1. 关闭客户端连接的函数
按照上述的逻辑结构，将成员函数和成员变量按顺利排列
*/


class Http_Conn
{
public:
    // 构造函数
    Http_Conn();

    // 析构函数
    ~Http_Conn();

    // 成员函数
    // 业务逻辑处理单元函数
    void process();
    // 读取缓冲区的数据
    bool m_read();
    // 向缓冲区写入数据
    bool m_write();
    // epoll相关变量的获取
    void getClientInfo(int epfd, int cfd, char *ip, int port);
    // 辅助函数1：关闭客户端连接
    void closeConn();
    // 成员变量
    // 静态成员变量
    static int clientCount;

private:
    // 枚举类型
    enum class METHOD : int;
    enum class CHECK_STATE : int;
    enum class HTTP_CODE : int;
    enum class LINE_STATUS : int;

    // *******【成员函数】*******

    /*
    1. 从内核缓冲区读取数据
    */

    /*
    2. 完成HTTP请求报文的解析、请求资源查找、填写响应报文
    */
    // http解析的主函数
    HTTP_CODE httpParse();
    // 解析http请求报文的[请求行]（第一行）的函数
    HTTP_CODE requestParse(char *text);
    // 解析http请求报文的[首部字段]的函数
    HTTP_CODE headerParse(char *text);
    // 解析http请求报文的[正文]的函数
    HTTP_CODE contentParse(char *text);


    // 请求资源查找判断的函数
    HTTP_CODE responseRequest();

    // http响应报文的相关函数
    // http响应的主函数
    bool httpResponse( HTTP_CODE ret);
    // 写入状态行的函数
    bool writeStatus(int status, const char *text);
    // 写入首部字段的函数
    bool writeHeader(const int len);
    // 写入报文主体的函数
    bool writeContent(const char *text);
    // 实际执行写任务的函数
    bool writeResponse(const char *format, ... );

    // 在首部字段函数中实际执行相应填写的函数
    bool writeContentLength(const int len);
    bool writeContentType(); 
    bool writeKeepAlive();
    bool writeBlankLine();
    
    

    // 初始化下面的成员变量的函数。下面的变量，都要等到readBuf中有数据了才能正确初始化
    void m_init();
    // 有初始化，就要销毁
    void m_destroy();
    // 成功接收过一次数据后，更新一次变量
    void m_restart();
    // 格式化处理readBuf中的数据
    char *m_getline();
    
    // *******【成员变量】*******
    /*
    1. 从内核缓冲区读取数据
    */
    // epoll和客户端的信息
    int m_epollFd;
    int m_sockFd;
    char m_IP[20];
    int m_port;
    // http_conn对象存放读入的数据的地方
    char *m_readBuf;
    // 这个是一个指针，表明readBuf中读取到的字节数
    int m_readBufIndex;
    
    /*
    2. 完成HTTP请求报文的解析、请求资源查找、填写响应报文
    */
    char *m_writeBuf;
    // 这个是记录m_writeBuf中写入的数据长度
    int m_writeBufIndex;
    // 这个现在不用了，原本是用来记录调用send发送数据的长度的
    int m_writePtr;
    /*
    用于m_getline()函数的指针
    readBuf: ................................................
                |
                | readBuf[m_carryReadBufPtr++] -->
                v
    m_text:  ....
    */
    int m_carryReadBufPtr;
    char *m_text;
    // requestParse 函数中出现的变量
    char *m_method;
    char *m_url;
    char *m_version;
    // headlerParse 函数中出现的变量
    int m_contentLength;
    CHECK_STATE m_checkState;
    bool m_keepAlive;
    char *m_host;
    // responseRequest函数中出现的变量
    char *m_mmapAddr;
    struct stat m_fileInfo;
    // httpResponse函数中出现的变量
    int m_bytesToSend;
    int m_bytesHaveSend;

    /*
    3. 向内核缓冲区中写入数据（HTTP响应报文）
    */
    // m_write函数中
    struct iovec m_iov[2];
    int m_iovCnt;

    // Log 文件对象
    // LOG myLog;
    // char *m_openfile;
};

#endif