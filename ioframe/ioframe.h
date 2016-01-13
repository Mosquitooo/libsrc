
#ifndef __IOFRAME_H__
#define __IOFRAME_H__

#include <sys/epoll.h>
#include <map>
#include <queue>

#define MAX_EPOLL_EVENTS_NUM 1024
#define TCP_BUFFER_SIZE  8192

typedef void (*NetFunc)(void *);

typedef struct
{
	unsigned int nDataLen;
	char*        pMessage;
}NetMessage;

typedef struct
{
	int  nByte;        			     //已发送或接收的字节数
	char Cache[TCP_BUFFER_SIZE];     //消息体
}MessageBlock;

/*
protocol:
	unsigned int length	//数据长度
	char*       payload	//净荷
	char        cMagic	//魔法字符, 分割符
*/

//send: 直接发送数据. 如果返回错误则存储到缓冲区, 下次触发继续发送
//recv：存储到缓冲区, 之后放到任务队列.

class NetEngine
{
public:
	NetEngine();
	~NetEngine();

public:
	void Init(NetFunc );
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
	void Encode(MessageBlock& msg);
	
private:
	
	pthread_mutex_t m_RecvMutex;	//发送队列互斥锁
	pthread_mutex_t m_SendMutex;	//接收队列互斥锁
	
	std::queue<NetMessage> m_RecvList; //数据接收队列
	std::map<int, MessageBlock>  m_RecvCacheMap;      //数据接收缓存队列
	std::map<int, MessageBlock>  m_SendCacheMap; //数据发送缓存队列

	epoll_event m_events[MAX_EPOLL_EVENTS_NUM];
	int m_listenfd;
	int m_epollfd;
	NetFunc m_callback;
	const int m_MaxListenNum;
};

#endif
