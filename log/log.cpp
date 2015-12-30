
#include <stdarg.h>
#include "log.h"

#define MAX_STR_LEN 1024

Log::Log(const char* filename)
{
	file.open(filename, ios::out);
}

Log::~Log()
{
	file.close();
}

void Log::Trace(Log_Level level, const char* fotmat, ...)
{
	if(m_loglevel < level) return;
	char str[MAX_STR_LEN] = {0};
	va_list args;
	va_start(args,fotmat);
	vsprintf(str, fotmat, args);
	WriteFile(str);
	va_end(args);
}

void Log::WriteFile(const char* string)
{
	if(string == NULL) return;
	
	//需要进行同步
	if(file.is_open())
	{
		file << string;
	}
	else
	{
		return;
	}
}

void Log::SetLogLevel(Log_Level level)
{
	m_loglevel = level;
}
