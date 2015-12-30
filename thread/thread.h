
#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <list>
#include "task.h"

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
	void InitThreadPool();
private:
	Thread* CreateThread();
	static void* ThreadFunc(void* arg);
	void IncreaseThreadNum();
	void DecreaseThreadNum();
	void MoveToIdleThread(Thread* pThread);
	void GetThreadRun(Task_Func func, void* arg);
private:
	int m_PreThreadNum; //创建线程数
	int m_MaxThreadNum; //最大线程数量
	int m_CurThreadNum; //当前线程数量

	pthread_mutex_t m_ThreadMutex; //线程列表锁
	pthread_cond_t  m_ThreadCond;  //线程列表条件变量
	std::list<Thread*> m_ThreadList;
};

#endif
