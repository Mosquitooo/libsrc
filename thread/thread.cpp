
#include <string.h>
#include <errno.h>
#include "thread.h"

Thread::Thread()
{
	m_tid = 0;
	pthread_attr_init(&m_attr);
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
	m_task.func = 0;
	m_task.data = NULL;
}

Thread::~Thread()
{
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
	pthread_attr_destroy(&m_attr);
}

ThreadPool::ThreadPool(int ThreadNum):m_PreThreadNum(ThreadNum)
{
	pthread_mutex_init(&m_ThreadMutex, NULL);
	pthread_cond_init(&m_ThreadCond, NULL);
}

ThreadPool::~ThreadPool()
{
	pthread_mutex_destroy(&m_ThreadMutex);
	pthread_cond_destroy(&m_ThreadCond);
}

void ThreadPool::InitThreadPool( )
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
		std::list<Thread*>::iterator it = m_ThreadList.begin();
		for(; it != m_ThreadList.end(); ++it)
		{
			if(NULL != (*it))
			{
				delete *it;
				*it = NULL;
			}
		}
	}
}

Thread* ThreadPool::CreateThread()
{
	Thread* pThread = new Thread;
	if(pThread != NULL)
	{
		int ret = pthread_create(&pThread->m_tid, &pThread->m_attr, ThreadPool::ThreadFunc, pThread);
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
			pThread->m_task.func(pThread->m_task.data);
		}

		pthread_mutex_lock(&pThread->m_mutex);
		if(ETIMEDOUT == pthread_cond_timedwait(&pThread->m_cond, &pThread->m_mutex, &timewait))
		{
			pthread_mutex_unlock(&pThread->m_mutex);
			break;
		}
		pthread_mutex_unlock(&pThread->m_mutex);
	}

	if(NULL != pThread)
	{
		delete pThread;
		pThread = NULL;
	}
	return 0;
}

void ThreadPool::MoveToIdleThread(Thread* pThread)
{
	if(pThread != NULL)
	{
		pThread->m_task.func = NULL;
		pThread->m_task.data = NULL;
		if(m_ThreadList.empty())
		{
			//广播:有空闲线程
			pthread_cond_broadcast(&m_ThreadCond);
		}
		m_ThreadList.push_front(pThread);
	}
}

void ThreadPool::GetThreadRun(Task_Func func, void* arg)
{
	//获取空闲线程执行任务, 如果当前没有空闲线程则挂起等待唤醒
	pthread_mutex_lock(&m_ThreadMutex);
	if(m_ThreadList.empty())
	{
		pthread_cond_wait(&m_ThreadCond, &m_ThreadMutex);
	}
	Thread* pThread = m_ThreadList.front();
	m_ThreadList.pop_front();
	pthread_mutex_unlock(&m_ThreadMutex);

	//线程接收任务, 从睡眠中唤醒
	pthread_mutex_lock(&pThread->m_mutex);
	pThread->m_task.func = func;
	pThread->m_task.data = arg;
	pthread_cond_signal(&pThread->m_cond);
	pthread_mutex_unlock(&pThread->m_mutex);

}
