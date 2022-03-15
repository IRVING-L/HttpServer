/*
针对C++的可变参数模板函数的测试demo
 */
#include <iostream>

int add(int &num){}

template <typename... Args>
int add(int &num, const Args &...args)
{
    num += t;
    add(num, args...);
}

int main(int argc, char const *argv[])
{
    int sum = 0;
    add(sum, 1, 2, 3, 4, 6);
    std::cout << sum << std::endl;
    return 0;
}
