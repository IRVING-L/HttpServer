#ifndef __THREADPOOLV5_H__
#define __THREADPOOLV5_H__
#include <queue>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <semaphore.h>
#include "locker.h"
class ThreadPool
{
public:
    // 构造函数
    explicit ThreadPool(int t_num, int j_num)
    :m_threadNum(t_num), m_queMaxSize(j_num), 
    m_resource(0, 0), m_resEmpty(0, m_queMaxSize), m_mtx(0,1), m_stop(false) 
    {
        for(int i = 0; i < m_threadNum; ++i)
        {
            std::thread([this]()
            {
                while(!m_stop)
                {
                    m_resource.wait();
                    m_mtx.wait();

                    auto todoTask = m_jobQueue.front();
                    m_jobQueue.pop();

                    m_resEmpty.post();
                    m_mtx.post();

                    todoTask();
                }
            }).detach();
        }
    }
    // 析构函数
    ~ThreadPool()
    {
        m_stop = true;
    }
    // 对外接口：向线程池中添加任务
    template <class F>
    void append(F &&task)
    {
        m_resEmpty.wait();
        m_mtx.wait();

        m_jobQueue.emplace(std::forward<F>(task));

        m_resource.post();
        m_mtx.post();
    }
    // 删除我们不需要的拷贝函数
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
private:
    int m_threadNum;
    int m_queMaxSize;
    Locker m_resource;
    Locker m_resEmpty;
    Locker m_mtx;
    bool m_stop;
    std::queue<std::function<void()>> m_jobQueue;
    std::vector<std::thread> m_workThreads;
};
#endif