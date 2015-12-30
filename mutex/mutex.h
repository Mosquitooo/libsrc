
#ifndef __MUTEX_H__
#define __MUTEX_H__

class Mutex
{
public:
	Mutex();
	~Mutex();
public:
	bool Lock();
	bool Unlock();
private:
	pthread_mutex_t m_Mutex;
};

class AutoMutex
{
public:
	AutoMutex(Mutex* pMutex):m_pMutex(pMutex)
	{
		m_pMutex->Lock();
	}
	
	~AutoMutex()
	{
		m_pMutex->Unlock();
	}
private:
	Mutex *m_pMutex;
};

#define _AUTOMUTEX(t) (AutoMutex mutex(t))
#endif
