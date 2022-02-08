#include "../lib/httpconnect.h"

// 静态static成员的定义
bool Httpconnection::isET = false;
const char *Httpconnection::srcDir = "/root/serverProjectNC/WebServerV2";
std::atomic<int> Httpconnection::userCount(0);

Httpconnection::Httpconnection()
{
    m_fd = -1;
    m_addr = {0};
    isClose = true;
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
    do
    {
        len = writev(m_fd, m_iov, m_iovCnt);
        if(len < 0)
        {
            *saveErrno = errno;
            break;
        }
        if(m_iov[0].iov_len + m_iov[1].iov_len == 0)
        {
            break; // 传输结束
        }
        else if(static_cast<size_t>(len) > m_iov[0].iov_len)
        {
            m_iov[1].iov_base = m_iov[1].iov_base + (len - m_iov[0].iov_len);
            m_iov[1].iov_len -= (len - m_iov[0].iov_len);
            if(m_iov[0].iov_len)
            {
                m_writeBuf._init();
                m_iov[0].iov_len = 0;
            }
        }
        else
        {
            m_iov[0].iov_base = m_iov[0].iov_base + len;
            m_iov[0].iov_len -= len;
            m_writeBuf.updateReadPtr(static_cast<size_t>(len));
        }
    }while(isET || writeBytes() > 10240);
    return len;
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
    m_request._init();
    if(m_readBuf.readableBytes() <= 0)
    {
        return false;
    }
    else if(m_request.parse(m_readBuf))
    {
        // 解析完成后，调用response模块进行响应
        m_response.init(srcDir, m_request.path(), m_request.isKeepAlive(), 200);
    }
    else
    {
        std::cout << "400" << std::endl;
        m_response.init(srcDir, m_request.path(), false, 400);
    }
    m_response.makeResponse(m_writeBuf);
    m_iov[0].iov_base = const_cast<char *>(m_writeBuf.curReadPtr());
    m_iov[0].iov_len = m_writeBuf.readableBytes();

    if(m_response.fileLen() > 0 && m_response.file() )
    {
        m_iov[1].iov_base = m_response.file();
        m_iov[1].iov_len = m_response.fileLen();
        m_iovCnt = 2;
    } 
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