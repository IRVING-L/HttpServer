#include "log.h"

Log::Log()
{
    struct timeval pt;
    gettimeofday(&pt, NULL);
    struct tm *lt = gmtime(&pt.tv_sec);
    char filePath[64] = {0};
    snprintf(filePath, sizeof(filePath) - 1, "./.log_file/%d_%02d_%02d_%02d:%02d:%02d.log",
             lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, (lt->tm_hour + 8) % 24, lt->tm_min, lt->tm_sec);
    _openFile = fopen(filePath, "w");
    if (_openFile == nullptr)
    {
        mkdir("./.log_file/", 0776);
        _openFile = fopen(filePath, "w");
    }
    // 启动日志线程
    _logThread1 = std::thread(&Log::logThreadFunc1, this, 1);
    _logThread2 = std::thread(&Log::logThreadFunc2, this, 1);
}
Log::~Log()
{
    fclose(_openFile);
    /*
    日志线程，只是调用这个实例的程序的一个子线程，如果调用者要结束进程了
    需要等待这个日志线程结束后再退出
    */
    if (_logThread1.joinable()) { _logThread1.join(); }
    if (_logThread2.joinable()) { _logThread2.join(); }
}
// 单例模式
Log *Log::getInstance()
{
    static Log _log;
    return &_log;
}
// 对外接口: 多个生产者
void Log::writeMsg(int level, const char *filename, const char *func, int line,
                   const char *format, ...)
{
    std::string msg;
    char date[30] = {0};
    getDate(date);
    msg.append(date);
    msg.append(" " + std::to_string(gettid()) + " ");
    switch (level)
    {
    case 1:
    {
        msg.append("TRACE ");
        break;
    }
    case 2:
    {
        msg.append("DEBUG ");
        break;
    }
    case 3:
    {
        msg.append("INFO ");
        break;
    }
    case 4:
    {
        msg.append("WARN ");
        break;
    }
    case 5:
    {
        msg.append("ERROR ");
        break;
    }
    default:
    {
        msg.append("INFO ");
        break;
    }
    }
    va_list vaList;
    va_start(vaList, format);
    char str[256] = {0};
    vsnprintf(str, sizeof(str) - 1, format, vaList);
    va_end(vaList);
    msg.append(str);
    msg.append(" ");
    msg.append(filename);
    msg.append(":" + std::to_string(line));
    msg.append(":");
    msg.append(func);
    // 并发访问，上锁
    {
        std::unique_lock<std::mutex> lck(_queueMtx);
        while (_bufferA.size() >= MAX_QUEUE_SIZE)
        {
            _writeCond.notify_one();
            _queueCond.wait(lck);
        }
        _bufferA.push(msg);
    }
}

// 获取时间：20220419 21:06:13.153335Z
void Log::getDate(char *date)
{
    struct timeval pt;
    gettimeofday(&pt, NULL);
    struct tm *lt = gmtime(&pt.tv_sec);
    sprintf(date, "%d%02d%02d %02d:%02d:%02d.%-6ldZ",
            lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, (lt->tm_hour + 8) % 24, lt->tm_min, lt->tm_sec, pt.tv_usec);
}

// 将内存中的log消息写入到磁盘文件中去：单个消费者
void Log::writeToFile()
{
    // 上锁
    std::unique_lock<std::mutex> lck(_queueMtx);
    if (_bufferA.empty())
    {
        return;
    }
    while (!_bufferA.empty())
    {
        std::string msg(_bufferA.front());
        _bufferA.pop();
        fprintf(_openFile, "%s\r\n", msg.c_str());
        fflush(_openFile);
    }
    _queueCond.notify_all();
}

// 日志线程的工作函数
void Log::logThreadFunc1(int)
{
    // 睡眠3s，醒来后就将内存中的日志写入到文件中去
    while (true)
    {
        sleep(3);
        writeToFile();
    }
}
void Log::logThreadFunc2(int)
{
    // 缓冲区满了后，就需要将日志写入到文件中
    while (true)
    {
        {
            std::unique_lock<std::mutex> lck(_queueMtx);
            if (_bufferA.size() < MAX_QUEUE_SIZE)
            {
                _writeCond.wait(lck);
            }
        }
        writeToFile();
    }
}