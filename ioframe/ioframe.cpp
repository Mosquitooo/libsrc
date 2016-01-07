
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


NetEngine::NetEngine():m_MaxListenNum(10)
{

}

NetEngine::~NetEngine()
{
	
}

void NetEngine::Init(NetFunc callback)
{
	m_callback = callback;
	assert((m_epollfd = epoll_create(MAX_EPOLL_EVENTS_NUM)) != -1);
}

void NetEngine::BindPort(int port, SocketType type)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	
	int socketfd;
	switch(type)
	{
		case SOCKET_TYPE_TCP:
		{
			assert((socketfd = socket(PF_INET, SOCK_STREAM, 0)) >= 0);
			assert(bind(socketfd, (struct sockaddr*)&addr, sizeof(addr)) != -1);
			assert(listen(socketfd, m_MaxListenNum) != -1);
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
					RecvFromCli(socketfd);
				}
				
				if(m_events[i].events & EPOLLOUT)
				{
					SendToClient(socketfd);
				}
			}
		}
	}
}

void NetEngine::RecvFromCli(int socketfd)
{
	//在边沿触发模式下, 需要循环读取接收缓冲区的数据
	memset(m_buffer, 0x00, sizeof(m_buffer));
	int ncount = 0, ret = 0;
	while((ret = recv(socketfd, m_buffer, TCP_BUFFER_SIZE - 1, 0)) > 0)
	{
		ncount += ret;
	}
	
	if(ncount == 0 && ret < 0)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			//非阻塞模式，当前缓冲区无数据可读。
		}
		else if(errno == EINTR)
		{
			//被中断, 继续处理
			continue;
		}
		else
		{
			//其它未知错误
			Close(socketfd);
		}
	}
	else if(ret == 0)
	{
		//对端socket关闭, 发送过FIN
		Close(socketfd);
	}
	else
	{
		
	}
}

void NetEngine::Send(int socketfd, const char * pMessage)
{
	if(pMessage == NULL)
	{
		return;
	}
	
	int msglen = strlen(pMessage);
	NetMessage Msg = {0};
	
	char Msg.pMessage = (char*)malloc(msglen + 1);
	if(Msg.pMessage == NULL)
	{
		return;
	}
	memset(Msg.pMessage, 0x00, msglen + 1);
	
	pthread_mutex_lock(&m_SendMutex);
	std::map<int, NetMessage>::iterator it = m_SendList.find(socketfd);
	if(it != m_SendList.end())
	{
		it->second.push(Msg);
	}
	else
	{
		m_SendList.insert(std::map<int, NetMessage>::value_type(socketfd, Msg));
	}
	pthread_mutex_unlock(&m_SendMutex);
}

void NetEngine::Close(int socketfd)
{
	std::map<int, MessageBlock>::iterator iter = m_SendFailList.find(fd);
	if(iter != m_SendFailList.end())
	{
		m_SendFailList.erase(iter);
	}
	
	std::map<int, NetMessage>::iterator it = m_SendList.find(fd);
	if(iter != m_SendFailList.end())
	{
		m_SendList.erase(it);
	}
	close(socketfd);
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

void NetEngine::SendToClient(int fd)
{
	int nwrite = 0, nDataSize = 0, n = 0;
	std::map<int, MessageBlock>::iterator iter = m_SendFailList.find(fd);
	if(iter != m_SendFailList.end() && !)
	{
		while(!it->second.empty())
		{
			MessageBlock Msg = it->second.front();
			nDataSize = strlen(Msg.pMessage);
			n = nDataSize - Msg.nSendByte;
			while(n > 0)
			{
				nwrite = send(fd, Msg.pMessage + Msg.nSendByte , n);
				if(nwrite < n)
				{
					return;
				}
				n -= nwrite;
				Msg.nSendByte += nwrite;
			}
			it->second.pop();
		}
	}
	
	std::map<int, NetMessage>::iterator it = m_SendList.find(fd);
	if(it != m_SendList.end())
	{
		if(it->second.empty())
		{
			return;
		}
		
		NetMessage Msg = it->second.front();
		
		nDataSize = strlen(Msg.pMessage);
		n = nDataSize;
		while(n > 0)
		{
			nwrite = send(fd, Msg.pMessage + nDataSize - n, n);
			if(nwrite < n)
			{
				//对端缓冲区满, 消息发送失败, 缓存到失败队列, 记录已发送的字节数
				MessageBlock message = {0};
				memcpy(&message.Msg, &Msg, sizeof(Msg));
				message = nDataSize - n;
				m_SendFailList.insert(std::<int, MessageBlock>::value_type(fd, message));
				
				if(nwrite == -1 && errno != EAGAIN)
				{
					perror(strerr(errno));
				}
				
				return;
			}
			n -= nwrite;
		}
		free(Msg.pMessage);
		it->second.pop();
	}
}
