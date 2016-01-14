
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
	pthread_t m_trans; //分发线程ID
	pthread_mutex_t m_MesgMutex;	//消息队列锁
	pthread_cond_t  m_MesgCond;     //消息队列条件变量

	std::queue<Message> m_RecvList; //数据接收队列

	NetFunc m_callback;
};


#endif

