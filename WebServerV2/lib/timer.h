#ifndef __TIMER_H__
#define __TIMER_H__
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>
/*
简要说明：
    - 这是一个定时清理非活跃连接的头文件，实现原理简单概括起来：
        - 使用【堆】这个数据结构，把每个socket连接上次活跃的时间按照大小保存在大/小
        根堆中，可以设置一个定时器，时间到了自动去检查过期的socket连接
*/

typedef std::chrono::milliseconds _ms;
typedef std::chrono::high_resolution_clock _clock;
typedef _clock::time_point _timeStamp;
typedef std::function<void()> _timeoutCallBack;
class Timenode
{
public:
    int m_id;          // socket通信方
    _timeStamp m_expire;   // 过期时间（应该是从元年到当前经过的时间）
    _timeoutCallBack m_cb; // 回调函数。功能暂时不清楚
    // 运算符重载
    bool operator<(const Timenode &t)
    {
        return m_expire < t.m_expire;
    }
};

class TimeManager
{
    typedef std::shared_ptr<Timenode> SP_Timenode;

public:
    TimeManager(int heapNum = 64);
    ~TimeManager();

    void addTimer(int id, int timeout, const _timeoutCallBack &cb);
    void handleExpiredEvent();
    int getNextHandle();
    void updateTimer(int id, int timeout);
    // 删除指定id节点，并且用指针触发处理函数
    void work(int id);
    void pop();
    void clear();
private:
    void m_del(int i);
    void swim(int i);
    bool sink(int i, int n);
    void swapNode(int i, int j);
    std::vector<Timenode> m_heap;
    /*
    key-value ---> id-i. i is the pos of m_heap elements
    */
    std::unordered_map<int, int> m_ref; // 映射
};

#endif