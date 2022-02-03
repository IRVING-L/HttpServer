#include "../lib/bufferV2.h"

/* 服务端的程序

socket()
    |
bind()
    |
listen()
    |
accept()


*/
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>
int main()
{
    //创建套接字
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);

    //绑定
    struct sockaddr_in saddr;
    //初始化结构体addr
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, "10.0.12.8", (void *)&saddr.sin_addr.s_addr);
    saddr.sin_port = htons(12345);
    bind(listenFd, (struct sockaddr *)&saddr, 16);

    //监听listen
    listen(listenFd, 10); //无连接会阻塞

    //有连接。accept
    struct sockaddr_in caddr;
    socklen_t caddrLen;
    int cfd = accept(listenFd, (struct sockaddr *)&caddr, &caddrLen);

    //连接建立成功，执行服务端和客服端的工作
    /* 。。 */
    //打印客服端的IP和端口号
    printf("客户端和服务端成功建立连接\n");
    char cip[16];
    printf("客户端IP：%s\n", inet_ntop(AF_INET, &caddr.sin_addr.s_addr, cip, sizeof(cip)));
    printf("客户端端口号%x\n", ntohs(caddr.sin_port));
    Buffer buf;
    int err = 0;
    // set nonblocking
    int oldflag = fcntl(cfd, F_GETFL);
    int newflag = oldflag | O_NONBLOCK;
    fcntl(cfd, F_SETFL, newflag);
    ssize_t len = 0;
    // epoll
    int epfd = epoll_create(5);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = cfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    struct epoll_event evs[2];
    while (1)
    {
        epoll_wait(epfd, evs, 2, -1);
        if (evs[0].data.fd == cfd)
        {
            //接收数据
            printf("data is coming\n");
            len = buf.recvFd(cfd, &err);
            if (len > 0)
            {
                //printf("recv data:%s\n", buf._all2str());
                std::cout << buf._all2str() << std::endl;
                // buf.print();
                // write
                //buf.writeFd(cfd, &err);
            }
        }
    }
    return 0;
}