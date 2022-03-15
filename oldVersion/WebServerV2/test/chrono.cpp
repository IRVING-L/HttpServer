#include <iostream>
#include <chrono>
#include <unistd.h>
using namespace std;
using namespace chrono;
// chrono 时间库函数测试demo
int main()
{
    std::cout << "*********this is chrono library test*********" << std::endl;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    high_resolution_clock::time_point t2 = t1 + microseconds(5);

    for (int i = 0; i < 10000; ++i)
        ;

    high_resolution_clock::time_point t3 = high_resolution_clock::now();

    duration<double> tt1 = duration_cast<duration<double>>(t2 - t1);
    duration<double> tt2 = duration_cast<duration<double>>(t3 - t1);
    duration<double> tt3 = duration_cast<duration<double>>(t3 - t2);

    cout << "t2-t1 " << tt1.count() << " s\n";
    cout << "t3-t1 " << tt2.count() << " s\n";
    cout << "t3-t2 " << tt3.count() << " s\n";

    return 0;
}