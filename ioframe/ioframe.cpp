
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "ioframe.h"


NetEngine::NetEngine()
{
	
}

NetEngine::~NetEngine()
{
	
}

void NetEngine::Init()
{
	assert((m_epollfd = epoll_create(MAX_EPOLL_EVENTS_NUM)) != -1);
}

void NetEngine::BindPort(int port, SocketType type)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	addr.sin_port = htons(port);
	
	int socketfd;
	switch(type)
	{
		case SOCKET_TYPE_TCP:
		{
			assert((socketfd = socket(PF_INET, SOCK_STREAM, 0)) >= 0);
			assert(bind(socketfd, (struct sockaddr*)&addr, sizeof(addr)) != -1);
			assert(listen(socketfd, MaxListenNum) != -1);
		}
		break;
		case SOCKET_TYPE_UDP:
		{
			assert((socketfd = socket(PF_INET, SOCK_DGRAM, 0)) >= 0);
			assert(bind(socketfd, (struct sockaddr*)&addr, sizeof(addr)) != -1);
		}
		break;
		default:
			return;
		break;
	}
	
	//在服务器调用accept之前，客户端主动发送RST终止连接，导致刚刚建立的连接从就绪队列中移出。
	//如果套接口被设置成阻塞模式，服务器就会一直阻塞在accept调用上，直到其他某个客户建立一个新的连接为止。
	//但是在此期间，服务器单纯地阻塞在accept调用上，就绪队列中的其他描述符都得不到处理。
	//解决办法是把监听套接口设置为非阻塞，当客户在服务器调用accept之前中止某个连接时，accept调用可以立即返回-1。
	epoll_add_fd(socketfd);
	
	Socket soc;
	soc.socketfd = socketfd;
	soc.type = type;
	m_socklist.insert(std::map<int,Socket>::value_type(socketfd, soc));
}

void NetEngine::Run()
{
	while(true)
	{
		int number = epoll_wait(m_epollfd, m_events, MAX_EPOLL_EVENTS_NUM, -1);
		if(number <= 0)
		{
			printf("epoll failure");
			break;
		}
		
		for(int i = 0; i < number; ++i)
		{
			int socketfd = m_events[i].data.fd;
			std::map<int,Socket>::iterator it = m_socklist.find(socketfd);
			
			if(it != m_socklist.end())
			{
				switch(it->second.type)
				{
					case SOCKET_TYPE_TCP:
					{
						struct sockaddr_in cliaddr;
						socklen_t cliaddr_len = sizeof(cliaddr);
						bzero(&cliaddr, sizeof(cliaddr));
						
						//在边沿触发模式下, 多个连接同时到达epoll只会通知一次, 所以此处应该循环accpet
						int confd = 0;
						while((confd = accept(socketfd, (struct sockaddr*)&cliaddr, &cliaddr_len)) > 0)
						{
							epoll_add_fd(confd);
						}
						
						if(confd == -1)
						{
							if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
							perror("accept");
						}
					}
					break;
					
					case SOCKET_TYPE_UDP:
					{
						//dosomething
					}
					break;
						default:
					break;
				}
			}
			else
			{
				if(m_events[i].events & EPOLLIN)
				{
					while(true)
					{
						//在边沿触发模式下, 需要循环读取接收缓冲区的数据
						memset(m_buffer, 0x00, sizeof(m_buffer));
						int ncount = 0;
						int ret = 0;
						while((ret = recv(socketfd, m_buffer, TCP_BUFFER_SIZE - 1, 0)) > 0)
						{
							ncount += ret;
						}
						
						if(ret < 0)
						{
							if(errno == EAGAIN || errno == EWOULDBLOCK)
							{
								break;
							}
							close(socketfd);
							break;
						}
						else if(ret == 0)
						{
							close(socketfd);
						}
						else
						{
							//dosomething
							printf("dosomething: %s\n", m_buffer);
						}
					}
				}
				else
				{
					printf("unkonw data");
				}
			}
		}
	}
}

int NetEngine::setnoblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return new_option;
}

void NetEngine::epoll_add_fd(int fd)
{
	epoll_event event;
	event.data.fd = fd;
	event.events  = EPOLLIN | EPOLLET;
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event);
	setnoblocking(fd);
}
