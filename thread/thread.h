
#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>

class Thread
{
public:
	Thread();
	~Thread();
	pthread_t m_tid;
	pthread_attr_t  m_attr;
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;
	Task m_task;
};

class ThreadPool
{
public:
	ThreadPool(int ThreadNum = 0);
	~ThreadPool();
public:
	void InitThreadPool(int ThreadNum);
private:
	Thread* CreateThread();
	static void* ThreadFunc(void* arg);
	void IncreaseThreadNum();
	void DecreaseThreadNum();
private:
	int m_PreThreadNum; //创建线程数
	int m_MaxThreadNum; //最大线程数量
	int m_CurThreadNum; //当前线程数量

	pthread_mutex_t m_ThreadMutex; //线程列表锁
	std::list<Thread> m_ThreadList;
};

#endif
