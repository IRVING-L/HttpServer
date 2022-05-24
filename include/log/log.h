#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <semaphore.h>
#include <stdarg.h>
#include "locker.h"
class Log
{
public:
    static Log *getInstance();
    ~Log();
    // 对外接口
    void writeMsg(int level, const char *filename, const char *func ,int line, 
                    const char *format, ...);
    // 将内存中的log消息写入到磁盘文件中去
    void writeToFile();

private:
    Log();
    // 打开文件，文件指针
    FILE *_openFile;
    // 获取时间：20220419 21:06:13.153335Z
    void getDate(char *date);
    // 获取线程id
    pid_t _gettid() { return syscall(SYS_gettid); }
    // 缓冲区：位于进程的堆区的缓冲区
    std::queue<std::string> _bufferA;
    // 缓冲区的大小限制
    // 一条log消息，给他算最大128个字节把，buffer中最多装100000条吧
    // 所以一个buffer装满会占据内存大概1MB的空间，能接受的吧
    const int MAX_QUEUE_SIZE = 100000;
    // 使用互斥锁+条件变量
    std::mutex _queueMtx;
    std::condition_variable _queueCond;
    std::condition_variable _writeCond;


    // 日志线程的工作函数
    // 设计了两个日志线程，一个线程定时3s清理内存log消息，
    // 另一个会在内存写满后清理消息
    void logThreadFunc1(int);
    void logThreadFunc2(int);
    // 日志线程
    std::thread _logThread1;
    std::thread _logThread2;
};

// 定义方便使用的宏
#define LOG_BASE(level, filename, func, line, format, ...)                 \
    do                                               \
    {                                                \
        Log *log = Log::getInstance();               \
        log->writeMsg(level, filename, func, line, format, ##__VA_ARGS__); \
    } while (0);
#define LOG_DEBUG(format, ...) LOG_BASE(2, __FILE__, __func__ , __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_BASE(3, __FILE__, __func__ , __LINE__, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) LOG_BASE(4, __FILE__, __func__ , __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_BASE(5, __FILE__, __func__ , __LINE__, format, ##__VA_ARGS__)
#endif