#pragma once
#include<queue>
#include<pthread.h>
// ����ṹ��
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
    //�����Զ�ʵ��ͷ����β����ά�������ṩ ������������
    std::queue<Task>mytask_queue;
    //std::mutex Queue_mutex;
    pthread_mutex_t queue_mutex;
public:
    TaskQueue();
    ~TaskQueue();
    //�������
    void Add_Task(Task task);
    //ȡ������
    Task Pop_Task();
    int TaksNumber() {
        return mytask_queue.size();
    }
};

