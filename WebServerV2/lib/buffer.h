#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <atomic>
#include <unistd.h>  //read() write()
#include <sys/uio.h> //readv() writev()
#include <assert.h>
/*
这是一个用vector<char>做的缓冲区
性能应该是比string要好的
但是会差与char数组吧
*/
class Buffer
{
public:
    Buffer(int bufsize = 1024);
    ~Buffer();

    // 将数据写入到缓冲区
    void append(const char *str, size_t len);
    void append(const std::string &str);
    void append(const void *data, size_t len);
    void append(const Buffer &buf);

private:
    // 成员变量
    std::vector<char> m_buffer;
    std::atomic<int> m_readPos;
    std::atomic<int> m_writePos;
};
#endif