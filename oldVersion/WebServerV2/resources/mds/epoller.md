

## Epoller的介绍

epoll是I/O多路复用技术的一种，目前常用的I/O多路复用技术有：epoll、select、poll。

I/O多路复用技术可以让服务器同时监听多个客户端发起的socket连接。epoll相较于select和poll，由于其底层的红黑树，不仅可以知道是否有socket通信状态发生了改变，还能够知道有几个socket发生了状态改变。不需要在程序中使用循环遍历所有的socket连接，确定到底是哪一个socket状态发生了改变。这一特点大大提高了服务器监听的效率，也让epoll成为了目前I/O多路复用技术的主流。

与epoll相关的几个函数分别是：
```cpp
// 在内核中创建epoll对象
int epoll_create(int size); 
// 修改epoll中的fd属性：增删改
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event); 
// 监听socket
int epoll_wait(int epfd, struct epoll_event *events,
                      int maxevents, int timeout);
```

使用epoll的基本步骤：
- 调用epoll_create创建epoll对象
- 调用epoll_ctl向epoll中添加需要监听的socket
- 调用epoll_wait监听socket

使用epoll时操作较为频繁的便是向epoll中添加、修改、删除对象以及调用wait监听，我们在Epoller类中可以封装成不同功能的函数，便于在其他外部文件中快速的调用，减少直接调用epoll库函数输入大量参数以及不小心导致的传参错误情况发生

定义的几个函数：
```cpp
    // 成员函数
    int wait(int timeout = -1);
    bool add(int fd, uint32_t ev);
    bool mod(int fd, uint32_t ev);
    bool del(int fd);
```
使用epoll时，需要使用数组保存发生了改变的socket，这里使用vector容器来保存：
```cpp
    // Epoller对象中的epollfd文件描述符
    int m_epollFd;
    // 使用vector保存改变的事件
    std::vector<struct epoll_event> m_epEves;
```

完整的Epoller类如下：
```cpp
#ifndef __EPOLLER_H__
#define __EPOLLER_H__
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <vector>
class Epoller
{
public:
    explicit Epoller(int maxevents = 1024);
    ~Epoller();
    // 成员函数
    int wait(int timeout = -1);
    bool add(int fd, uint32_t ev);
    bool mod(int fd, uint32_t ev);
    bool del(int fd);
    //对外要使用的接口
    int getSockFd(int i) const;
    uint32_t getFdEvent(int i) const;

private:
    // Epoller对象中的epollfd文件描述符
    int m_epollFd;
    // 使用vector保存改变的事件
    std::vector<struct epoll_event> m_epEves;
};
#endif
```