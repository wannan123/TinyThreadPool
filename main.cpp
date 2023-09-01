//main.cpp
#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void taskFunc(void* arg)
{
    int num = *(int*)arg;
    cout << "thread: " << pthread_self() << ", number=" << num << endl;
    sleep(1);
}

int main()
{
    ThreadPool pool(3,10);
    int i;
    for(i = 0; i < 100; ++i)
    {
        int* num = new int(i+100);
        pool.threadPoolAdd(taskFunc, (void*)num);
    }
    sleep(20);
    // for(;i < 200; ++i) 
    // {
    //     sleep(1);
    //     int* num = new int(i+100);
    //     pool.threadPoolAdd(taskFunc, (void*)num);
    // }
    return 0;
}
