#ifndef __BUFFERV2_H__
#define __BUFFERV2_H__
#include <iostream>
#include <string>
#include <alloca.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h> /* bzero */
#include <sys/types.h>
#include <sys/socket.h> /* recv */
/*
其他giter大佬们的buffer，是使用vector封装char做的
我个人认为既然都要自己实现一个内存管理类了，不如更进一步，使用char数组做底层的数据结构
自己动手写扩容啊什么的函数，正好可以使用C++11之后的std::move以及allocator类等
权当是锻炼啦

- buffer目前只能接收
*/
#define BUF_SIZE_INIT 128
class Buffer
{
public:
    Buffer(int bufsize = BUF_SIZE_INIT);
    ~Buffer();
    // 向缓冲区写入数据
    void append(const char *str, size_t len);
    void append(const std::string &str);
    void append(const void *data, size_t len);
    // void append(const Buffer &buf);
    // 将缓冲区中的数据，发送到socket通信的对端
    ssize_t writeFd(int sockfd, int *Errno);
    // 从socket通信的对端，接收数据到缓冲区
    ssize_t recvFd(int sockfd, int *Errno);

    size_t _size() const;
    //缓存区中可以读取的字节数
    size_t writeableBytes() const;
    //缓存区中可以写入的字节数
    size_t readableBytes() const;
    //缓存区中已经读取的字节数
    size_t readBytes() const;
    //缓冲区中已经写入的字节数
    size_t writeBytes() const;
    // 缓冲区头。有了缓冲区头+写入的数据长度，便可以在类外使用writev函数进行写入
    const char *beginPtr() const;
    // 当前的读取指针的位置
    const char *curReadPtr() const {return m_elements + m_readPos;}
    // 当前的写入指针的位置
    const char *curWritePtr() const {return m_elements + m_writePos;}
    // 更新读指针的位置：读指针是对外使用的接口，外部调用读取数据，要及时更新指针的位置
    void updateReadPtr(size_t pos);
    // 将缓冲区中的内容输出成string
    std::string _all2str();

private:
    // 检查空间是否足够
    void ensureWriteable(size_t len);
    // 获得更多内存并拷贝已有元素
    void allocateSpace(size_t len);
    // 销毁元素并释放内存
    void _free();
    // 将指针和变量初始化
    void _init();
    // 底层的数据结构，char数组
    // char数组的头指针
    char *m_elements;
    // char数组的尾指针
    char *m_end;
    // string
    std::string m_str;

    size_t m_readPos;
    size_t m_writePos;
};

#endif
