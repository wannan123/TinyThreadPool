#pragma once
#include <string>
#include <queue>
// 任务结构体
using callback = void (*)(void* arg);
struct Task
{
    Task(){
        func = nullptr;
        arg = nullptr;
    }
    Task(callback func_,void* arg_){
        func = func_;
        arg = arg_;
    }
    ~Task(){};
    callback func;
    void* arg;

};
// 线程池结构体
class ThreadPool
{
public:
    void AddTask(Task task);
    Task GetTask();
    void DeleteTask(Task task);
private:
    // 任务队列
    std::queue<Task> taskQ;

    pthread_t managerID;    // 管理者线程ID
    pthread_t *threadIDs;   // 工作的线程ID
    int minNum;             // 最小线程数量
    int maxNum;             // 最大线程数量
    int busyNum;            // 忙的线程的个数
    int liveNum;            // 存活的线程的个数
    int exitNum;            // 要销毁的线程个数
    pthread_mutex_t mutexPool;  // 锁整个的线程池
    pthread_mutex_t mutexBusy;  // 锁busyNum变量
    pthread_cond_t notFull;     // 任务队列是不是满了
    pthread_cond_t notEmpty;    // 任务队列是不是空了

    int shutdown;           // 是不是要销毁线程池, 销毁为1, 不销毁为0
};