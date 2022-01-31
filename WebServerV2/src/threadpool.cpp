#include "../lib/threadpool.h"
// 线程执行的m_worker函数
void Threadpool::m_worker(Threadpool *arg)
{
    while (!arg->m_stop)
    {
        std::function<void()> todoTask;
        bool isGet = false;
        //上锁
        {
            std::unique_lock<std::mutex> lk(m_mtx);
            m_cond.wait(lk, [arg]()
                        { return !arg->m_queue.empty(); });
            isGet = arg->m_queue.dequeue(todoTask);
        }
        // 退出作用域 = 释放锁
        // 执行任务todotask
        if (isGet)
        {
            todoTask();
        }
    }
}
Threadpool::Threadpool(int threadNum) : m_stop(false)
{

    for (int i = 0; i < threadNum; ++i)
    {
        m_threads.push_back(std::move(std::thread(&Threadpool::m_worker, this, this)));
    }
}

template <typename F, typename... Args>
auto Threadpool::append(F &&f, Args &&...args)
    -> std::future<decltype(f(args...))>
{
    std::function<decltype(f(args...))()> func =
        std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto taksPtr =
        std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
    std::function<void()> wrapperFunc = [taksPtr]()
    { (*taksPtr)(); };

    m_queue.enqueue(wrapperFunc);
    m_cond.notify_all();
    return taksPtr->get_future();
}

Threadpool::~Threadpool()
{
    /*
    析构函数需要执行的功能：
        - 回收所有的子线程
        - 唤醒所有被条件变量阻塞的进程，让他们完成本该完成的工作
        - 更改m_stop标记
    */
    m_stop = true;
    m_cond.notify_all();
    for (auto &t : m_threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}
