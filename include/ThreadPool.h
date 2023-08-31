#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <condition_variable>
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
    ThreadPool(int min, int max);
    // 销毁线程池
    ~ThreadPool();

    // 给线程池添加任务
    void threadPoolAdd(Task task);
    void threadPoolAdd(callback f,void* arg);

    // 获取线程池中工作的线程的个数
    int getWorkNum();

    // 获取线程池中活着的线程的个数
    int getAliveNum();
    void threadExit();

private:
    // 任务队列
    std::queue<Task> taskQ;

    std::thread managerID;    // 管理者线程ID
    std::vector<std::thread> threadIDs;   // 工作的线程ID

    int minNum;             // 最小线程数量
    int maxNum;             // 最大线程数量
    int busyNum;            // 忙的线程的个数
    int liveNum;            // 存活的线程的个数
    int exitNum;            // 要销毁的线程个数

    std::mutex mutexPool; // 锁整个的线程池
    std::condition_variable notempty;   // 任务队列是不是空了(因为是队列，所以不可能满)
    
    int shutdown;           // 是不是要销毁线程池, 销毁为1, 不销毁为0
    // 工作的线程(消费者线程)任务函数
    static void worker(void* arg);
    // 管理者线程任务函数
    static void manager(void* arg);
    static const int NUMBER = 2;
};