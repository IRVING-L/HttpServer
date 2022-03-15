#include <unistd.h>
#include <iostream>
#include "lib/webserver.h"
int main(int argc, char const *argv[])
{
    if (argc < 1)
    {
        std::cout << "请按如下方式运行程序: ./" << basename(argv[0]) <<"portname\n";
        exit(-1);
    }
    // 获取端口号
    int port = atoi(argv[1]); // atoi: convert string to int
    
    Webserver server(port, 5000, 1, 8); // 5000 ms 的定时
    server.start();
    return 0;
}
