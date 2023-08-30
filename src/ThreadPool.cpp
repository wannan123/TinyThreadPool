#include "ThreadPool.hh"
void ThreadPool::AddTask(Task task)
{
    taskQ.push(task);
}

