#include <iostream>
#include <functional>

class A
{
public:
    void change()
    {
        num = 10;
        std::cout << "A::change\n";
    }

private:
    int num;
};

class B
{
public:
    void func(const std::function<void()> &cb)
    {
        cb();
    }

private:
    std::function<void()> cb;
};

int main(int argc, char const *argv[])
{
    A a;
    B b;
    auto f = std::bind(&A::change, &a);
    auto f1 = std::bind(&A::change, &a);
    b.func(f);
    return 0;
}
