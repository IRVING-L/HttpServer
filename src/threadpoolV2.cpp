#include "threadpoolV2.h"

void Threadpool::worker()
{
    while (!m_isStop)
    {
        //std::function<void()> todoTask;
        // 线程同步的问题：没有资源的话，应该是阻塞掉线程
        {
            std::unique_lock<std::mutex> m_lk(m_mtx); // 上锁
            if (m_jobQueue.empty())
            {
                m_cond.wait(m_lk);
            }
            /*
            分析问题：
                - 进程同步上锁的顺序搞错了。先判断是否有资源，再竞争访问资格
                - 而不是先竞争访问资格，再判断是否有资源：没有资源就会被阻塞，然后释放锁。
                - 多个线程都处于因无资源导致的阻塞状态，然后资源来了，都去抢，没抢到的就会执行空函数，程序就会崩溃
            */
            std::cout << "m_jobQueue size = " << m_jobQueue.size() << std::endl;
            auto todoTask = m_jobQueue.front();
            m_jobQueue.pop();
            // 手动解锁
            m_lk.unlock();
            // 执行任务函数
            std::cout << "成功获取到任务" << std::endl;
            todoTask();
            // 任务函数执行解锁，上锁
        }
        
    }
}

Threadpool::Threadpool(int t_num, int j_num) : m_threadNum(t_num), m_jobQueueNum(j_num), m_isStop(false)
{
    for (int i = 0; i < t_num; ++i)
    {
        std::cout << "正在创建第" << i << "个线程\n";
        m_threads.emplace_back(std::move(std::thread(&Threadpool::worker, this)));
        
    }
}

Threadpool::~Threadpool()
{
    m_isStop = true;
    m_cond.notify_all();
    for (auto &it : m_threads)
    {
        if (it.joinable())
        {
            it.join();
        }
    }
}
