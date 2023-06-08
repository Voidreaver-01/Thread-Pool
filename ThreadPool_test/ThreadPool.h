#pragma once
#include"TaskQueue.h"
#include<iostream>
#include<condition_variable>
#include<pthread.h>
#include <string>
#include<windows.h>

//单例模式
class ThreadPool
{
    // 任务队列
    TaskQueue* taskQ;
    /*
    * 由task_Q管理
    int queueCapacity;  // 容量
    int queueSize;      // 当前任务个数
    int queueFront;     // 队头 -> 取数据
    int queueRear;      // 队尾 -> 放数据
    */
    //基于C和C++混合的线程池
    pthread_t managerID;    // 管理者线程ID
    pthread_t* threadIDs;   // 工作的线程ID
    int minNum;             // 最小线程数量
    int maxNum;             // 最大线程数量
    int busyNum;            // 忙的线程的个数
    int liveNum;            // 存活的线程的个数
    int exitNum;            // 要销毁的线程个数
    std::mutex mutexPool;  // 锁整个的线程池   无需太多锁，避免锁的管理
    static const int NUMBER = 2;
    std::condition_variable notEmpty;    // 任务队列是不是空了   不存在队满的情况
    //基于pthread的mutex和conditon_variable
    pthread_cond_t notEmpty_p;
    pthread_mutex_t mutexPool_p;



    bool shutdown;           // 是不是要销毁线程池, 销毁为1, 不销毁为0
public:
    // 创建线程池并初始化
    ThreadPool(int min, int max);

    // 销毁线程池
    ~ThreadPool();

    // 给线程池添加任务
    void AddTask(Task task);

    // 获取线程池中工作的线程的个数
    int GetBusyThreadNumber();

    // 获取线程池中活着的线程的个数
    int GetLiveThreadNumber();

    //////////////////////
private:
    // 工作的线程(消费者线程)任务函数
    static void* worker(void* arg);
    // 管理者线程任务函数
    static void* manager(void* arg);
    // 单个线程退出
    void threadExit();





};

