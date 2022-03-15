#include "../lib/threadpoolV2.h"
#include <iostream>
#include <unistd.h>

void add1(int a, int b)
{
    sleep(1);
    std::cout << a << "+" << b << "=" << a + b << std::endl;
}
void add2(int a, int b)
{
    sleep(1);
    std::cout << a << "+" << b << "=" << a + b << std::endl;
}
void add3(int a, int b)
{
    sleep(1);
    std::cout << a << "+" << b << "=" << a + b << std::endl;
}

int main()
{
    Threadpool pool;
    pool.append(std::bind(add1, 1, 2));
    pool.append(std::bind(add2, 10, 20));
    pool.append(std::bind(add3, 100, 200));
    for(int i = 0 ; i < 1000; ++i)
    {
        pool.append(std::bind(add1, i ,i*10));
    }
    sleep(1000);
    return 0;
}