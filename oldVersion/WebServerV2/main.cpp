#include "./lib/threadpool.h"
#include<iostream>
#include<vector>
#include<future>
#include<functional>
#include<random>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(-1000, 1000);
auto rnd = std::bind(dist, mt);

void simulate_hard_computation() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
}

void multiply(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
}

void multiply_output(int & out, const int a, const int b) {
  simulate_hard_computation();
  out = a * b;
  std::cout << a << " * " << b << " = " << out << std::endl;
}

int multiply_return(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
  return res;
}
int main()
{
    Threadpool pool(8);
    for(int i=0;i<8;++i)
    {
        pool.append(multiply,i,i+1);
    }
    system("pause");
    return 0;
}