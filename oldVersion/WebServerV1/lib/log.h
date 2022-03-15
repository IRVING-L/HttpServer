
#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
/*
想自己写一个日志文件输出的类
这个类需要实现的功能：
1. 打开一个文件，对文件进行读写（更多的是写）。
    - 这个写应该是要加互斥锁的
2. 对外的接口是一个写入函数，写函数应该可以传入不同的字符串
3. 写的内容格式：
    时间 程序 正在执行的动作
        - 时间：固定的：年月日时分秒微秒
        - 程序：要有路径名
        - 动作：自己传入？
*/
#define MAX_FILE_LEN 100
#define MAX_WRITE_SIZE 1024
class LOG
{
public:
    // 构造函数
    LOG() = default;
    LOG(const char *filename);
    ~LOG();
    void init();
    // 对外接口函数
    void print2File(const char* format, ...);

private:
    char *m_writeBuf;
    static int m_logFd;
    static pthread_mutex_t m_mutex;
};

#endif