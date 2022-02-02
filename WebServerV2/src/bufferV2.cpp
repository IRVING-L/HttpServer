#include "../lib/bufferV2.h"

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
// 返回缓冲区的大小
size_t Buffer::_size() const
{
    return m_end - m_elements;
}
//缓存区中可以读取的字节数
size_t Buffer::writeableBytes() const
{
    return _size() - m_writePos;
}
//缓存区中可以写入的字节数
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
std::string Buffer::_all2str()
{
    std::string str(m_elements, m_writePos);
    _init();
    return str; // 为什么这里可以返回一个局部变量？
}
// 私有成员函数
// 返回当前写指针的位置
char *Buffer::curWritePtr()
{
    return m_elements + m_writePos;
}
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
