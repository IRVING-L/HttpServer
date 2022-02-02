#include "../lib/buffer.h"

// 构造函数
Buffer::Buffer(int bufsize)
    : m_buffer(bufsize), m_readPos(0), m_writePos(0) {}
// 析构函数
Buffer::~Buffer() 
{
    m_buffer.clear();
}

// 公有成员函数
void Buffer::append(const char *str, size_t len)
{
    if (str == nullptr)
    {
        std::cout << "location:buffer.h:append,"
                  << "error:invalid str"
                  << std::endl;
        return;
    }
    // 确保有足够的空间写入长度为len的数据
    
}