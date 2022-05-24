#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <vector>
/*
说明：
epoll是I/O多路复用技术中，性能最好的一个。
封装epoll中的几个函数：创建、添加、修改、删除
对外留出接口，在其他对象中，需要操作的epoll对象内容：
    - 需要访问events数组中的内容
        - 具体来讲就是发生改变的fd
        - 该fd对应的event
*/

// struct epoll_event
// {
//     uint32_t events;   /* Epoll events */
//     epoll_data_t data; /* User data variable */
// };

class Epoller
{
public:
    explicit Epoller(int maxevents = 1024);
    ~Epoller() = default;
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