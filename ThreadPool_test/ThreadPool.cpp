#include "ThreadPool.h"

ThreadPool::ThreadPool(int min, int max) {
	taskQ = new TaskQueue;
	while (1)
	{
		//申请并初始化一块空间管理工作线程的id
		threadIDs = new pthread_t[max];
		if (threadIDs==nullptr)
		{
			std::cout << "new ThreadIDs manager fail" << std::endl;
			break;
		}
		for (int i = 0; i < max; ++i)
		{
			threadIDs[i] = {nullptr,0};
		}
		//初始化其余参数
		this->busyNum = 0;
		this->exitNum = 0;
		this->liveNum = min;
		this->maxNum = max;
		this->minNum = min;
		//初始化锁和条件变量
		if (pthread_mutex_init(&this->mutexPool_p,NULL))
		{
			std::cout << "mutex init fail" << std::endl;
			break;
		}
		else {
			mutexPool.unlock();
		}
		if (pthread_cond_init(&notEmpty_p, NULL) != 0)
		{
			std::cout << "condition_variable init fail" << std::endl;
			break;
		}
		this->shutdown = false;

		pthread_create(&managerID, NULL, manager, this);
		for (int i = 0; i < min; ++i)
		{
			pthread_create(&threadIDs[i], NULL, worker, this);
		}



		return;
	}

	//释放资源
	if (threadIDs) delete[]threadIDs;
	if (taskQ)
	{
		delete taskQ;
		//taskQ = nullptr;
	}
}

ThreadPool::~ThreadPool()
{
	//关闭线程池
	this->shutdown = true;
	//阻塞并回收管理者线程
	pthread_join(managerID, NULL);
	//唤醒阻塞的活线程，注意这些活线程不在执行任何任务
	for (int i = 0; i < this->liveNum; ++i)
	{
		pthread_cond_signal(&this->notEmpty_p);
	}
	//销毁任务队列
	if (this->taskQ)
	{
		delete this->taskQ;
		this->taskQ = nullptr;
	}
	//销毁工作线程id数组
	if (this->threadIDs)
	{
		delete[]this->threadIDs;
		//this->threadIDs = nullptr;
	}
	//销毁互斥锁喝条件变量
	pthread_cond_destroy(&this->notEmpty_p);
	pthread_mutex_destroy(&this->mutexPool_p);

}

void ThreadPool::AddTask(Task task)
{
	//锁住线程池
	pthread_mutex_lock(&mutexPool_p);
	if (shutdown)
	{
		pthread_mutex_unlock(&mutexPool_p);
		return;
	}
	//添加任务
	this->taskQ->Add_Task(task);
	//唤醒一个worker线程
	pthread_cond_signal(&this->notEmpty_p);
	pthread_mutex_unlock(&mutexPool_p);
	return;
}

int ThreadPool::GetBusyThreadNumber()
{
	pthread_mutex_lock(&mutexPool_p);
	int Busy_Number = this->busyNum;
	pthread_mutex_unlock(&mutexPool_p);
 }

int ThreadPool::GetLiveThreadNumber()
{
	pthread_mutex_lock(&mutexPool_p);
	int Live_Number = this->busyNum;
	pthread_mutex_unlock(&mutexPool_p);
	return Live_Number;
}

void* ThreadPool::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	//worker线程不断访问工作队列
	while (true) {
		//std::unique_lock<std::mutex>worker_mutex(pool->mutexPool_p);
		//如果工作队列没有任务且线程池未被关闭，那么阻塞工作队列
		while (pool->taskQ->TaksNumber() == 0 && !pool->shutdown) {
			//该行代码类似于条件变量的使用，阻塞，同时释放🔒
			pthread_cond_wait(&pool->notEmpty_p, &pool->mutexPool_p);
			if (pool->exitNum > 0)
			{
				pool->exitNum--;
				if (pool->liveNum > pool->minNum)
				{
					pool->liveNum--;
					pthread_mutex_unlock(&pool->mutexPool_p);
					pool->threadExit();
				}
			}

		}
		//判断线程池是否呗关闭
		if (pool->shutdown)
		{
			pthread_mutex_unlock(&pool->mutexPool_p);
			pool->threadExit();
		}

		//从任务队列中取出一个任务
		Task task_temp = pool->taskQ->Pop_Task();
		pool->busyNum++;


		//解锁
		pthread_mutex_unlock(&pool->mutexPool_p);

		std::cout << "thread id" << std::this_thread::get_id() << "is starting now." << std::endl;
		task_temp.function(task_temp.arg);
		delete task_temp.arg;
		task_temp.arg = nullptr;


		pthread_mutex_lock(&pool->mutexPool_p);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexPool_p);
		std::cout << "thread id" << std::this_thread::get_id() << "is ending now." << std::endl;



	}
	return nullptr;
}

void* ThreadPool::manager(void* arg)
{
	ThreadPool* pool = (ThreadPool*)arg;
	while (!pool->shutdown)
	{
		Sleep(3000);
		//取出作业队列中的任务数和当前线程数量 取出忙的线程数目
		pthread_mutex_lock(&pool->mutexPool_p);
		int queuesize = pool->taskQ->TaksNumber();
		int busynumber = pool->busyNum;
		int livenumber = pool->liveNum;
		pthread_mutex_unlock(&pool->mutexPool_p);

		//添加线程--线程不足且允许添加线程
		if (queuesize > livenumber && livenumber < pool->maxNum) {
			pthread_mutex_lock(&pool->mutexPool_p);
			int count = 0;
			for (int i = 0; i < pool->maxNum && count<NUMBER && pool->maxNum>pool->liveNum; ++i) {
				if (pool->threadIDs[i].x==0)//pool->threadIDs[i]==0
				{
					pthread_create(&pool->threadIDs[i], NULL, worker, pool);
					count++;
					pool->liveNum++;
					std::cout << "i love study\n";
				}
			}


			pthread_mutex_unlock(&pool->mutexPool_p);
		}

		//销毁线程:忙线程数*2《存活线程数目且存活线程数目>最小线程数目
		if (busynumber * 2 < livenumber && livenumber > pool->minNum)
		{
			pthread_mutex_lock(&pool->mutexPool_p);
			pool->exitNum = NUMBER;
			pthread_mutex_unlock(&pool->mutexPool_p);
			for (int i = 0; i < NUMBER; ++i)
			{
				pthread_cond_signal(&pool->notEmpty_p);
			}
		}


	}
	return nullptr;
}

void ThreadPool::threadExit()
{
	pthread_t threadid = pthread_self();
	for (int i = 0; i < this->maxNum; ++i)
	{
		if (pthread_equal(this->threadIDs[i], threadid))
		{
			this->threadIDs[i].x = 0;
			//this->threadIDs[i].p = nullptr;
			std::cout << "this thread's void*="<<this->threadIDs[i].p<<"thread's unsigned int's=" <<this->threadIDs[i].x<< ",and it is existing" << std::endl;
			break;
		}
	}
	pthread_exit(NULL);
}


