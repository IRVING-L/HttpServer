#include <iostream>


typedef void (*my_handler_t)(int);



class A
{
public:
    A() {
        std::cout << "A constuctor" << std::endl;
    }
    A(int a) : m_a(a) {}

    ~A()
    {
        std::cout << " A destroyer" << std::endl;
    }

private:
    int m_a;
};

class B
{
public:
    B()
    {
        std::cout << "B constuctor" << std::endl;
    }
    B(int b) : m_b(b) {}
    ~B()
    {
        std::cout << " B destroyer" << std::endl;
    }

private:
    int m_b;
    A m_a;
};

int main(int argc, char const *argv[])
{
    B b;
    return 0;
}
