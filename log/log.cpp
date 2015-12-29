
#include "log.h"

#deinfe MAX_STR_LEN 1024

LOG::LOG(const char* filename)
{
	file.open(filename, ios::out);
}

LOG::~LOG()
{
	file.close();
}

void LOG::Trace(Log_Level level, const char* fotmat, ...)
{
	if(m_loglevel < level) return;
	char str[MAX_STR_LEN] = {0};
	va_list args = NULL;
	va_start(args,fotmat);
	vsprintf(str, cmd, args);
	WriteFile(str);
	va_end(args);
}

void LOG::WriteFile(const char* string)
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

void LOG::SetLogLevel(int level)
{
	m_loglevel = level;
}
