
#ifndef __MESGMANAGER_H__
#define __MESGMANAGER_H__

#include <pthread.h>
#include <queue>

typedef struct
{
	unsigned int nDataLen;
	char*        pMessage;
}NetMessage;

typedef struct
{
	int socketfd;
	NetMessage Msg;
}Message;

typedef void (*NetFunc)(const Message& );

class MesgManager
{
public:
	MesgManager();
	~MesgManager();
	void Init(NetFunc );
	void SendMesg(Message& );
private:
	static void* ThreadFunc(void* arg);
	
public:
	pthread_t m_trans; //�ַ��߳�ID
	pthread_mutex_t m_MesgMutex;	//��Ϣ������
	pthread_cond_t  m_MesgCond;     //��Ϣ������������

	std::queue<Message> m_RecvList; //���ݽ��ն���

	NetFunc m_callback;
};


#endif

