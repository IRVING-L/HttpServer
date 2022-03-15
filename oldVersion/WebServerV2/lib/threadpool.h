#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <queue>
#include <vector>
#include "safequeue.h"
/*
说明：
- 自C++11之后，C++就在语言层面对多线程开发进行了支持，在C++11之上，对多线程开发的常用的线程、
    锁、条件变量等包装了相应的库。相较于LInux C的pthread库函数，主要有两个特点：
    - 一是C++的线程开发基于的是OOP思想，面向对象开发。
    - 二是C++的技巧语法可以排上用场：例如lambda语句、bind语句、尾置函数类型声明、decltype表达式等
*/

class Threadpool
{
public:
    explicit Threadpool(int threadNum = 8);
    ~Threadpool();
    template <typename F, typename... Args>
    auto append(F &&f, Args &&...args)
        -> std::future<decltype(f(args...))>;

private:
    // 成员函数
    void m_worker(Threadpool *arg);
    void m_run();
    // 成员变量
    bool m_stop;
    std::mutex m_mtx;
    std::condition_variable m_cond;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    Safequeue<std::function<void()>> m_queue;
};

#endif