
#ifndef __IOFRAME_H__
#define __IOFRAME_H__

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
	NetEngine(const char* host, int port);
	~NetEngine();

public:
	void Init();
	void BindPort(int port, SocketType type);
	void Run();
private:
	int  setnoblocking(int fd);
	void epoll_add_fd(int epollfd, int fd);
	
private:
	int m_epollfd;
	epoll_event m_events[MAX_EPOLL_EVENTS_NUM];
	std::list<Socket> m_fdlist;
	char m_buffer[TCP_BUFFER_SIZE];
};

#endif