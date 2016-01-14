
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
	pthread_mutex_init(&m_RecvMutex, NULL);
	pthread_mutex_init(&m_SendMutex, NULL);
}

NetEngine::~NetEngine()
{
	pthread_mutex_destroy(&m_RecvMutex);
	pthread_mutex_destroy(&m_SendMutex);
}

void NetEngine::Init(NetFunc callback)
{
	m_MesgManager.Init(callback);
	assert((m_epollfd = epoll_create(MAX_EPOLL_EVENTS_NUM)) != -1);
}

void NetEngine::BindPort(int port)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if((m_listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
	}
	
	if(bind(m_listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
	}
	
	if(listen(m_listenfd, m_MaxListenNum) == -1)
	{
		perror("listen");
	}
	
	//�ڷ���������accept֮ǰ���ͻ�����������RST��ֹ���ӣ����¸ոս��������ӴӾ����������Ƴ���
	//����׽ӿڱ����ó�����ģʽ���������ͻ�һֱ������accept�����ϣ�ֱ������ĳ���ͻ�����һ���µ�����Ϊֹ��
	//�����ڴ��ڼ䣬������������������accept�����ϣ����������е��������������ò�������
	//����취�ǰѼ����׽ӿ�����Ϊ�����������ͻ��ڷ���������accept֮ǰ��ֹĳ������ʱ��accept���ÿ�����������-1��
	epoll_add_fd(m_listenfd);
}

void NetEngine::Run()
{
	while(true)
	{
		int number = epoll_wait(m_epollfd, m_events, MAX_EPOLL_EVENTS_NUM, -1);
		if(number <= 0)
		{
			break;
		}

		for(int i = 0; i < number; ++i)
		{
			int socketfd = m_events[i].data.fd;
			
			if(socketfd == m_listenfd)
			{
				AccpetFromCli(socketfd);
			}
			else if(m_events[i].events & EPOLLIN)
			{
				RecvFromCli(socketfd);
			}
			else if(m_events[i].events & EPOLLOUT)
			{
				SendToClient(socketfd);
			}
			else
			{
				
			}
		}
	}
}

void NetEngine::AccpetFromCli(int socketfd)
{
	struct sockaddr_in cliaddr;
	socklen_t cliaddr_len = sizeof(cliaddr);
	bzero(&cliaddr, sizeof(cliaddr));
	
	//�ڱ��ش���ģʽ��, �������ͬʱ����epollֻ��֪ͨһ��, ���Դ˴�Ӧ��ѭ��accpet
	int confd = 0;
	while(true)
	{
		confd = accept(socketfd, (struct sockaddr*)&cliaddr, &cliaddr_len);
		if(confd <= 0)
		{
			if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
				perror("accept");
			break;
		}
		epoll_add_fd(confd);
		MessageBlock Msg = {0};
		
		pthread_mutex_lock(&m_RecvMutex);
		m_RecvCacheMap.insert(std::map<int, MessageBlock>::value_type(confd, Msg));
		pthread_mutex_unlock(&m_RecvMutex);

		pthread_mutex_lock(&m_SendMutex);
		m_SendCacheMap.insert(std::map<int, MessageBlock>::value_type(confd, Msg));
		pthread_mutex_unlock(&m_SendMutex);
	}
}


/*  protocol: |datalength|payload|MagicChar|    */
/*            |   4 byte |    datalength   |    */
/*                               |  1 byte |    */

void NetEngine::RecvFromCli(int socketfd)
{
	pthread_mutex_lock(&m_RecvMutex);
	std::map<int, MessageBlock>::iterator it = m_RecvCacheMap.find(socketfd);
	if(it == m_RecvCacheMap.end())
	{	
		pthread_mutex_unlock(&m_RecvMutex);
		return;
	}

	int ret = 0;
	bool bClose = false;
	
	while(true)
	{
		ret = recv(socketfd, it->second.Cache + it->second.nByte, TCP_BUFFER_SIZE - it->second.nByte, 0);
		if(ret < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;
			}
			else if(errno == EINTR)
			{
				continue;
			}
			else
			{
				bClose = true;
				break;
			}
		}
		else if(ret == 0)
		{
			bClose = true;
			break;
		}
		else{
			it->second.nByte += ret;
		}
	}
	pthread_mutex_unlock(&m_RecvMutex);

	if(bClose)
	{
		Close(socketfd);
		return;
	}
	
	//�������
	pthread_mutex_lock(&m_RecvMutex);
	Encode(socketfd, it->second);
	pthread_mutex_unlock(&m_RecvMutex);
}


void NetEngine::Send(int socketfd, const char * pMessage, int MsgSize)
{
	if(pMessage == NULL){
		return;
	}

	int nwrite = 0;
	int nCount = 0;

	//���
	MessageBlock msg;
	Decode(pMessage, MsgSize, msg);
	
	//����
	while(nCount < msg.nByte)
	{
		nwrite = send(socketfd, msg.Cache, msg.nByte, 0);
		if(nwrite < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//д�뷢�ͻ�����
				MovetoSendCache(socketfd, msg.Cache + nCount, msg.nByte - nCount);
				break;
			}
			else if(errno == EINTR)
			{
				continue;
			}
			else
			{
				Close(socketfd);
				break;
			}
		}
		else if(nwrite == 0)
		{
			Close(socketfd);
			break;
		}
		else
		{
			nCount += nwrite;
		}
	}
	
}

void NetEngine::Close(int socketfd)
{
	pthread_mutex_lock(&m_SendMutex);	
	std::map<int, MessageBlock>::iterator its = m_SendCacheMap.find(socketfd);
	if(its != m_SendCacheMap.end())
	{
		m_SendCacheMap.erase(its);
	}
	pthread_mutex_unlock(&m_SendMutex);

	
	pthread_mutex_lock(&m_RecvMutex);
	std::map<int, MessageBlock>::iterator itr = m_RecvCacheMap.find(socketfd);
	if(itr != m_RecvCacheMap.end())
	{
		m_RecvCacheMap.erase(itr);
	}
	pthread_mutex_unlock(&m_RecvMutex);

	epoll_ctl(m_epollfd, EPOLL_CTL_DEL, socketfd, 0);
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

void NetEngine::MovetoSendCache(int socketfd, const char* buffer, int nDataLen)
{
	pthread_mutex_lock(&m_SendMutex);
	std::map<int, MessageBlock>::iterator its = m_SendCacheMap.find(socketfd);
	if(its == m_SendCacheMap.end())
	{
		return;
	}

	if(its->second.nByte + nDataLen < TCP_BUFFER_SIZE)
	{
		memcpy(its->second.Cache, buffer, nDataLen);
		its->second.nByte += nDataLen;
	}
	pthread_mutex_unlock(&m_SendMutex);
}


void NetEngine::SendToClient(int socketfd)
{
	pthread_mutex_lock(&m_SendMutex);
	std::map<int, MessageBlock>::iterator iter = m_SendCacheMap.find(socketfd);
	if(iter == m_SendCacheMap.end() || (iter != m_SendCacheMap.end() && iter->second.nByte == 0))
	{
		return;
	}

	int nCount = 0, nwrite = 0;	
	bool bClose = false;
	while(nCount < iter->second.nByte)
	{
		nwrite = send(socketfd, iter->second.Cache + nCount, iter->second.nByte - nCount, 0);
		if(nwrite < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;
			}
			else if(errno == EINTR)
			{
				continue;
			}
			else
			{
				bClose = true;
				break;
			}
		}
		else if(nwrite == 0)
		{		
			bClose = true;
			break;
		}
		else
		{
			nCount += nwrite;
		}
	}
	
	if(nCount > 0)
	{
		memmove(iter->second.Cache, iter->second.Cache + nCount, iter->second.nByte - nCount);
		iter->second.nByte -= nCount;
	}
	pthread_mutex_unlock(&m_SendMutex);
	
	if(bClose)
	{
		Close(socketfd);
	}
}

void NetEngine::Decode(const char* pMessage, unsigned int MsgSize, MessageBlock& msg)
{
	memcpy(msg.Cache, &MsgSize, sizeof(unsigned int));
	memcpy(msg.Cache + sizeof(unsigned int), pMessage, MsgSize);
	msg.nByte = MsgSize + sizeof(unsigned int);
}

void NetEngine::Encode(int socketfd, MessageBlock& msg)
{
	Message data = {0};
	memcpy(&data.Msg.nDataLen, msg.Cache, sizeof(unsigned int));

	if(data.Msg.nDataLen + sizeof(unsigned int) > msg.nByte)
	{
		return;
	}

	//char cMagic;
	//��֤�ָ���
	//memcpy(&cMagic, msg.Cache + sizeof(unsigned int) + data.nDataLen, sizeof(char));

	data.Msg.pMessage = (char*)malloc(data.Msg.nDataLen);
	if(data.Msg.pMessage == NULL)
	{
		return;
	}

	//���û�����
	unsigned int nwrite = data.Msg.nDataLen + sizeof(unsigned int);
	unsigned int nrest = msg.nByte - nwrite;
	memcpy(data.Msg.pMessage, msg.Cache + sizeof(unsigned int), data.Msg.nDataLen);
	memmove(msg.Cache, msg.Cache + nwrite, nrest);
	msg.nByte = nrest;
	data.socketfd = socketfd;
	m_MesgManager.SendMesg(data);
}


