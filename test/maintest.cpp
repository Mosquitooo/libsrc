
#include "ioframe.h"
#include "thread.h"
#include "test.h"
#include "mesgmanager.h"
#include <stdio.h>


NetEngine  NetManager;
void TransferMsg(const Message& msg)
{
	printf("socket :%d\n", msg.socketfd);
	printf("datelen:%d\n", msg.Msg.nDataLen);
	printf("Message:%s\n", msg.Msg.pMessage);
	NetManager.Send(msg.socketfd,msg.Msg.pMessage,msg.Msg.nDataLen);
	
}


int main()
{
	NetManager.Init(TransferMsg);
	NetManager.BindPort(33333);
	NetManager.Run();
	return 0;
}

