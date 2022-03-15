#include "../lib/threadpoolV4.h"


void ThreadPool::worker()
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
}

ThreadPool::ThreadPool(int t_num, int j_num)
:m_threadNum(t_num), m_queMaxSize(j_num), m_resource(0, 0), m_resEmpty(0, m_queMaxSize), m_mtx(0,1), m_stop(false) 
{
    for(int i = 0; i < t_num; ++i)
    {
        m_workThreads.push_back(std::move(std::thread(&ThreadPool::worker, this)));
    }
}

ThreadPool::~ThreadPool()
{
    m_stop = true;
    for(int i = 0; i < m_threadNum; ++i)
    {
        if(m_workThreads[i].joinable()) { m_workThreads[i].join(); }
    }
}