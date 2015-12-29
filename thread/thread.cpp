
#include "thread.h"

Thread::Thread()
{
	m_tid = 0;
	pthread_attr_init(&m_attr);
	pthread_mutex_init(&m_mutex, NULL); //初始化互斥量
	pthread_cond_init(&m_cond, NULL); //初始化条件变量
	task.fun = 0;
	task.data = NULL;
}

Thread::~Thread()
{
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
	pthread_attr_destroy(&m_attr);
}

ThreadPool::ThreadPool(int ThreadNum):m_threadnum(ThreadNum)
{
	pthread_mutex_init(m_ThreadMutex);
}

ThreadPool::~ThreadPool()
{
	pthread_mutex_destroy(&m_ThreadMutex);
}

void ThreadPool::Init()
{
	int FailFlag = 0;
	for (int i = 0; i < m_PreThreadNum; ++i)
	{
		if(NULL == CreateThread())
		{
			FailFlag = -1;
		}
	}

	//删除创建失败的线程
	if(FailFlag < 0)
	{

	}
}

Thread* ThreadPool::CreateThread()
{
	Thread* pThread = new Thread;
	if(Thread != NULL)
	{
		int ret = pthread_create(pThread->m_tid, pThread->m_attr, ThreadPool::ThreadFunc, thread);
		if(0 != ret)
		{
			delete pThread;
			pThread = NULL;
		}
	}
	return pThread;
}

void* ThreadPool::ThreadFunc(void* arg)
{
	Thread* pThread = (Thread*)arg;
	if(NULL == pThread)
	{
		return NULL;
	}
	//增加线程总数
	TODO:

	//检测是否有任务, 如果没有则进入睡眠等待唤醒.
	//睡眠超过一定时间没被唤醒线程将退出
	struct timespec timewait;
	memset(&timewait, 0x00, sizeof(timewait));
	while(true)
	{
		if(NULL != pThread->m_task.func)
		{
			pThread->m_task.func(pThread->m_task.m_data);
		}

		pthread_mutex_lock(pThread->m_mutex);
		if(ETIMEOUT == pthread_cond_timedwait(&pThread->m_mutex, &pThread->m_cond, &timewait))
		{
			pthread_mutex_lock(pThread->m_mutex);
			break;
		}
		pthread_mutex_unlock(pThread->m_mutex);
	}

	if(NULL != pThread)
	{
		delete pThread;
		pThread = NULL;
	}
	return 0;
}
