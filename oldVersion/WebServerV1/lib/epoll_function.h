#ifndef __EPOLL_FUNCTION_H__
#define __EPOLL_FUNCTION_H__

/*
对几个epoll操作的函数进行一下封装
*/

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>

// 往epollfd中添加socket

extern void epollAdd(int epfd, int sockfd, bool oneshot);

// 删除epollfd中的sockfd
extern void epollDel(int epfd, int sockfd);

// 修改epollfd中的sockfd

extern void epollMod(int epfd, int sockfd, int event);

#endif