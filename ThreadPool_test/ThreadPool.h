#pragma once
#include"TaskQueue.h"
#include<iostream>
#include<condition_variable>
#include<pthread.h>
#include <string>
#include<windows.h>

//����ģʽ
class ThreadPool
{
    // �������
    TaskQueue* taskQ;
    /*
    * ��task_Q����
    int queueCapacity;  // ����
    int queueSize;      // ��ǰ�������
    int queueFront;     // ��ͷ -> ȡ����
    int queueRear;      // ��β -> ������
    */
    //����C��C++��ϵ��̳߳�
    pthread_t managerID;    // �������߳�ID
    pthread_t* threadIDs;   // �������߳�ID
    int minNum;             // ��С�߳�����
    int maxNum;             // ����߳�����
    int busyNum;            // æ���̵߳ĸ���
    int liveNum;            // �����̵߳ĸ���
    int exitNum;            // Ҫ���ٵ��̸߳���
    std::mutex mutexPool;  // ���������̳߳�   ����̫�������������Ĺ���
    static const int NUMBER = 2;
    std::condition_variable notEmpty;    // ��������ǲ��ǿ���   �����ڶ��������
    //����pthread��mutex��conditon_variable
    pthread_cond_t notEmpty_p;
    pthread_mutex_t mutexPool_p;



    bool shutdown;           // �ǲ���Ҫ�����̳߳�, ����Ϊ1, ������Ϊ0
public:
    // �����̳߳ز���ʼ��
    ThreadPool(int min, int max);

    // �����̳߳�
    ~ThreadPool();

    // ���̳߳��������
    void AddTask(Task task);

    // ��ȡ�̳߳��й������̵߳ĸ���
    int GetBusyThreadNumber();

    // ��ȡ�̳߳��л��ŵ��̵߳ĸ���
    int GetLiveThreadNumber();

    //////////////////////
private:
    // �������߳�(�������߳�)������
    static void* worker(void* arg);
    // �������߳�������
    static void* manager(void* arg);
    // �����߳��˳�
    void threadExit();





};

