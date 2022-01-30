// #include <iostream>
// #include <unistd.h>
// #include "../lib/threadpoolV3.h"

// class Job
// {
// public:
//     Job(){}
//     Job(int *a)
//     {
//         arg = new int(*a);
//     }
//     // 发给线程的回调函数
//     void process()
//     {
//         printf("%d\n", *arg);
//     }
//     ~Job()
//     {
//         delete arg;
//     }
// private:
//     int *arg;
// };

// int main()
// {
//     // 创建一个线程池对象，线程数量4，任务队列长度为20
//     ThreadPool<Job> *pool = new ThreadPool<Job>(4,20);
//     printf("线程池创建成功\n");

//     int i;
//     /*
//     V3 打印只会打印20 40 60 80 100
//     为什么？
//     打印函数中，保存的arg是一个指针，指针记录着传入参数的地址，
//     我们传入的参数是变量i
//     i的地址是不会变
//     所以每20个工作队列中的任务，其参数arg全都指向一个地址，
//     */
//     Job *newjob;
//     for(i = 0;i<100;++i)
//     {
//         newjob = new Job(&i);
//         pool->append(newjob);
        
//     }
    
//     printf("end\n");
//     pthread_exit(NULL); // 主线程结束
//     return 0;
// }