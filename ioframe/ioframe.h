
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


class NetEngine
{
public:
	NetEngine();
	~NetEngine();

public:
	void Init(NetFunc );
	void BindPort(int port, SocketType type);
	void Run();
	void Send(const char *);
private:
	int  setnoblocking(int fd);
	void epoll_add_fd(int fd);
	
private:
	std::map<int, const char *> m_RecvList; //数据接受队列
	sta::map<int, const char *> m_SendList; //数据发送队列

	std::map<int, Socket> m_socklist;
	epoll_event m_events[MAX_EPOLL_EVENTS_NUM];
	int m_epollfd;
	char m_buffer[TCP_BUFFER_SIZE];
	NetFunc m_callback;
	const int m_MaxListenNum;
};

#endif
