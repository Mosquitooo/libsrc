
#ifndef __IOFRAME_H__
#define __IOFRAME_H__

#include <sys/epoll.h>
#include <map>
#include "mesgmanager.h"

#define MAX_EPOLL_EVENTS_NUM 1024
#define TCP_BUFFER_SIZE  8192

typedef struct
{
	int  nByte;        			     //�ѷ��ͻ���յ��ֽ���
	char Cache[TCP_BUFFER_SIZE];     //��Ϣ��
}MessageBlock;

class NetEngine
{
public:
	NetEngine();
	~NetEngine();

public:
	void Init(NetFunc);
	void BindPort(int port);
	void Run();
	void Send(int socketfd, const char *, int);
	void Close(int socketfd);
 
private:
	int  setnoblocking(int socketfd);
	void epoll_add_fd(int socketfd);

	void AccpetFromCli(int socketfd);
	void RecvFromCli(int socketfd);

	void MovetoSendCache(int socketfd, const char*, int);
	void SendToClient(int socketfd);

	void Decode(const char* pMessage, unsigned int Datalen, MessageBlock& );
	void Encode(int socketfd, MessageBlock& msg);
	
private:
	pthread_mutex_t m_RecvMutex;	//���Ͷ��л�����
	pthread_mutex_t m_SendMutex;	//���ն��л�����
	
	std::map<int, MessageBlock>  m_RecvCacheMap; //���ݽ��ջ������
	std::map<int, MessageBlock>  m_SendCacheMap; //���ݷ��ͻ������

	MesgManager m_MesgManager;

	epoll_event m_events[MAX_EPOLL_EVENTS_NUM];
	int m_listenfd;
	int m_epollfd;
	const int m_MaxListenNum;
};

#endif
