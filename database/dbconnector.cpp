
#include <stdlib.h>
#include "dbconnector.h"

//--------------------------------------DBResult-----------------------------------
DBResult::DBResult():m_ColCount(0),m_res(NULL),m_pMysql(NULL)
{

}

DBResult::~DBResult()
{

}

bool DBResult::Init(MYSQL* mysql)
{
	m_pMysql = mysql;
}

void DBResult::GetFieldValue(int FieldIndex, int* pvalue)
{
	if(FieldIndex < m_ColCount)
	{
		*pvalue = atoi(m_row[FieldIndex]);
	}
}

void DBResult::GetFieldValue(int FieldIndex, char* pbuff, int bufflen)
{
	if(FieldIndex < m_ColCount)
	{
		memcpy(pbuff, m_row[FieldIndex], bufflen - 1);
	}
}

//得到结果
bool DBResult::GetResult()
{
	m_res = mysql_use_result(m_pMysql); 
	m_ColCount = mysql_field_count(m_pMysql);
}

//得到下一行
bool DBResult::GetNextRow()
{
	m_row = mysql_fetch_row(m_res);
	if(m_row == NULL)
	{
		return false;
	}
	return true;
}

//释放
void DBResult::Release()
{
	mysql_free_result(m_res);
}

//--------------------------------------DBConnector--------------------------------
DBConnector::DBConnector()
{
	pthread_mutex_init(&m_mutex, NULL);
	if(!mysql_init(&mysql))
	{
		printf("mysql init failure\n");
	}
}

DBConnector::~DBConnector()
{
	mysql_close(&mysql);
	pthread_mutex_destroy(&m_mutex);
}

bool DBConnector::Connect(const char* host, const char* user, const char* password, const char* database, int port)
{
	if (!mysql_real_connect(&mysql, host, user, password, database, 0, NULL, CLIENT_MULTI_RESULTS)) 
	{ 
		fprintf(stderr, "%s\n", mysql_error(&mysql)); 
		return false;
	}
}

void DBConnector::ResetProc()
{
	m_command.assign(NULL);
}

void DBConnector::SetProcName(const string& ProcName)
{
	m_command.assign("call ");
	m_command.append(ProcName);
	m_command.append("(");
}

void DBConnector::AddProcParam(const char* ParamName, int ParamValue)
{
	snprintf(m_convert, MAX_CONVERT_SIZE - 1, "%s", ParamValue);
	m_command.append(m_convert);
}

void DBConnector::AddProcParam(const char* ParamName, const char* ParamValue)
{
	m_command.append(ParamValue);
}

bool DBConnector::Exec()
{
	m_command.append(")");
	return mysql_query(&mysql, m_command.c_str());
}

bool DBConnector::DBConnector::Exec(string& query)
{
	return mysql_query(&mysql, query.c_str());
}



