#include "../lib/http_connect.h"

// 定义文件资源的根目录
// 如果要访问文档，那就是srcRoot + "/doc/xxx.html"
// 如果要访问图片，那就是srcRoot + "img/xxx.jpg"
const char *srcRoot = "/root/serverProjectNC/WebServer";
// 定义HTTP响应的一些状态信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file from this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the requested file.\n";

// 枚举
/*
http请求方法
GET: 向服务器请求资源
POST：向服务器发送资源
*/
enum class Http_Conn::METHOD : int
{
    GET,
    POST,
    HEAD,
    PUT,
    DELETE,
    TRACE,
    OPTIONS,
    CONNECT
};

/*
   解析客户端请求时，主状态机的状态
   CHECK_STATE_REQUESTLINE:当前正在分析请求行
   CHECK_STATE_HEADER:当前正在分析头部字段
   CHECK_STATE_CONTENT:当前正在解析请求体
*/
enum class Http_Conn::CHECK_STATE : int
{
    CHECK_STATE_REQUESTLINE,
    CHECK_STATE_HEADER,
    CHECK_STATE_CONTENT
};

/*
    服务器处理HTTP请求的可能结果，报文解析的结果
    NO_REQUEST          :   请求不完整，需要继续读取客户数据
    GET_REQUEST         :   表示获得了一个完成的客户请求
    BAD_REQUEST     :   表示客户请求语法错误
    FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
    FILE_REQUEST        :   文件请求,获取文件成功
    NO_RESOURCE         :   表示服务器没有资源
    INTERNAL_ERROR      :   表示服务器内部错误
    CLOSED_CONNECTION   :   表示客户端已经关闭连接了
*/
enum class Http_Conn::HTTP_CODE : int
{
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    NO_RESOURCE,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};
// 从状态机的三种可能状态，即行的读取状态，分别表示
// 1.读取到一个完整的行 2.行出错 3.行数据尚且不完整
enum class Http_Conn::LINE_STATUS : int
{
    LINE_OK,
    LINE_BAD,
    LINE_OPEN
};

// 构造函数
Http_Conn::Http_Conn()
{
    m_epollFd = -1;
    m_sockFd = -1;
    memset(m_IP, 0, sizeof(m_IP));
    m_port = 0;
    m_readBuf = new char[MAX_READ_BUFSIZE * BLOCKS];
    m_writeBuf = new char[MAX_WRITE_BUFSIZE * BLOCKS];
    m_readBufIndex = 0;
    m_writeBufIndex = 0;
    m_writePtr = 0;
    m_carryReadBufPtr = 0;
    m_text = new char[MAX_HEADLER_LEN];
    m_method = new char[5];
    m_url = new char[MAX_URL_LEN];
    m_version = new char[10];
    m_contentLength = 0; // 没有请求内容的时候，请求报文中就没有content字段
    m_checkState = CHECK_STATE::CHECK_STATE_REQUESTLINE;
    m_keepAlive = false;
    m_host = new char[MAX_URL_LEN];
    m_mmapAddr = NULL;
    m_bytesToSend = 0;
    m_bytesHaveSend = 0;
    // myLog.init();
}

// 析构函数
Http_Conn::~Http_Conn()
{
    delete[] m_readBuf;
    delete[] m_writeBuf;
    m_destroy();
}
// 对epollfd和客户端信息初始化
void Http_Conn::getClientInfo(int epfd, int cfd, char *ip, int port)
{
    m_epollFd = epfd;
    m_sockFd = cfd;
    strcpy(m_IP, ip);
    m_port = port;
}
// 关闭客户端连接
void Http_Conn::closeConn()
{
    if (m_sockFd != -1)
    {
        // 从epfd中移除sockfd
        epollDel(m_epollFd, m_sockFd);
        // 从操作系统中释放fd
        close(m_sockFd);
        // 清空对象中的变量
        m_init();

        m_sockFd = -1;
        // 计数减一
        clientCount--;
    }
    else
    {
        // // printf("尝试对一个非法sockfd进行关闭操作，失败\n");
        // printf("%s", "尝试对一个非法sockfd进行关闭操作，失败\n");
    }
}
// 成员函数
// 从内核中读取数据，对recv函数的封装
bool Http_Conn::m_read()
{
    // 循环读取数据
    // 无论是使用read还是recv，在网络通信的过程中，存在读取的数据长度小于要求的长度的情况，请注意
    if (m_readBufIndex > MAX_READ_BUFSIZE)
    {
        // // printf("error: no more space to recv\n");
        // printf("%s", "error: no more space to recv\n");
        return false;
    }
    int bytesRead = 0;
    while (true)
    {
        bytesRead = recv(m_sockFd, m_readBuf + m_readBufIndex, MAX_READ_BUFSIZE - m_readBufIndex, 0);
        if (bytesRead == -1)
        {
            /*
             the socket is nonblocking (see fcntl(2)),
             in which case the value -1 is returned
             and the external variable errno is set to EAGAIN or EWOULDBLOCK.
            */
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 发生这种情况就是没有数据读了
                break;
            }
            // 否则的话就是有其他错误
            // printf("recv error\n");

            return false;
        }
        if (bytesRead == 0)
        {
            // 对端关闭了连接
            // printf("%s", "client close the connection\n");
            return false;
        }
        m_readBufIndex += bytesRead;
    }
    // 测试打印
    // printf("recv data:\n%s\n", m_readBuf);
    return true;
}

// 向缓冲区中写入数据.并且由函数的返回值，在mian.cpp中决定是否关闭客户端连接
bool Http_Conn::m_write()
{
    // 封装send函数

    /*
    理一下m_write函数的逻辑：
    分两种情况：
        - 1.不发送实际的内容（文字、图片、HTML文件等），只是向客户端发送错误信息
        - 2.完整的响应报文：响应行+响应首部字段+响应主体（文字、图片等资源）
        我们可以发现，两种情况的差别，在于一个需要发送大型数据（图片文档等），另一个不需要。

        响应行+响应首部字段我们是放在【m_writeBuf】中进行发送的，如果是返回一些错误信息，错误信息的说明
        我们也是放在【m_writeBuf】中

        如果要发送图片，图片是保存在操作系统的磁盘上，在用户程序中，我们通过open打开得到相关联的fd进行读和写
    */

    // 使用writev分散写数据
    // 异常判断
    if (m_bytesToSend <= 0)
    {
        // 没有数据要发送
        // printf("%s", "没有数据要发送\n");
        ;
        // 把所有的成员变量重新初始化一遍
        m_init();
        // 更新sockfd为读状态
        epollMod(m_epollFd, m_sockFd, EPOLLIN);
        return true;
    }
    int writevRet = 0;
    while (1)
    {
        writevRet = writev(m_sockFd, m_iov, m_iovCnt);
        if (writevRet == -1)
        {
            // 调用writev函数时发生了错误，如果错误号为EAGAIN
            if (errno == EAGAIN)
            {
                // 重置一次写事件，退出write后再让程序写一次
                epollMod(m_epollFd, m_sockFd, EPOLLOUT);
                return true;
            }
            else
            {
                // 否则就是其他错误了，这个写事件就作废了
                // 解除内存映射
                munmap(m_mmapAddr, m_fileInfo.st_size);
                return false;
            }
        }
        // 更新以下两个变量
        m_bytesHaveSend += writevRet;
        m_bytesToSend -= writevRet;

        if (m_bytesToSend > 0)
        {
            // 需要更新m_iov中的base和len
            if (m_bytesHaveSend > m_iov[0].iov_len)
            {
                m_iov[1].iov_base = m_mmapAddr + (m_bytesHaveSend - m_iov[0].iov_len);
                m_iov[1].iov_len = m_bytesToSend;
            }
            else
            {
                m_iov[0].iov_base = m_writeBuf + m_bytesHaveSend;
                m_iov[0].iov_len = m_writeBufIndex - m_bytesHaveSend;
            }
        }
        else
        {
            // 没有数据需要发送了
            // printf("%s", "数据发送完毕！\n");
            // 解除内存映射
            munmap(m_mmapAddr, m_fileInfo.st_size);
            // 更新sockfd为读状态
            epollMod(m_epollFd, m_sockFd, EPOLLIN);
            if (m_keepAlive)
            {
                // 长连接，这个socket上还会再次发送来数据
                m_init();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

// 静态成员变量的初始化
int Http_Conn::clientCount = 0;

// 成员函数：传递给线程池中线程的函数
void Http_Conn::process()
{
    // printf("%s", "现在是process()函数\n");
    // m_init();
    // 解析HTTP请求
    // printf("执行process\n");
    HTTP_CODE parseRet = httpParse();
    if (HTTP_CODE::BAD_REQUEST == parseRet)
    {
        // 返回值不正确，更新sockfd在epfd中的读状态
        // 也就是这一次的请求数据有问题，让客户端再次发送一遍请求数据
        // printf("请求报文错误\n");
        epollMod(m_epollFd, m_sockFd, EPOLLIN);
        return;
    }
    // parseRet 只有两个状态：BAD_REQUEST 或者 GET_REQUEST
    // 执行响应解析函数
    HTTP_CODE resStatus = responseRequest();
    // 正确解析了请求
    // 生成响应
    bool responseRet = httpResponse(resStatus);
    if (false == responseRet)
    {
        // 发送响应报文出现了错误，直接关闭这个客户端连接？
        closeConn();
        return;
    }
    // 更新【写】状态
    epollMod(m_epollFd, m_sockFd, EPOLLOUT);
    // printf("退出process\n");
    return;
}
// 变量的初始化和销毁
void Http_Conn::m_init()
{
    // printf("%s", "现在是m_init()函数\n");
    // memset(m_readBuf, 0, m_readBufIndex);
    m_readBufIndex = 0;
    // memset(m_writeBuf, 0, m_writeBufIndex);
    m_writeBufIndex = 0;
    m_carryReadBufPtr = 0;
    memset(m_text, 0, sizeof(m_text));
    memset(m_method, 0, sizeof(m_method));
    memset(m_url, 0, sizeof(m_url));
    memset(m_version, 0, sizeof(m_version));
    m_contentLength = 0;
    m_checkState = CHECK_STATE::CHECK_STATE_REQUESTLINE;
    memset(m_host, 0, sizeof(m_host));
    // memset(m_mmapAddr, 0, sizeof(m_mmapAddr));
    m_mmapAddr = NULL;
    // printf("退出m_init()函数\n");

    // 将客户端端口设置为端口复用
    // int reuse = 1;
    // if (m_sockFd > 0)
    // {
    //     setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    // }
}

void Http_Conn::m_destroy()
{
    delete[] m_url;
    delete[] m_version;
    delete[] m_method;
    delete[] m_text;
    delete[] m_host;
}

// 格式化处理m_readBuf中的数据
char *Http_Conn::m_getline()
{
    // printf("%s", "现在是getline函数\n");
    // test m_readBuf
    // // printf("m_readBuf 总数据：%s\n", m_readBuf);
    // 每调用一次这个函数，就从m_readBuf中读取一行的内容
    if (m_carryReadBufPtr >= m_readBufIndex)
    {
        // 超过了m_readBuf的读取字节数
        // 不能再读了，但是仍旧需要返回一个字符串，最后被headlerRequest处理
        // printf("%s", "没有数据给m_getline()读取了\n");
        memset(m_text, 0, sizeof(m_text));
        m_text[0] = '\0';
        return m_text;
    }
    int m_textIndex = 0;
    char ch;
    // // printf("m_readBufIndex:%d, m_carrym_readBufPtr:%d\n", m_readBufIndex, m_carryReadBufPtr);
    // // printf("from m_carrym_readBufPtr, get %s\n", m_carrym_readBufPtr);
    for (;;)
    {
        ch = m_readBuf[m_carryReadBufPtr++];
        if (ch == '\n')
        {
            // 读完了一整行的数据
            m_text[m_textIndex] = '\0'; // 在字串符的末尾添加一个
            break;
        }
        m_text[m_textIndex++] = ch;
    }
    // // printf("getline 读取到的数据：%s\n", m_text);
    return m_text;
}

// http请求报文解析函数
Http_Conn::HTTP_CODE Http_Conn::httpParse()
{
    // 数据是保存在m_readBuf中的
    // 使用有限状态机依次读取
    // 定义一个char* 变量用于处理请求报文
    // printf("%s", "现在是httpRequest()函数\n");
    HTTP_CODE ret = HTTP_CODE::NO_REQUEST;
    char *text = 0;
    while (m_checkState != CHECK_STATE::CHECK_STATE_CONTENT)
    {
        text = m_getline();
        switch (m_checkState)
        {
        case CHECK_STATE::CHECK_STATE_REQUESTLINE:
            // 请求行
            // // printf("进入状态机CHECK_STATE_REQUESTLINE\n");
            ret = requestParse(text);
            if (HTTP_CODE::BAD_REQUEST == ret)
            {
                // 异常处理
                // printf("%s", "error in requestParse\n");
                return HTTP_CODE::BAD_REQUEST;
            }
            // 切换状态
            m_checkState = CHECK_STATE::CHECK_STATE_HEADER;
            break;

        case CHECK_STATE::CHECK_STATE_HEADER:
            // 首部字段
            // // printf("进入状态机CHECK_STATE_HEADER\n");
            ret = headerParse(text);
            if (HTTP_CODE::BAD_REQUEST == ret)
            {
                // 异常处理
                // printf("%s", "error in headerParse\n");
                return HTTP_CODE::BAD_REQUEST;
            }
            else if (HTTP_CODE::GET_REQUEST == ret)
            {
                // 请求报文已经解析完毕
                fflush(NULL);
                // printf("this is test for headlerRequest: m_host:%s, m_contentLength:%d, m_keepalive:%d\n", m_host, m_contentLength, m_keepAlive);
                return HTTP_CODE::GET_REQUEST;
            }
            else if (HTTP_CODE::NO_REQUEST == ret)
            {
                // 请求未完成有两种状态：
                // 一是首部字段的解析还未完成，二是正文的解析还未完成
                // 两者的区别在什么？
                if (text[0] == '\0' && m_contentLength > 0)
                {
                    m_checkState = CHECK_STATE::CHECK_STATE_CONTENT;
                }
                break;
            }
            break;

        case CHECK_STATE::CHECK_STATE_CONTENT:
            // // printf("进入状态机CHECK_STATE_CONTENT\n");
            ret = contentParse(text);
            if (HTTP_CODE::BAD_REQUEST == ret)
            {
                // 异常处理
                // printf("%s", "error in contentParse\n");
                return HTTP_CODE::BAD_REQUEST;
            }
            else if (HTTP_CODE::GET_REQUEST == ret)
            {
                return HTTP_CODE::GET_REQUEST;
            }
            break;

        default:
            // printf("%s", "error in httpParse\n");
            return HTTP_CODE::BAD_REQUEST;
            break;
        }
    }
    return HTTP_CODE::NO_REQUEST;
}

// 解析http请求报文的[请求行]（第一行）的函数
Http_Conn::HTTP_CODE Http_Conn::requestParse(char *text)
{
    // GET /index.html HTTP/1.1
    // printf("%s", "现在进入requestParse函数\n");
    char *strTmp = new char[MAX_URL_LEN];
    int textIndex = 0;
    int strTmpIndex = 0;
    int seq = 0;
    while (1)
    {
        // // printf("进入while循环\n");
        if (textIndex >= strlen(text))
        {
            // // printf("处理到了最后一串字符串\n");
            strTmp[strTmpIndex] = '\0';
            // HTTP/1.1
            strcpy(m_version, strTmp);
            break;
        }
        else if (seq < 2 && (text[textIndex] == ' ' || text[textIndex] == '\t'))
        {
            strTmp[strTmpIndex] = '\0';
            strTmpIndex = 0;
            seq == 0 ? strcpy(m_method, strTmp) : strcpy(m_url, strTmp);
            seq++;
            textIndex++;
        }
        strTmp[strTmpIndex++] = text[textIndex++];
    }
    // m_method 只能是GET，暂时不支持其他方法
    if (strcmp(m_method, "GET") != 0)
    {
        // error
        return HTTP_CODE::BAD_REQUEST;
    }
    // 处理一下url，去除掉URL开头的
    if (strncasecmp(m_url, "http://", 7) == 0)
    {
        // URL格式正确
        m_url += 7; // 向后移动指针的位置
        char *check = strchr(m_url, '/');
        if (check == NULL || check[0] == '/')
        {
            // URL格式不正确
            return HTTP_CODE::BAD_REQUEST;
        }
    }
    // 打印输出一下
    // printf("requestParse, m_method:%s, m_url:%s, m_version:%s\n", m_method, m_url, m_version);
    delete[] strTmp;
    return HTTP_CODE::NO_REQUEST; //

    // 获取资源地址
}
// 解析http请求报文的[首部字段]的函数
Http_Conn::HTTP_CODE Http_Conn::headerParse(char *text)
{
    // 在while循环中，text一行一行的读取m_readBuf中的内容
    /*
    在这个函数，我们先主要关心以下几个内容
    Connection字段
    Content-Length字段
    Host字段
    */
    // // printf("现在进入healderParse函数\n");
    if (text[0] == '\0')
    {
        // printf("%s", "首部字段解析完毕！\n");
        if (m_contentLength != 0)
        {
            m_checkState = CHECK_STATE::CHECK_STATE_CONTENT;
            return HTTP_CODE::NO_REQUEST;
        }
        // 请求完成
        return HTTP_CODE::GET_REQUEST;
    }
    // Connection字段
    else if (strncasecmp(text, "Connection:", 11) == 0)
    {
        // printf("%s", "匹配上了Connection字段\n");
        // 匹配上了
        text += 11;
        // 如果有\t字符还有匹配掉
        text += strspn(text, " \t");
        if (strncasecmp(text, "keep-alive", 10) == 0)
        {
            // keep-alive也匹配上了
            m_keepAlive = true;
        }
    }
    // Content-length字段匹配
    else if (strncasecmp(text, "Content-Length:", 15) == 0)
    {
        // printf("%s", "匹配上了Content-Length字段\n");
        // 匹配上了
        text += 15;
        text += strspn(text, " \t");
        m_contentLength = atol(text); // 获取主体的长度
    }
    // Host字段匹配
    else if (strncasecmp(text, "Host:", 5) == 0)
    {
        // 匹配上了
        // printf("%s", "匹配上了Host字段\n");
        text += 5;
        text += strspn(text, " \t");
        strcpy(m_host, text);
    }
    else
    {
        // printf("%s", "invalid Headler\n");
    }

    return HTTP_CODE::NO_REQUEST;
}
// 解析http请求报文的[正文]的函数
Http_Conn::HTTP_CODE Http_Conn::contentParse(char *text)
{
    // printf("%s", "现在进入contentParse函数\n");
    // printf("%s\n", text);
    return HTTP_CODE::GET_REQUEST;
}

Http_Conn::HTTP_CODE Http_Conn::responseRequest()
{
    // printf("%s", "responseRequest\n");
    // 在这个函数中，我们需要对请求的资源是否存在进行一个判断
    // 没有资源，返回一个HTTP_CODE::NO_RESOURCE
    // 资源的查找：用stat函数获取资源的属性：存在-》是否可读写-》打开文件
    char absolutePath[50];
    memset(absolutePath, 0, sizeof(absolutePath));
    strcpy(absolutePath, srcRoot);
    strcat(absolutePath, m_url);

    // 判断文件是否存在
    int ret = stat(absolutePath, &m_fileInfo);
    if (ret == -1 && errno == ENOENT)
    {
        // printf("%s", "由stat的返回值&错误码判断：文件不存在\n");
        return HTTP_CODE::NO_RESOURCE;
    }
    //判断是否是一个普通文件
    if (m_fileInfo.st_mode & S_IFREG != S_IFREG)
    {
        // printf("%s", "文件类型错误\n");
        return HTTP_CODE::BAD_REQUEST;
    }
    // 判断文件是否具有可读的权限
    if (m_fileInfo.st_mode & S_IROTH != S_IROTH)
    {
        // printf("%s", "不具有读取权限\n");
        return HTTP_CODE::FORBIDDEN_REQUEST;
    }
    // 文件存在、文件具有读取权限，打开文件
    int fd = open(absolutePath, O_RDONLY);
    // 创建内存映射
    m_mmapAddr = (char *)mmap(0, m_fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    // 关闭文件描述符
    close(fd);
    return HTTP_CODE::FILE_REQUEST;
}

bool Http_Conn::writeResponse(const char *format, ...)
{
    // 可变参数宏va_list
    va_list ap;
    va_start(ap, format);
    // 使用VNs// printf向writeBuf中写入数据
    int writeLen = vsnprintf(m_writeBuf + m_writeBufIndex,
                             MAX_WRITE_BUFSIZE - m_writeBufIndex - 1, format, ap);

    if (writeLen > (MAX_WRITE_BUFSIZE - m_writeBufIndex - 1))
    {
        // 正常情况下不能够写入比指定的数据长度还要大的数据
        // printf("%s", "error in vsn// printf\n");
        va_end(ap);
        return false;
    }
    // 更新writeBuf的指针
    m_writeBufIndex += writeLen;
    va_end(ap);
    return true;
}

// 写入状态行的函数
bool Http_Conn::writeStatus(int status, const char *text)
{
    return writeResponse("%s %d %s\r\n", "HTTP/1.1", status, text);
}

// 在首部字段函数中实际执行相应填写的函数
bool Http_Conn::writeContentLength(const int len)
{
    return writeResponse("Content-Length: %d\r\n", len);
}
bool Http_Conn::writeContentType()
{
    // 目前支持最简单的html文件格式，也许以后会支持其他格式的文件
    return writeResponse("Content-Type: %s\r\n", "text/html");
}
bool Http_Conn::writeKeepAlive()
{
    return writeResponse("Connection: %s\r\n", m_keepAlive == true ? "keep-alive" : "close");
}
bool Http_Conn::writeBlankLine()
{
    return writeResponse("\r\n");
}
// 写入首部字段的函数
bool Http_Conn::writeHeader(const int len)
{
    // 响应报文的首部字段：
    // 这里只写几个比较常规的字段
    if (writeContentLength(len) && writeContentType() && writeKeepAlive() && writeBlankLine())
    {
        return true;
    }
    else
    {
        return false;
    }
}
// 写入报文主体的函数
bool Http_Conn::writeContent(const char *text)
{
    return writeResponse("%s\r\n", text);
}
/*
    服务器处理HTTP请求的可能结果，报文解析的结果
    NO_REQUEST          :   请求不完整，需要继续读取客户数据
    GET_REQUEST         :   表示获得了一个完成的客户请求
    BAD_REQUEST     :   表示客户请求语法错误
    FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
    FILE_REQUEST        :   文件请求,获取文件成功
    NO_RESOURCE         :   表示服务器没有资源
    INTERNAL_ERROR      :   表示服务器内部错误
    CLOSED_CONNECTION   :   表示客户端已经关闭连接了
*/
bool Http_Conn::httpResponse(Http_Conn::HTTP_CODE ret)
{
    // printf("%s", "httpResponse\n");
    // printf("HTTP_CODE:%d\n", ret);
    switch (ret)
    {

    // 400
    case HTTP_CODE::BAD_REQUEST:
        writeStatus(400, error_400_title);
        writeHeader(strlen(error_400_form));
        writeContent(error_400_form);

        break;
    // 403
    case HTTP_CODE::FORBIDDEN_REQUEST:
        writeStatus(403, error_403_title);
        writeHeader(strlen(error_403_form));
        writeContent(error_403_form);
        break;
    // 404
    case HTTP_CODE::NO_RESOURCE:
        // printf("%s", "no resource\n");
        writeStatus(404, error_404_title);
        writeHeader(strlen(error_404_form));
        writeContent(error_404_form);
        break;
    // 500
    case HTTP_CODE::INTERNAL_ERROR:
        writeStatus(500, error_500_title);
        writeHeader(strlen(error_500_form));
        writeContent(error_500_form);
        break;
    // 200
    case HTTP_CODE::FILE_REQUEST:
        writeStatus(200, ok_200_title);
        writeHeader(m_fileInfo.st_size);
        m_iov[0].iov_base = m_writeBuf;
        m_iov[0].iov_len = m_writeBufIndex;
        m_iov[1].iov_base = m_mmapAddr;
        m_iov[1].iov_len = m_fileInfo.st_size;
        m_iovCnt = 2; // 有两块内存的数据需要一起传输
        m_bytesToSend = m_writeBufIndex + m_fileInfo.st_size;
        return true;
        break;

    default:
        return false;
        break;
    }
    m_iov[0].iov_base = m_writeBuf;
    m_iov[0].iov_len = m_writeBufIndex;
    m_iovCnt = 1; // 只有一块内存需要传输
    m_bytesToSend = m_writeBufIndex;
    // printf("%s", "退出httpResponse\n");
    return true;
}
