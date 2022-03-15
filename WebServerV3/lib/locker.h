// 把信号量sem的函数封装一下

#ifndef __LOCKER_H__
#define __LOCKER_H__

#include <semaphore.h>
#include <exception>
#include <stdexcept>
#include <iostream>

class Locker
{
public:
    explicit Locker(int _pshared, unsigned int value)
    {
        _init(_pshared, value);
    }
    ~Locker()
    {
        _destroy();
        
    }
    Locker(const Locker &) = delete;
    Locker(const Locker &&) = delete;
    Locker &operator=(const Locker &) = delete;

    bool wait()
    {
        int ret = 0;
        try
        {
            int ret = sem_wait(&m_sem);
            if(ret != 0) {throw std::runtime_error("call sem_wait() faild");}
        }
        catch(const std::runtime_error& e){ std::cerr << e.what() << '\n'; }
        
        return ret == 0;
    }
    bool post()
    {
        int ret = 0;
        try
        {
            int ret = sem_post(&m_sem);
            if(ret != 0) {throw std::runtime_error("call sem_post() faild");}
        }
        catch(const std::runtime_error& e){ std::cerr << e.what() << '\n'; }
        
        return ret == 0;
    }
private:
    sem_t m_sem;
    void _init(int _pshared, unsigned int value)
    {
        try
        {
            if(value < 0) { throw std::invalid_argument("The value cannot be less than 0"); }
            int ret = sem_init(&m_sem, _pshared, value);
            if(ret != 0 ) { throw std::runtime_error("call sem_init() faild"); }
        }
        catch(const std::invalid_argument &e) { std::cerr << e.what() << "\n"; }
        catch(const std::runtime_error &e){ std::cerr << e.what() << '\n'; }
        
    }
    void _destroy()
    {
        try
        {
            int ret = sem_destroy(&m_sem);
            if(ret != 0) { throw std::runtime_error("call sem_destroy() faild");}
        }
        catch(const std::runtime_error& e){ std::cerr << e.what() << '\n'; }
    }
};

#endif