
#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include <fstream>

using namespace std;

typedef enum
{
	LOG_INFO,
	LOG_DEBUG,
	LOG_WARING,
	LOG_ERROR
}Log_Level;

class Log
{
public:
	Log(const char* filename);
	~Log();
public:
	void Trace(Log_Level level, const char* fotmat, ...);
	void SetLogLevel(Log_Level level);
private:
	void WriteFile(const char* string);
private:
	Log_Level m_loglevel;
	ofstream file;
	pthread_mutex_t m_mutex;
};

#endif
