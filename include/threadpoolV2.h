#ifndef __THREADPOOLV2_H__
#define __THREADPOOLV2_H__

#include <queue>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
/*
线程池第二版：
    - 设计思路：
        - 牢记线程池的本质是生产者/消费者模型。
        线程消费资源（需要执行的任务函数）；外部调用append函数生产资源（添加需要执行的函数）
        - 对外接口：
            - append函数：使用函数模板。支持传入不同类型的函数。
        - 对内。使用了C++11库<mutex>, <thread>，这两个库的功能是线程同步（上锁）和创建管理线程
            其次，定义了一个工作队列，队列中的元素类型为function<void()> 多态函数适配器
            对工作队列的长度没有限制。（也许会是一个设计隐患）
*/

class Threadpool
{
private:
    int m_threadNum;
    int m_jobQueueNum;
    bool m_isStop;
    std::mutex m_mtx;
    std::condition_variable m_cond;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobQueue;
    void worker();

public:
    explicit Threadpool(int t_num = 8, int j_num = 200);
    ~Threadpool();

    template <class F>
    void append(F &&arg)
    {
        {
            std::unique_lock<std::mutex> m_lk(m_mtx);
            std::cout << "添加了新任务\t";
            m_jobQueue.emplace(std::forward<F>(arg));
            std::cout << "m_jobQueue size = " << m_jobQueue.size() << std::endl;
        }
        m_cond.notify_one();
    }
};

#endif