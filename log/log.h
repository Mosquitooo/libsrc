
#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include "mutex.h"

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
	~LOG();
public:
	void Trace(Log_Level level, const char* fotmat, ...);
	void SetLogLevel(int level);
private:
	void WriteFile(const char* string);
private:
	Log_Level m_loglevel;
	ofstream file;
	Mutex m_mutex;
}

#endif