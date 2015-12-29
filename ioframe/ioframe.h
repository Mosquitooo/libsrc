
#ifndef __IOFRAME_H__
#define __IOFRAME_H__

class NetEngine
{
public:
	NetEngine(const char* host, int port);
	~NetEngine();

public:
	void Init();
	void Run();
private:
	
};

#endif