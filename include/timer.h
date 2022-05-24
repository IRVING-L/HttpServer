#ifndef __TIMER_H__
#define __TIMER_H__
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <iostream>
#include "log.h"
/*
简要说明：
    - 这是一个定时清理非活跃连接的头文件，实现原理简单概括起来：
        - 使用【堆】这个数据结构，把每个socket连接上次活跃的时间按照大小保存在小
        根堆中，可以设置一个定时器/每次循环的末尾，时间到了自动去检查过期的socket连接

    - timer.h 是一个额外的头文件，既然我们要实现对连接的清理，需要从epollfd中删除sockfd，
        同时close(sockfd)。由于timemannager类中不包含socket通信双方的信息。所以删除连接的
        的工作我们需要使用一个回调函数cb进行处理。在其他文件中，调用timemanager类时，我们
        通过使用bind(&A::func, &a, ...)，可以将其他类的类内函数传入进来。回调函数实现类与类
        之间的通信的作用就体现了。
*/

typedef std::chrono::milliseconds _ms;
typedef std::chrono::high_resolution_clock _clock;
typedef _clock::time_point _timeStamp;
typedef std::function<void()> _timeoutCallBack;
class Timenode
{
public:
    // 构造函数
    Timenode () = default;
    Timenode(int fd, const _timeStamp &t, const _timeoutCallBack &f):m_id(fd), m_expire(t), m_cb(f) {}
    int m_id;          // socket通信方
    _timeStamp m_expire;   // 过期时间（应该是从元年到当前经过的时间）
    _timeoutCallBack m_cb; // 回调函数。功能暂时不清楚
    
    // 运算符重载
    bool operator<(const Timenode &t)
    {
        return m_expire < t.m_expire;
    }
    bool operator>(const Timenode &t)
    {
        return m_expire > t.m_expire;
    }

    Timenode &operator=(const Timenode &t)
    {
        // LOG_DEBUG("正在执行赋值拷贝构造函数");
        m_id = t.m_id;
        m_cb = t.m_cb;
        m_expire = t.m_expire;
        return *this;
    }
};

class TimeManager
{
    typedef std::shared_ptr<Timenode> SP_Timenode;

public:
    TimeManager();
    ~TimeManager();

    void addTimer(int id, int timeout, const _timeoutCallBack &cb);
    int getNextHandle();
    void updateTimer(int id, int timeout);

    // 删除指定id节点，并且用指针触发处理函数
    void delFd(int fd){
        
        if(m_ref.count(fd))
        {
            int pos = m_ref[fd];
            // LOG_DEBUG("input fd: %d, fd's pos: %d", fd, pos);
            m_del(pos);
        }
        else
        {
            LOG_ERROR("m_ref do not have fd:%d",fd);
            return;
        }
    }
    void work(int id);
    void pop();
    void clear();
private:
    
    void m_del(int i);
    void swim(int i);
    bool sink(int i, int n);
    void swapNode(int i, int j);


    void handleExpiredEvent();

    // 对下面两个数据容器的访问和修改，要上锁
    std::mutex _heapMtx;
    std::mutex _mapMtx;
    std::vector<Timenode> m_heap;
    /*
    key-value ---> id-i. i is the pos of m_heap elements
    */
    std::unordered_map<int, int> m_ref; // 映射
};

#endif