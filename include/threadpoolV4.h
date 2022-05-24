#ifndef __THREADPOOLV4_H__
#define __THREADPOOLV4_H__

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
    explicit ThreadPool(int t_num, int j_num);
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    template<class F>
    void append(F &&task)
    {
        m_resEmpty.wait();
        m_mtx.wait();

        m_jobQueue.emplace(std::forward<F>(task));

        m_resource.post();
        m_mtx.post();
    }
private:

    void worker();

    
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