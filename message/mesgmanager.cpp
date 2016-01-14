
#include "mesgmanager.h"


MesgManager::MesgManager()
{
	pthread_mutex_init(&m_MesgMutex, NULL);
	pthread_cond_init(&m_MesgCond, NULL);
}

MesgManager::~MesgManager()
{
	
	pthread_mutex_destroy(&m_MesgMutex);
	pthread_cond_destroy(&m_MesgCond);
}


void MesgManager::Init(NetFunc callback)
{
	m_callback = callback;
	pthread_create(&m_trans, NULL,ThreadFunc ,this);
}

void MesgManager::SendMesg(Message& msg)
{
	pthread_mutex_lock(&m_MesgMutex);
	
	if(m_RecvList.empty())
	{	
		m_RecvList.push(msg);
		pthread_cond_signal(&m_MesgCond);
	}
	else
	{
		m_RecvList.push(msg);
	}
	pthread_mutex_unlock(&m_MesgMutex);
}


void* MesgManager::ThreadFunc(void* arg)
{
	MesgManager* pNetMesg = (MesgManager*)arg;
	while(true)
	{
		pthread_mutex_lock(&pNetMesg->m_MesgMutex);
		if(pNetMesg->m_RecvList.empty())
		{
			pthread_cond_wait(&pNetMesg->m_MesgCond, &pNetMesg->m_MesgMutex);
		}
		
		Message Msg = {0};
		Msg = pNetMesg->m_RecvList.front();
		pNetMesg->m_callback(Msg);
		free(Msg.Msg.pMessage);
		pNetMesg->m_RecvList.pop();
		pthread_mutex_unlock(&pNetMesg->m_MesgMutex);
	}
	
}



