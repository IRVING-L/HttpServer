#ifndef __LOCKER_H__
#define __LOCKER_H__
#include <pthread.h>
#include <exception>
#include <semaphore.h>
// 包装线程同步的三种机制：互斥锁、条件变量、信号量

// 互斥锁的封装
class Locker
{
private:
    // 互斥锁量
    pthread_mutex_t m_mutex;

public:
    // 构造函数
    Locker()
    {
        // 对互斥锁量进行初始化
        int ret;
        ret = pthread_mutex_init(&m_mutex, NULL);
        // 成功返回0，错误返回非0
        if (ret != 0)
        {
            // 抛出异常
            throw std::exception();
        }
    }
    // 成员函数
    // 上锁
    bool lock()
    {
        /*
        上锁函数使用trylock是否会好一点？
        trylock 对一个已经上锁的mutex上锁，会返回一个EBUSY == 16错误号
        */
        int ret;
        ret = pthread_mutex_trylock(&m_mutex);
        return ret == 0;
    }
    // 解锁
    bool unlock()
    {
        int ret;
        // 其他线程能够解锁当前线程加的锁吗？这是一个问题
        ret = pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    // 获取互斥锁指针
    pthread_mutex_t *get_mutex_ptr()
    {
        // 此函数是 locker类 和 cond类 之间通信的桥梁
        return &m_mutex;
    }
    // 析构函数。析构函数不能够有参数
    ~Locker()
    {
        // 销毁互斥锁量
        int ret;
        ret = pthread_mutex_destroy(&m_mutex);
        // 成功返回0，错误返回非0
        if (ret != 0)
        {
            // 抛出异常
            throw std::exception();
        }
    }
};

// 封装条件变量

class Cond
{
private:
    // 条件变量
    pthread_cond_t m_cond;

public:
    // 构造函数初始化。只初始化条件变量
    Cond()
    {
        int ret;
        ret = pthread_cond_init(&m_cond, NULL);
        if (ret != 0)
        {
            throw std::exception();
        }
    }

    // 成员函数
    // 条件阻塞当前线程
    bool wait(pthread_mutex_t *mutex_ptr)
    {
        // 封装pthread_cond_wait函数
        int ret;
        ret = pthread_cond_wait(&m_cond, mutex_ptr);
        return ret == 0;
    }
    bool timedwait(pthread_mutex_t *mutex_ptr, const struct timespec time)
    {
        int ret;
        ret = pthread_cond_timedwait(&m_cond, mutex_ptr, &time);
        return ret == 0;
    }
    // 唤醒线程
    bool signal()
    {
        int ret;
        ret = pthread_cond_signal(&m_cond);
        return ret == 0;
    }
    bool broadcast()
    {
        int ret;
        ret = pthread_cond_broadcast(&m_cond);
        return ret == 0;
    }

    // 析构函数
    ~Cond()
    {
        int ret;
        ret = pthread_cond_destroy(&m_cond);
        if (ret != 0)
        {
            throw std::exception();
        }
    }
};

// 封装信号量

class Sem
{
private:
    // 信号量
    sem_t m_sem;

public:
    // 构造函数
    Sem()
    {
        int ret;
        // 默认初始化成1，也就是互斥量
        ret = sem_init(&m_sem, 0, 0);
        if (ret != 0)
        {
            throw std::exception();
        }
    }
    Sem(const int sem_value)
    {
        if(sem_value < 0)
        {
            throw std::exception();
        }
        int ret;
        // 默认初始化成1，也就是互斥量
        ret = sem_init(&m_sem, 0, sem_value);
        if (ret != 0)
        {
            throw std::exception();
        }
    }
    // 成员函数
    // wait
    bool wait()
    {
        int ret;
        ret = sem_wait(&m_sem);
        if (ret != 0)
        {
            return false;
            throw std::exception();
        }
        return ret == 0;
    }
    // post
    bool post()
    {
        int ret;
        ret = sem_post(&m_sem);
        if (ret != 0)
        {
            return false;
            throw std::exception();
        }
        return ret == 0;
    }
    // 析构函数
    ~Sem()
    {
        int ret;
        ret = sem_destroy(&m_sem);
        if (ret != 0)
        {
            throw std::exception();
        }
    }
};

#endif