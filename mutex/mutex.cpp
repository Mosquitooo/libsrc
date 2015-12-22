
#include<pthread.h>
#include "mutex.h"

Mutex::Mutex()
{
	pthread_mutex_init(&m_Mutex, NULL);
};

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_Mutex); 
}

bool Mutex::Lock()
{
	pthread_mutex_lock(&m_Mutex); 
}

bool Mutex::Unlock()
{
	pthread_mutex_unlock(&m_Mutex); 
}
