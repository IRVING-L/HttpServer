#include <iostream>
#include <functional>
using namespace std;

template<class F>
void testFunc(F &&arg)
{
    auto task = forward<F>(arg);
    task();
}
void add(int a, int b, int c, int d)
{
    cout << a + b + c + d << endl;
}
queue
int main()
{
    testFunc(bind(add, 1, 1, 2, 3));
    return 0;
}