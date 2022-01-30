// 封装一个【线程池】类
/*
【线程池】类设计要点：
    1 - 核心线程数量
        - 根据工作任务来定。设计时取28原则。根据80%的工况设计好核心线程数
        剩余的20%利用最大线程数来解决。
    2 - 任务队列长度
        - 公式：任务队列长度 = 核心线程数 / 单个任务执行时间 * 2；
    3 - 最大线程数量
        - 公式：最大线程数 = （系统产生的最大任务数 - 工作队列长度）* 单个任务
        的执行时间；
    4 - 最大空闲时间
        - 参考体统运行环境和硬件压力来设定。没有固定值。

【线程池】类实现步骤
    1 - 编写任务类
        - 实现Runnable接口;
    2 - 编写线程类
        - 用于执行任务,需要持有所有任务;
    3 - 编写线程池类
        - 包含提交任务,执行任务的能力;
    4 - 编写测试类
        - 创建线程池对象,提交多个任务测试。
*/
#ifndef __THREADPOOLV2_H__
#define __THREADPOOLV2_H__

// 导入头文件
#include "locker.h"
#include <queue>
#include <vector>
#include <stdio.h>
// 类内声明，类外定义
class Job
{
public:
    Job(){}
    Job(void *a)
    {
        arg = a;
    }
    // 发给线程的回调函数
    void *func()
    {
        printf("%d\n", *(int*)arg);
    }
    // 数据
    void *usr_data;
private:
    void *arg;
};
class ThreadPool
{
public:
    // 构造函数
    ThreadPool(unsigned int t_nums, unsigned int j_nums);

    // 析构函数
    ~ThreadPool();

    // 成员函数：往线程池中【添加】任务事件
    bool append(Job *Newjob);

private:
    // 核心线程的数量
    unsigned int m_thread_num;
    // 最大线程的数量
    unsigned int m_thread_max;
    // 任务队列长度
    unsigned int m_queue_len;
    // 工作队列（存放线程的）
    pthread_t *m_work_threads; // 用动态数组实现
    // 任务队列（存放线程需要执行的任务《函数》的）
    std::queue<Job *> m_job_queue;
    // 用queue，list，数组实现都行
    // 线程的回调函数
    static void *worker(void *arg);
    // 线程同步机制
    Sem sem_job_src;
    Sem sem_mutex;
    // 设置一个标志位，表示整个线程池是否在进行中
    bool isRun;
};

// 构造函数
ThreadPool::ThreadPool(unsigned int t_nums, unsigned int j_nums)
{
    // 初始化线程数量等成员变量
    // 判断输入数据的合法性
    if (t_nums <= 0 || j_nums <= 0)
    {
        printf("参数掺入错误\n");
        throw std::exception();
    }
    m_thread_num = t_nums;
    m_queue_len = j_nums;
    // 初始化信号量，表示job队列资源的信号量不变，互斥锁的信号量初始化为1
    sem_mutex.post();
    // 初始化线程池状态
    isRun = true;
    // 申请堆区内存，存放子线程的线程号
    m_work_threads = new pthread_t[m_thread_num];
    if (!m_work_threads)
    {
        // 如果开辟堆区动态内存失败，抛出异常
        isRun = false;
        printf("堆区开辟内存失败\n");
        throw std::exception();
    }
    // 创建 m_thread_num 个子线程
    for (int i = 0; i < m_thread_num; ++i)
    {
        int ret;
        ret = pthread_create(m_work_threads + i, NULL, worker, this);
        if (ret != 0)
        {
            // 创建线程出现异常，终止整个程序，清除资源
            delete[] m_work_threads;
            isRun = false;
            printf("创建线程失败\n");
            throw std::exception();
        }
    }
    // 线程创建后，设置线程分离
    for (int i = 0; i < m_thread_num; ++i)
    {
        int ret;
        ret = pthread_detach(m_work_threads[i]);
        if (ret != 0)
        {
            // 创建线程出现异常，终止整个程序，清除资源
            delete[] m_work_threads;
            isRun = false;
            printf("线程分离失败\n");
            throw std::exception();
        }
    }
}
// 析构函数
ThreadPool::~ThreadPool()
{
    // 销毁指针，释放堆区内存等
    delete[] m_work_threads;
    isRun = false;
}
// public成员函数：append
bool ThreadPool::append(Job *Newjob)
{
    // 往内存池中，添加一个工作事件，事件应该被添加到任务队列中
    // 在主线程中，向任务队列中写入数据，必须要加锁
    // 上互斥锁
    printf("添加任务\n");
    sem_mutex.wait();
    // 判断job队列是否还有空间
    if (m_job_queue.size() >= m_queue_len)
    {
        // printf("任务队列已满，暂时无法添加新任务，请稍后再试\n");
        sem_mutex.post();
        return false;
    }
    m_job_queue.push(Newjob);
    sem_mutex.post();   // 解锁
    sem_job_src.post(); // job信号量加1
    return true;
}

// private成员函数：m_run
void *ThreadPool::worker(void *arg)
{
    // 内存池中的线程，需要执行的任务，任务从任务队列中取
    // 从任务队列中取出一个任务，从工作队列（线程s）中取出一个线程，让线程去执行这个任务（函数）
    // 任务的形态应该是什么：
    // 《函数》
    // 其实本质上，仍旧是生产者和消费者模型
    printf("执行工作任务\n");
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->isRun)
    {
        // 消耗一个资源，如果任务队列没有资源，阻塞等待(就相当于是让线程睡眠了)
        pool->sem_job_src.wait();
        pool->sem_mutex.wait(); // 互斥锁

        // 取出一个任务
        Job *Newjob = pool->m_job_queue.front();
        pool->m_job_queue.pop();
        // 退出，解锁
        pool->sem_mutex.post();
        // 拿到job后，在锁外执行job内具体的函数
        printf("线程成功获取任务\n");
        Newjob->func();
    }
}
#endif