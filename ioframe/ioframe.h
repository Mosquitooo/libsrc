
#ifndef __IOFRAME_H__
#define __IOFRAME_H__

#include <sys/epoll.h>
#include <map>

#define MAX_EPOLL_EVENTS_NUM 1024
#define TCP_BUFFER_SIZE  1024

static const int MaxListenNum = 10;


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
	void Init();
	void BindPort(int port, SocketType type);
	void Run();
private:
	int  setnoblocking(int fd);
	void epoll_add_fd(int fd);
	
private:
	std::map<int, Socket> m_socklist;
	epoll_event m_events[MAX_EPOLL_EVENTS_NUM];
	int m_epollfd;
	char m_buffer[TCP_BUFFER_SIZE];
};

#endif
