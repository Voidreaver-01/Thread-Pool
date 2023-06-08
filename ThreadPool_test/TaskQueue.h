#pragma once
#include<queue>
#include<pthread.h>
// 任务结构体
using fun_ptr = void (*)(void* arg);
struct Task
{
    fun_ptr function;
    //void (*function)(void* arg);
    void* arg;
    Task() :function(nullptr), arg(nullptr) {}
    Task(fun_ptr f, void* arg_temp) :function(f), arg(arg_temp) {}
};

class TaskQueue
{
    //容器自动实现头部和尾部的维护，不提供 任务数量限制
    std::queue<Task>mytask_queue;
    //std::mutex Queue_mutex;
    pthread_mutex_t queue_mutex;
public:
    TaskQueue();
    ~TaskQueue();
    //添加任务
    void Add_Task(Task task);
    //取出任务
    Task Pop_Task();
    int TaksNumber() {
        return mytask_queue.size();
    }
};

