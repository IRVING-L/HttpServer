#include "../lib/log.h"

LOG::LOG(const char*filename)
{
    // 以只读的方式打开一个文件
    // 权限：644 --> 110 100 100 --> -rw-r--r--
    m_logFd = open(filename, O_WRONLY | O_APPEND | O_CREAT, 644);
    // m_writeBuf = new char[MAX_WRITE_SIZE];
    // pthread_mutex_init(&m_mutex, NULL);
}
LOG::~LOG()
{
    close(m_logFd);
    delete [] m_writeBuf;
    pthread_mutex_destroy(&m_mutex);
}
void LOG::init()
{
    // m_logFd = open(filename, O_WRONLY | O_APPEND | O_CREAT, 644);
    m_writeBuf = new char[MAX_WRITE_SIZE];
    // pthread_mutex_init(&m_mutex, NULL);
}
// 静态成员的初始化
int LOG::m_logFd = open("/root/serverProjectNC/WebServer/docs/loginfo.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
pthread_mutex_t LOG::m_mutex = PTHREAD_MUTEX_INITIALIZER;
void LOG::print2File(const char *format, ...)
{
    // 上锁
    pthread_mutex_lock(&m_mutex);
    va_list ap;
    va_start(ap, format);
    // 网writebuf中写入数据
    int writeLen = vsnprintf(m_writeBuf, sizeof(m_writeBuf), format, ap);
    // 往文件中写入数据
    write(m_logFd, m_writeBuf, writeLen + 1);
    va_end(ap);
    // 解锁
    pthread_mutex_unlock(&m_mutex);
}