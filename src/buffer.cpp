#include "buffer.h"
#include "log.h"
Buffer::Buffer(int bufsize)
{
    m_elements = new char[bufsize];
    assert(m_elements);
    m_end = m_elements + bufsize;
    m_readPos = 0;
    m_writePos = 0;
}

Buffer::~Buffer()
{
    _free();
}

// 成员函数
/* 向缓冲区写入数据
    目前只支持三种传入参数：
        - char*
        - string
        - void*
*/
void Buffer::append(const char *str, size_t len)
{
    //printf("function:Buffer::append(const char *str, size_t len)\n");
    if (str == nullptr)
    {
        LOG_ERROR("error:invalid str");
        return;
    }
    ensureWriteable(len);
    // 移动元素
    for (size_t i = 0; i < len; ++i)
    {
        m_elements[m_writePos + i] = std::move(str[i]);
    }
    m_writePos += len; // 更新写指针的位置
}
void Buffer::append(const std::string &str)
{
    append(str.data(), str.length());
}
void Buffer::append(const void *data, size_t len)
{
    if (data == nullptr)
    {
        LOG_ERROR("error:invalid str");
        return;
    }
    append(static_cast<const char *>(data), len);
}
// 从sockfd中读取数据到缓冲区
// 返回值：读取正确，返回读取到的字节数；读取错误，对端断线，返回-1
ssize_t Buffer::recvFd(int sockfd, int *Errno)
{
    // 使用readv进行读取
    char extraBuf[65536] = {0}; // 额外扩展空间
    // buffer剩余空闲
    ssize_t writeable = writeableBytes();
    // 分散读写的结构体
    struct iovec iov[2];
    // 装填结构体
    iov[0].iov_base = const_cast<char *>(curWritePtr());
    iov[0].iov_len = writeable;
    iov[1].iov_base = extraBuf;
    iov[1].iov_len = sizeof(extraBuf);
    // 调用readv
    int len = readv(sockfd, iov, 2);
    if(len < 0)
    {
        // 错误发生
        Errno = &errno;
    }
    else if(len <= writeable)
    {
        // 读取到的数据，没有写入到extraBuf中国
        m_writePos += len;
    }
    else
    {
        m_writePos = _size();
        append(extraBuf, len - writeable);
    }
    return len;
}
ssize_t Buffer::writeFd(int sockfd, int *Errno)
{
    // 调用send函数，向sockfd中发送数据
    ssize_t len = 0;
    ssize_t sendBytes = 0;
    while(true)
    {
        len = send(sockfd, m_elements + haveReadBytes(), readableBytes() - len, 0);
        if (len == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                *Errno = errno;
                break;
            }
            *Errno = errno;
            return -1;
        }
        if (len == 0)
        {
            // 对端关闭了连接
            *Errno = errno;
            return -1;
        }
        m_readPos += len;
        sendBytes += len;
    }
    return sendBytes;
}
// 返回缓冲区的大小
size_t Buffer::_size() const
{
    return m_end - m_elements;
}
//缓存区中可以写入的字节数
size_t Buffer::writeableBytes() const
{
    return _size() - m_writePos;
}
//缓存区中可以读取的字节数
size_t Buffer::readableBytes() const
{
    return m_writePos - m_readPos;
}
//缓存区中已经读取的字节数
size_t Buffer::haveReadBytes() const
{
    return m_readPos;
}
//缓冲区中已经写入的字节数
size_t Buffer::haveWriteBytes() const
{
    return m_writePos;
}
void Buffer::updateReadPtr(size_t pos)
{
    m_readPos += pos;
}
std::string Buffer::_all2str()
{
    m_str.assign(m_elements, haveWriteBytes());
    _init();
    return m_str; // 为什么这里可以返回一个局部变量？
}
// 缓冲区头
const char *Buffer::beginPtr() const
{
    return m_elements;
}
// 私有成员函数
// 检查是否有足够的写入空间
void Buffer::ensureWriteable(size_t len)
{
    size_t res = _size() - m_writePos;
    if (res < len)
    {
        allocateSpace(len);
    }
    res = _size() - m_writePos;
    if (res < len)
    {
        LOG_ERROR("扩容失败");
    }
}
// 扩容
void Buffer::allocateSpace(size_t len)
{
    size_t newlen = 2 * (haveWriteBytes() + len);
    char *newdata = new char[newlen];
    for (size_t i = 0; i < m_writePos; ++i)
    {
        newdata[i] = std::move(m_elements[i]);
    }
    delete[] m_elements;
    m_elements = newdata;
    m_end = m_elements + newlen;
}

void Buffer::_free()
{
    delete[] m_elements;
    _init();
}
void Buffer::_init()
{
    bzero(m_elements, _size());
    m_readPos = 0;
    m_writePos = 0;
}
