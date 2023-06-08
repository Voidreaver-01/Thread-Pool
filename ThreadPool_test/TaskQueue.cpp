#include "TaskQueue.h"

TaskQueue::TaskQueue()
{
    pthread_mutex_init(&this->queue_mutex,NULL);
}

TaskQueue::~TaskQueue()
{
    pthread_mutex_destroy(&this->queue_mutex);
}

void TaskQueue::Add_Task(Task task)
{
    //std::unique_lock<std::mutex>Unique_mutex;
    pthread_mutex_lock(&queue_mutex);
    mytask_queue.push(task);
    pthread_mutex_unlock(&queue_mutex);
}

Task TaskQueue::Pop_Task()
{
    Task t;
    pthread_mutex_lock(&queue_mutex);
    if (!mytask_queue.empty())
    {
        t = mytask_queue.front();
        mytask_queue.pop();
    }
    pthread_mutex_unlock(&queue_mutex);
    //·µ»Ø¿ÕÈÎÎñ
    return t;
}
