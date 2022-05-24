#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <iostream>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string>
#include <chrono>
#include "../include/log/log.h"

using namespace std;
using namespace chrono;
void threadFunc1()
{
    for (int i = 0; i < 100; ++i)
    {
        const char *body = "threadFunc1 saying:~";
        LOG_DEBUG("%s", body);
        usleep(500000);
    }
}

int main()
{
    std::thread t1(threadFunc1);
    for (int i = 0; i < 100; ++i)
    {
        const char *body = "main thread is saying:~";
        LOG_DEBUG("%s", body);
        // struct timeval pt1;
        // gettimeofday(&pt1, NULL);
        // struct timeval pt2;
        // gettimeofday(&pt2, NULL);
        // cout << "time cost: " << pt2.tv_sec-pt1.tv_sec << " s "
        //      << pt2.tv_usec-pt1.tv_usec << " us " << endl;
        usleep(500000);
    }
    t1.join();

    // FILE *fp = NULL;

    // fp = fopen("./sssssssss.txt", "w+");
    // fprintf(fp, "This is testing for fprintf...\n");
    // fputs("This is testing for fputs...\n", fp);
    // fclose(fp);
    // struct timeval pt;
    // gettimeofday(&pt, NULL);
    // struct tm *lt = gmtime(&pt.tv_sec);
    // char date[30];
    // snprintf(date,sizeof(date)-1, "%d%02d%02d %02d:%02d:%02d.%ldZ",
    //             lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday, lt->tm_hour+8, lt->tm_min, lt->tm_sec, pt.tv_usec);
    // printf("%s\n",date);
    sleep(100);
    return 0;
}
