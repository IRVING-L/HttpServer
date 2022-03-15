#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *worker(void *arg)
{
    int fd = *(int *)arg;
    printf("fd:%d\n",fd);
    close(fd);
    printf("fd:%d\n",fd);
}

int main()
{
    // 测试一下，能否在子线程关闭主线程打开的文件
    int fd = open("text.txt", O_RDONLY);
    printf("fd:%d\n",fd);
    pthread_t tid;
    pthread_create(&tid, NULL, worker, (void *)&fd);
    pthread_join(tid, NULL);
    int fd1 = open("text1.txt", O_RDONLY | O_CREAT);
    printf("fd1:%d\n",fd1);
    close(fd);
    int fd2 = open("text2.txt", O_RDONLY | O_CREAT);
    printf("fd2:%d\n",fd2);
    return 0;
}