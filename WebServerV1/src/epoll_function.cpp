#include "../lib/epoll_function.h"

void epollAdd(int epfd, int sockfd, bool oneshot)
{
    // 判断epfd是否为正确的epollfd
    if (epfd < 0)
    {
        printf("epfd invalid\n");
        exit(-1);
    }
    struct epoll_event ev;
    ev.data.fd = sockfd;
    // 将sockfd在epollfd中注册为【读】+ 【写】 + 【连接中断】事件, 并同时设置epoll的模式为ET边缘触发
    // ET边缘触发只会通知一次
    ev.events = EPOLLIN | EPOLLRDHUP;
    if (oneshot)
    {
        // 将socket注册为触发一次的状态
        ev.events |= EPOLLONESHOT;
    }

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) != 0)
    {
        // 判断函数epoll_ctl是否正确执行
        printf("epoll_ctl error\n");
    }

    //设置socket为O_NONBLOCK状态。使用fcntl函数
    int oldFlags = fcntl(sockfd, F_GETFL);
    int newFlags = oldFlags | O_NONBLOCK;
    fcntl(sockfd, F_SETFL, newFlags);
}

void epollDel(int epfd, int sockfd)
{
    // 判断epfd是否为正确的epollfd
    if (epfd < 0)
    {
        printf("epfd invalid\n");
        exit(-1);
    }
    epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
}

void epollMod(int epfd, int sockfd, int event)
{
    // 判断epfd是否为正确的epollfd
    if (epfd < 0)
    {
        printf("epfd invalid\n");
        return;
    }
    // 判断sockfd是否有效
    if(sockfd < 0)
    {
        printf("invalid sockfd\n");
        return;
    }
    struct epoll_event ev;
    ev.data.fd = sockfd;
    // 将sockfd在epollfd中注册为【读】+【连接中断】事件, 并同时设置epoll的模式为ET边缘触发
    // ET边缘触发只会通知一次
    ev.events = event | EPOLLRDHUP ;
    epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
}