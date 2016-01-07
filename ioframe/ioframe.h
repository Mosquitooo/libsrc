
#ifndef __IOFRAME_H__
#define __IOFRAME_H__

#include <sys/epoll.h>
#include <map>

#define MAX_EPOLL_EVENTS_NUM 1024
#define TCP_BUFFER_SIZE  1024

typedef void (*NetFunc)(void *);

typedef enum
{
	SOCKET_TYPE_TCP,
	SOCKET_TYPE_UDP
}SocketType;

typedef struct
{
	int socketfd;
	SocketType type;
}Socket;

typedef struct
{
	int socketfd;
	char* pMessage;
}NetMessage;

typedef struct
{
	int nSendByte;
	NetMessage Msg;
}MessageBlock;


class NetEngine
{
public:
	NetEngine();
	~NetEngine();

public:
	void Init(NetFunc );
	void BindPort(int port, SocketType type);
	void Run();

	void Send(int socketfd, const char *);
	void Close(int socketfd);
private:
	int  setnoblocking(int fd);
	void epoll_add_fd(int fd);
	void RecvFromCli(int fd);
	void SendToClient(int fd);
	
private:
	
	pthread_mutex_t m_RecvMutex;	//发送队列互斥锁
	pthread_mutex_t m_SendMutex;	//接收队列互斥锁
	
	std::queue<NetMessage> m_RecvList; //数据接收队列
	sta::map<int, NetMessage> m_SendList; //数据发送队列
	sta::map<int, MessageBlock> m_SendFailList; //数据发送失败队列

	std::map<int, Socket> m_socklist;
	epoll_event m_events[MAX_EPOLL_EVENTS_NUM];
	int m_epollfd;
	char m_buffer[TCP_BUFFER_SIZE];
	NetFunc m_callback;
	const int m_MaxListenNum;
};

#endif
