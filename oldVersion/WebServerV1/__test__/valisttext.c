#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
void valistTest(int a, ...)
{
    // 做一个求和？
    int sum = 0;
    va_list ap;
    va_start(ap, a);
    // 参数的个数是未知的
    // 能否用参数a? 试一下
    // 可以的。
    // 用可变参数列表的最后一个实参去初始化ap，应该只是让其获得参数的栈区地址，进行关联
    int count = a;
    while (count)
    {
        sum += va_arg(ap, int);
        count--;
    }
    // 打印结果
    printf("sum is %d\n", sum);
}

void vsnpTest(char *writeBuf, const int LEN, const char *myFormat, ...)
{
    va_list ap;
    va_start(ap, myFormat);

    int ret = vsnprintf(writeBuf, LEN, myFormat, ap);
    printf("往数组中写入了%d个字符\n", ret);
    if (ret > LEN)
    {
        printf("error\n");
    }
    va_end(ap);

    // 打印测试一下
    printf("writeBuf:%s", writeBuf);
}

void test1(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    char writeBuf[128];

    va_end(ap);
}
int main()
{
    // valistTest(5,1,2,3,4,5);
    // const int LEN = 5;
    // char writeBuf[5];
    // memset(writeBuf, 0, sizeof(writeBuf));

    // vsnpTest(writeBuf, LEN, "%s", "abcdef");

    int fd = open("text.txt", O_WRONLY | O_CREAT, 0644);
    write(fd,"this is open/write test\n", 26);
    close(fd);
    return 0;
}