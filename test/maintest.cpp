
#include "ioframe.h"
#include "thread.h"
#include "test.h"
#include <stdio.h>

void NetMessage(void* arg)
{
	printf("NetMessage: %s \n", (char*)arg);
}

int main()
{
	NetEngine  NetManager;
	NetManager.Init(NetMessage);
	NetManager.BindPort(33333, SOCKET_TYPE_TCP);
	NetManager.Run();
	return 0;
}