#include "../lib/buffer.h"

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
        std::cout << "location:bufferV2.h:append,"
                  << "error:invalid str"
                  << std::endl;
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
        std::cout << "location:bufferV2.h:append,"
                  << "error:invalid str"
                  << std::endl;
        return;
    }
    append(static_cast<const char *>(data), len);
}
// 从sockfd中读取数据到缓冲区
ssize_t Buffer::recvFd(int sockfd, int *Errno)
{
    
    //printf("function:Buffer::recvFd(int sockfd, int *Errno)\n");
    // 当前版本暂时不使用分块读，分块读的必要性只有在支持POST时才能发挥作用
    // 由于针对的是非阻塞I/O + ET模式的读取，读取时需要使用while读取到不能读取为止
    ssize_t len = 0;
    ssize_t recvBytes = 0;
    // 设定一个缓冲区的缓冲区，这意味着，目前WebServer一次性最大能接受的数据长度不得超过64KB
    // 但是如果发送的数据超过了能够读取的大小，会造成数据丢失
    // 所以这里的设计并不是很合理，暂时先保留着，后续看看怎么改
    char temBuf[65536] = {0}; 
    while (true)
    {
        len = recv(sockfd, temBuf + recvBytes, 65535 - recvBytes, 0);
        if (len == -1)
        {
            // errno == EWOULDBLOCK 表示目前发送方没有数据可供读取
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
        recvBytes += len;
    }
    // 将tmpbuf中的数据调用append写入到缓冲区中
    // printf("recvFd():%s\n", temBuf);
    append(temBuf, recvBytes);
    return recvBytes;
}
ssize_t Buffer::writeFd(int sockfd, int *Errno)
{
    // 调用send函数，向sockfd中发送数据
    ssize_t len = 0;
    ssize_t sendBytes = 0;
    while(true)
    {
        len = send(sockfd, m_elements + readBytes(), readableBytes() - len, 0);
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
size_t Buffer::readBytes() const
{
    return m_readPos;
}
//缓冲区中已经写入的字节数
size_t Buffer::writeBytes() const
{
    return m_writePos;
}
void Buffer::updateReadPtr(size_t pos)
{
    m_readPos += pos;
}
std::string Buffer::_all2str()
{
    m_str.assign(m_elements, writeBytes());
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
        std::cout << "location:bufferV2.h:ensureW,"
                  << "error:扩容失败"
                  << std::endl;
    }
}
// 扩容
void Buffer::allocateSpace(size_t len)
{
    size_t newlen = 2 * (writeBytes() + len);
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
