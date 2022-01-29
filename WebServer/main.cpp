#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include "lib/threadpoolV3.h"
#include "lib/http_connect.h"
#include "lib/epoll_function.h"

#define THREAD_NUMS 4
#define JOB_NUMS 20
#define EPOLL_MAX 1024
#define MAX_CLIENT_NUM 65535

// 信号注册, 对sigaction的包装
/*
int sigaction(int signum,
const struct sigaction *act, struct sigaction *oldact);
struct sigaction {
    void     (*sa_handler)(int); // 执行函数
    void     (*sa_sigaction)(int, siginfo_t *, void *); // 备用执行函数
    sigset_t   sa_mask; // 信号集
    int        sa_flags; // 标志位
    void     (*sa_restorer)(void); // 不是给应用程序用的
};
*/
// 暂时没有明白这个函数的作用
void addsig(int sig, void(handler)(int))
{
    struct sigaction act;
    act.sa_handler = handler;
    sigfillset(&act.sa_mask);
    //
    assert(sigaction(sig, &act, NULL) != -1);
    // assert在此处发挥了什么样的作用？sigaction被正确执行了会打印错误报告并终止程序
}

int main(int argc, char const *argv[])
{

    // 启动服务端的socket
    // 使用固定的格式运行【可执行文件】，这里做一个判断检查
    if (argc < 1)
    {
        printf("请按如下方式运行程序: %s portname\n", basename(argv[0]));
        exit(-1);
    }

    // 获取端口号
    int port = atoi(argv[1]); // atoi: convert string to int

    // 忽略对端关闭连接产生的SIGPIPE信号
    // addsig(SIGPIPE, SIG_IGN);
    // 创建线程池
    ThreadPool<Http_Conn> *pool = new ThreadPool<Http_Conn>(THREAD_NUMS, JOB_NUMS);

    // 既是读取单元又是保存客户端信息的
    Http_Conn *usrsJobUnit = new Http_Conn[MAX_CLIENT_NUM];
    // socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    // 在bind之前设置【端口复用】
    int reuse = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // bind
    struct sockaddr_in saddr;
    //初始化结构体addr
    saddr.sin_family = AF_INET;
    // inet_pton(AF_INET, "10.0.12.8", (void *)&saddr.sin_addr.s_addr);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);
    bind(lfd, (struct sockaddr *)&saddr, 16);

    // listen
    listen(lfd, 5);

    // 创建epoll 对象 和 数组
    int epollfd = epoll_create(5); // 创建epoll
    // printf("epoll对应的文件描述符为：%d\n", epollfd);
    struct epoll_event epevs[EPOLL_MAX];

    // 将监听的文件描述符添加到epoll内存中去
    // 自定义一个函数
    // 视频中，在mian.cpp中通过extern声明，然后在其他的cpp文件中定义
    // 我这边呢，新开一个头文件好吧
    // epollAdd(epollfd, lfd, 0);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, lfd, &ev);
    // 使用epoll_wait监听文件描述符
    char clientIP[20]={0};
    int clientPort = -1;
    printf("httpServer is running...\n");
    while (1)
    {
        int num = epoll_wait(epollfd, epevs, EPOLL_MAX, -1);
        // 异常出错处理
        if (num < 0 && errno != EINTR)
        {
            printf("epoll failure\n");
            break;
        }
        for (int i = 0; i < num; ++i)
        {
            int currfd = epevs[i].data.fd;
            if (currfd == lfd)
            {
                // 客户端连接
                struct sockaddr_in caddr;
                socklen_t caddrlen;
                int cfd = accept(lfd, (struct sockaddr *)&caddr, &caddrlen);

                // 异常错误判断处理
                if (cfd < 0)
                {
                    // 无效的文件描述符
                    printf("invalid fd, errno is %d\n", errno);
                    continue;
                }
                // 设置端口复用
                // reuse = 1;
                // setsockopt(currfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

                // 打印信息

                fflush(stdout);
                inet_ntop(AF_INET, &caddr.sin_addr, clientIP, sizeof(clientIP)); // 网络IP地址的大端转小端
                clientPort = ntohs(caddr.sin_port);
                printf("新的一个客户端进来了, cfd: %d, ip:%s, port:%d\n", cfd, clientIP, clientPort);

                // 将客户端连接添加到epollfd中去
                epollAdd(epollfd, cfd, 1);

                // 无法再创建更多的连接了
                if (Http_Conn::clientCount >= MAX_CLIENT_NUM)
                {
                    printf("no more resource for new client\n");
                    continue;
                }
                // 将客户端的信息保存起来
                usrsJobUnit[cfd].getClientInfo(epollfd, cfd, clientIP, clientPort);
                // 计数加1
                Http_Conn::clientCount++;
            }
            else if (epevs[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                // 各种错误的宏
                printf("客户端断开了连接\n");

                // 删除该用户的数据
                usrsJobUnit[currfd].closeConn();
            }
            // 【读事件】
            else if (epevs[i].events & EPOLLIN)
            {
                // 读事件到来
                if (usrsJobUnit[currfd].m_read())
                {
                    // 数据读取完成，交给线程池处理
                    pool->append(&usrsJobUnit[currfd]);
                }
                else
                {
                    // 删除该用户的数据
                    usrsJobUnit[currfd].closeConn();
                }
            }
            // 【写事件】
            else if (epevs[i].events & EPOLLOUT)
            {
                printf("准备写入\n");
                if (!usrsJobUnit[currfd].m_write())
                {
                    // 关闭客户端连接
                    usrsJobUnit[currfd].closeConn();
                }
            }
        }
    }

    // 释放请求的资源
    close(lfd);
    close(epollfd);
    delete pool;
    delete[] usrsJobUnit;
    return 0;
}
