#include "../lib/httpconnect.h"

// 静态static成员的定义
bool Httpconnection::isET = false;
const char *Httpconnection::srcDir = "/root/myServer/WebServerV3/resources";
std::atomic<int> Httpconnection::userCount(0);

Httpconnection::Httpconnection()
{
    m_fd = -1;
    m_addr = {0};
    isClose = true;
    isGetRightResource = false;
    isHttpDone = false;
}

Httpconnection::~Httpconnection()
{
    closeHTTPConn();
}

void Httpconnection::initHTTPConn(int socketFd, const sockaddr_in &addr)
{
    if (socketFd < 0)
    {
        std::cout << "location:httpconnect.cpp:init, "
                  << "error:invalid fd" << std::endl;
        return;
    }
    userCount++;
    m_addr = addr;
    m_fd = socketFd;
    m_readBuf._init();
    m_writeBuf._init();
    isClose = false;
}

//每个连接中定义的对缓冲区的读写接口
ssize_t Httpconnection::readBuffer(int *saveErrno)
{
   
    //printf("function:Httpconnection::readBuffer(int *saveErrno)\n");
    ssize_t len = -1;
    do
    {
        len = m_readBuf.recvFd(m_fd, saveErrno);
        if(len <= 0)
        {
            break;
        }
    } while (isET);
    return len;
}
ssize_t Httpconnection::writeBuffer(int *saveErrno)
{
    ssize_t len = -1;
    ssize_t _writebytes = 0;
    do
    {
        len = writev(m_fd, m_iov, m_iovCnt);
        if(len < 0)
        {
            *saveErrno = errno;
            return -1;
        }
        if(m_iov[0].iov_len + m_iov[1].iov_len == 0)
        {
            break; // 传输结束
        }
        else if(static_cast<size_t>(len) > m_iov[0].iov_len)
        {
            m_iov[1].iov_base = (char *)m_iov[1].iov_base + (len - m_iov[0].iov_len);
            m_iov[1].iov_len -= (len - m_iov[0].iov_len);
            if(m_iov[0].iov_len)
            {
                m_writeBuf._init();
                m_iov[0].iov_len = 0;
            }
        }
        else
        {
            m_iov[0].iov_base = (char *)m_iov[0].iov_base + len;
            m_iov[0].iov_len -= len;
            m_writeBuf.updateReadPtr(static_cast<size_t>(len));
        }
        _writebytes += len;
    }while(isET || writeBytes() > 10240);
    return _writebytes;
}

//关闭HTTP连接的接口
void Httpconnection::closeHTTPConn()
{
    m_response._unmapFile();
    if(isClose == false)
    {
        isClose = true;
        userCount--;
        close(m_fd);
    }
}
//定义处理该HTTP连接的接口，主要分为request的解析和response的生成
bool Httpconnection::handleHTTPConn()
{
    // printf("function:Httpconnection::handleHTTPConn()\n");
    // 初始化【解析】模块
    m_request._init();
    if(m_readBuf.readableBytes() <= 0)
    {
        return false;
    }
    // 调用【解析】模块进行解析
    else if(m_request.parse(m_readBuf))
    {
        // 解析完成后，调用response模块进行响应
        //std::cout << "srcDir:" << srcDir << " http path:" << m_request.path() << std::endl;
        //std::cout << srcDir + m_request.path() << std::endl;
        // printf("%s\n",(srcDir + m_request.path()).data());
        m_response.init(srcDir, m_request.path(), m_request.isKeepAlive(), 200);
    }
    else
    {
        std::cout << "400" << std::endl;
        m_response.init(srcDir, m_request.path(), false, 400);
    }
    // 开始查找资源，并填写响应报文
    bool ret = m_response.makeResponse(m_writeBuf);
    m_iov[0].iov_base = const_cast<char *>(m_writeBuf.curReadPtr());
    m_iov[0].iov_len = m_writeBuf.readableBytes();

    if(m_response.fileLen() > 0 && m_response.file() )
    {
        //std::cout << "正在装填m_iov分散写结构体\n";
        m_iov[1].iov_base = m_response.file();
        m_iov[1].iov_len = m_response.fileLen();
        m_iovCnt = 2;
    } 
    // 程序运行到这里，就代表http解析已经完成, 成功获取到了资源（正确资源或者网络错误资源）
    if(ret) { isGetRightResource = true; }
    isHttpDone = true;

    return true;
}

//其他方法
const char *Httpconnection::getIP() const
{
    return inet_ntoa(m_addr.sin_addr);
}
int Httpconnection::getPort() const
{
    return ntohs(m_addr.sin_port);
}
int Httpconnection::getFd() const
{
    return m_fd;
}
sockaddr_in Httpconnection::getAddr() const
{
    return m_addr;
}
int Httpconnection::writeBytes()
{
    return m_iov[0].iov_len + m_iov[1].iov_len;
}
bool Httpconnection::isKeepAlive() const
{
    return m_request.isKeepAlive();
}