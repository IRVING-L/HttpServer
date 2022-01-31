#include <iostream>
#include <thread>

class A
{
private:
    std::thread m_t1;
    std::thread m_t2;

public:
    A()
    {
        m_t1 = std::move(std::thread([this]()
                                     { std::cout << "this is class func\n";
                                        std::cout << this->m_t1.get_id() << std::endl; }));
        m_t1.join();

        m_t2 = std::move(std::thread(&A::func, this, this));
        m_t2.join();
    }
    void func(A *arg) { std::cout << "this class func" << arg->m_t2.get_id() << std::endl; }
};

int main()
{
    A a;
    // std::thread t1(&A::func, &a);
    // t1.join();
    return 0;
}