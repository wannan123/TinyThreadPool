#include "ThreadPool.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
ThreadPool::ThreadPool(int min, int max)
{

    minNum = min;
    maxNum = max;
    busyNum = 0;
    liveNum = min;    // 和最小个数相等
    exitNum = 0;

    shutdown = 0;

    // 创建线程
    managerID = std::thread(manager,this);
    
    threadIDs.resize(max);
    for (int i = 0; i < min; ++i)
    {
        threadIDs[i] = std::thread(worker,this);
    }
    return;

}

ThreadPool::~ThreadPool()
{

    // 关闭线程池
    shutdown = 1;
    // 阻塞回收管理者线程
    if(managerID.joinable()) managerID.join();
    
    // 唤醒阻塞的消费者线程
    notempty.notify_all();
    for (int i = 0; i < liveNum; ++i)
    {
        if(threadIDs[i].joinable()) threadIDs[i].join();
    }

}


void ThreadPool::threadPoolAdd(Task task)
{
    // pthread_mutex_lock(&pool->mutexPool);
    std::unique_lock<std::mutex> lk(mutexPool,std::defer_lock);
    lk.lock();

    if (shutdown)
    {
        lk.unlock();
        return;
    }
    // 添加任务
    taskQ.push(task);
    lk.unlock();
    notempty.notify_all();
    
}
void ThreadPool::threadPoolAdd(callback f,void* arg)
{
    // pthread_mutex_lock(&pool->mutexPool);
    std::unique_lock<std::mutex> lk(mutexPool,std::defer_lock);
    lk.lock();

    if (shutdown)
    {
        lk.unlock();
        return;
    }
    // 添加任务
    taskQ.push(Task(f,arg));
    lk.unlock();
    notempty.notify_all();
    
}
int ThreadPool::getWorkNum()
{
    mutexPool.lock();
    int busyNum = busyNum;
    mutexPool.unlock();
    return busyNum;
}

int ThreadPool::getAliveNum()
{
    mutexPool.lock();
    int aliveNum = liveNum;
    mutexPool.unlock();
    return aliveNum;
}

void ThreadPool::worker(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);

    while (1)
    {

        //pthread_mutex_lock(&pool->mutexPool);
        //上锁
        std::unique_lock<std::mutex> lk(pool->mutexPool,std::defer_lock);
        lk.lock();
        // 当前任务队列是否为空                  
        while (pool->taskQ.empty() && !pool->shutdown)
        {
            // 阻塞工作线程
            pool->notempty.wait(lk);
            //pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

            // 判断是不是要销毁线程
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    std::cout << "threadid: " << pthread_self() << " exit......" << std::endl;
                    lk.unlock();
                    return;//这里直接return可以结束进程，并且在线程池析构函数中将会用join释放资源
                }
            }
        }

        // 判断线程池是否被关闭了
        if (pool->shutdown)
        {
            // pool->threadExit();
            std::cout << "threadid: " << pthread_self() << "exit......" << std::endl;
            return;//这里直接return可以结束进程，并且在线程池析构函数中将会用join释放资源
        }

        // 从任务队列中取出一个任务
        Task task;
        if(!pool->taskQ.empty()){
            task = pool->taskQ.front();
        }
        pool->taskQ.pop();
        pool->busyNum++;
        // 解锁

        lk.unlock();

        std::cout<<"thread"<<std::to_string(pthread_self())<<"start working..."<<std::endl;
        task.func(task.arg);
        free(task.arg);
        task.arg = nullptr;

        std::cout<<"thread"<<std::to_string(pthread_self())<<"end working..."<<std::endl;
        lk.lock();
        pool->busyNum--;
        lk.unlock();
    }
}

void ThreadPool::manager(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (!pool->shutdown)
    {
        // 每隔3s检测一次
        sleep(3);

        std::unique_lock<std::mutex> lk(pool->mutexPool,std::defer_lock);
        // 取出线程池中任务的数量和当前线程的数量
        lk.lock();
        int queueSize = pool->taskQ.size();
        int liveNum = pool->liveNum;
        int busyNum = pool->busyNum; 
        lk.unlock();

        // 添加线程
        // 任务的个数>存活的线程个数 && 存活的线程数<最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            lk.lock();
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER
                && pool->liveNum < pool->maxNum; ++i)
            {
                if (pool->threadIDs[i].get_id() == std::thread::id())
                {
                    std::cout << "Create a new thread..." <<std::endl;
                    pool->threadIDs[i] = std::thread(worker,pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            lk.unlock();
        }
        // 销毁线程
        // 忙的线程*2 < 存活的线程数 && 存活的线程>最小线程数
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            lk.lock();
            pool->exitNum = NUMBER;
            lk.unlock();
            // 让工作的线程自杀
            for (int i = 0; i < NUMBER; ++i)
            {
                pool->notempty.notify_all();
            }
        }
    }
}

// void ThreadPool::threadExit()
// {
//     std::thread tid1 = std::thread(pthread_self());
    // for (int i = 0; i < maxNum; ++i)
    // {
    //     if (threadIDs[i].get_id() == std::thread::id())
    //     {
    //         std::cout << "threadExit() function: thread " 
    //             << std::to_string(pthread_self()) << " exiting..." << std::endl;
    //         break;
    //     }
    // }
//     tid1.join();
// }



