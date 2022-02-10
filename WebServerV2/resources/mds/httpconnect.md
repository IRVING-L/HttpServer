## HTTP连接处理详解

### 背景

服务器和核心功能是完成对HTTP请求报文的解析，并向客户端发出HTTP响应报文。在Httpconnection模块正是要完成上述的功能需求。

为了完成报文解析、资源定位、发送响应等功能需求，该项目一共是写了4个头文件，与此对应的就是4个模块：Httpconnection模块、Httprequest模块、Httpresponse模块以及Buffer缓冲区模块。四个模块之间的相互关系如下图所示

![1](../images/img1.png)

### Httpconnection类成员变量介绍

Httpconnection是对HTTP连接的抽象，在模块中要定义一些变量保存socket通信客户端的信息：（建立socket连接由其他模块负责，这里只负责通信和关闭通信）一条socket连接就需要一个Httpconnection模块，所以我们使用`m_fd`唯一的标记它；并且使用`m_isClose`判断连接是否被关闭，便于调用`closeConn`关闭客户端连接。一个HTTP连接还需要读写数据，所以给每一个HTTP连接定义一个读缓冲区和一个写缓冲区。在解析请求和响应请求的时候，我们借助`Httprequest和Httpresponse`完成，所以也需要各种定义一个这两种变量。
```cpp
private:
    int m_fd;
    struct sockaddr_in m_addr; // 获取IP和port
    bool isClose;

    int m_iovCnt;
    struct iovec m_iov[2]; // 用于分散写writev的结构体

    Buffer m_readBuf;
    Buffer m_writeBuf;

    Httprequest m_request;
    Httpresponse m_response;
```
此外，还定义了三个static静态成员：`isET`, `srcDir`, `userCount`。类的静态成员不属于某一个类对象，是属于整个类的，所有的类对象都可以访问该静态成员。
`isET`布尔变量表征所有的Httpconnection对象是否在使用epoll的ET边缘触发模式。
`srcDir`定位资源的根路径
`userCount`是当前连接的数量。由于对该变量有多线程读取和写入，需要作线程保护。我们可以使用std::atomic对变量进行线程保护
```cpp
public:
    static bool isET;
    static const char *srcDir;
    static std::atomic<int> userCount;
```
### Httpconnection任务流程介绍

成员变量的介绍到这里就结束了。下面对Httpconnection模块的执行流程作一个介绍：

完成工作任务的主要是3个函数：
```cpp
    ssize_t readBuffer(int *saveErrno);
    bool handleHTTPConn();
    ssize_t writeBuffer(int *saveErrno);
```
`readBuffer`函数从m_fd对应的socket中读取数据，由于在Buffer缓冲区中我们已经定义好了读取函数`recvFd`，这里只是简单的对该函数进行封装即可。读取到的HTTP请求报文会保存在之前定义的读取缓冲`m_readBuf`

`handlerHTTPConn`整个类执行任务的核心。在该函数中，我们将`m_readBuf`交由请求解析模块m_request进行报文解析，成功解析后，会调用报文响应模块m_response完成响应报文状态行和首部字段的填写、分散写结构体m_iov的装填等一系列操作。函数执行成功后，将会返回1。

`writeBuffer`函数是将数据发送到m_fd对应的socket中去。由于需要发送服务器上的文件资源，针对这种情况通常都是使用`writev`分散写函数将多块内存的数据一同发送到socket对端。当`handlerHTTPConn`函数执行成功后，m_iov结构体已经填充好了要发送的响应报文（状态行、首部字段、报文主体），此时调用`writeBuffer`函数就可以将响应报文发送给客户端。


以上便是Httpconnection模块的工作流程。当然，在执行任务之前，需要调用`initHTTPConn`函数对成员变量进行初始化。此外，还定义了一些外部接口函数，以供访问内部的成员变量。

Httpconnection类的实现：
```cpp
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
#include "bufferV2.h"
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
```

## Httprequest类成员的介绍

通过上面对Httpconnection类的介绍，相信你对Httprequest类所需要完成的功能有了一个大概的印象：完成对HTTP请求的解析。

一个HTTP请求由三个部分组成：请求行、首部字段和报文主体。为了判断程序正在解析哪一部分，这里使用**状态机**技术概念构建我们这个解析类。换句话说，针对每一部分的解析，编写对应的解析函数，根据一个状态标志位，决定在某一个时刻程序应该运行哪一个解析函数。解析状态一共可分为四种：解析请求行、解析首部字段、解析报文主体、结束解析。这里使用了enum枚举类预先定义了解析状态`PARSE_STATE`

此外，定义了一个哈希表用于保存解析得到的首部字段的详细信息，以及一个用于查找网页的哈希表。

```cpp
    PARSE_STATE m_state;
    std::string m_method, m_path, m_version, m_body;
    std::unordered_map<std::string, std::string> m_header; // 首部字段的哈希表
    static const std::unordered_set<std::string>DEFAULT_HTML; // 用于查找网页的哈希表
```

### Httprequest任务流程介绍

Httprequest类对外最重要的一个函数：parse
```cpp
bool parse(Buffer &buff); //解析HTTP请求
```
他在内部调用多个私有的解析函数，完成对传入的Buffer进行解析

几个私有函数如下所示：
```cpp
    bool _parseRequestLine(const std::string &line);   //解析请求行
    void _parseRequestHeader(const std::string &line); //解析请求头
    void _parseDataBody(const std::string &line);      //解析数据体
    void _parsePath();                                 //解析请求资源的网址
```
Httprequest请求解析类的工作流程通过这几个函数就做完了，具体的细节大家可以去查看一下源代码，还是比较简单的。

这里插一句，代码中使用到了C++11的正则表达式匹配（std::regex），代替了常用的string字符串匹配，缩减了代码量，增加了程序的可读性。感兴趣的同学可以自己查阅资料了解一下

### Httpresponse类成员介绍

下面介绍一下响应报文类Httpresponse。这里再次给大家讲一下HTTP服务器的处理流程：获取请求报文并解析报文-->获取请求的资源 -->生成响应报文，并将响应报文和资源一同发送给客户端。

在Httprequest类中，只完成了解析报文这部分的任务，查找请求资源和发送响应报文这两部分的任务在Httpresponse类中完成。

客户端请求服务器上的“资源”，其一般保存在系统的硬盘/磁盘上。我们可以使用内存映射函数，将位于磁盘上的文件，映射到内存上，加快对文件的读写速度，所以这里定了一个指向映射文件的指针`m_mmfile`。此外，针对客户端请求的文件名，我们需要判断文件的合法性，这里定义了`stat`结构体保存文件的信息。如果文件被成功获取，可以从结构体`mmFileState`中获取文件大小等信息，非常的方便。

其他的变量通过变量名想必大家都能知道其大致的作用。`m_code`是获取资源的状态，同时也是响应报文中的状态码（常见的有200,403,404）。

最后还定义了三个【静态】的哈希表，这是为了在程序中能够快速的根据`m_code`或者资源路径打开对应的资源，具体的细节可以查看源码。
```cpp
    int m_code;
    bool m_keepalive;
    std::string m_path;
    std::string m_srcDir;

    char *m_mmfile;
    struct stat mmFileState;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
```

### Httpresponse任务流程介绍

类需要完成的功能都整合到了对外接口函数`makeResponse`上。
```cpp
    void makeResponse(Buffer &buffer);
```

首先，在调用Httpresponse类对象时，一开始通过`init`函数完成成员变量的初始化。在`makeResponse`函数中，通过路径判断需要访问的资源是否合法，下一步就是调用函数填写响应报文

填写响应报文我们定义了三个私有函数，分别用来填写状态行、首部字段和报文主体。在`addContent`函数中，通过根路径和子路径组合得到的绝对路径，通过内存映射访问对应的资源文件（合法路径打开对应的文件，非法路径打开预先设定好的HTML文件）
```cpp
    // 填写首部字段
    void addStateLine(Buffer &buffer);
    void addHeader(Buffer &buffer);
    void addContent(Buffer &buffer);
```
到这里，Httpresponse类需要完成的工作就做好了。这个类主要就是打开需要访问的资源，最后交由上层————Httpconnection类对象调用`writeBuffer`函数将获取到的资源以正确的响应报文格式发送出去。

**HTTP连接处理详解完**


