#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
/*
这个程序，是对stat函数以及writev分散写函数的测试demo
*/
int main()
{

    // stat 判断文件是否存在
    char *filePath = "/root/serverProjectNC/WebServer/docs/statTest.txt";
    struct stat fileInfo;
    int ret = stat(filePath, &fileInfo);
    if(ret == -1 && errno == ENOENT)
    {
        printf("由stat的返回值&错误码判断：文件不存在\n");
    }
    else
    {
        printf("文件存在\n");
        // 获取文件的属性
        printf("十进制表示：st_mode:%u\n八进制表示：st_mode:%o",fileInfo.st_mode,fileInfo.st_mode);
        // st_mode 是一个八进制的数：100644：-rw-r--r--
        printf("是否具有读取权限：%d\n", (fileInfo.st_mode & S_IRUSR) == S_IRUSR);

    }
    return 0;

}
