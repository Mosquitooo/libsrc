

#include "dbconnector.h"

//--------------------------------------DBResult-----------------------------------
DBResult::DBResult():m_ColCount(0),m_res(NULL),m_pMysql(NULL)
{
	
}

DBResult::~DBResult()
{
	
}

DBResult::Init(MYSQL* mysql)
{
	m_pMysql = mysql;
}

void DBResult::GetFieldValue(int FieldIndex, int* pvalue)
{
	if(FieldIndex < m_ColCount)
	{
		*pvalue = m_row[FieldIndex];
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
	m_row = mysql_fetch_row(res);
	if(m_row == NULL)
	{
		return false;
	}
	return true;
}

//释放
void DBResult::Release()
{
	mysql_free_result(res);
}

//--------------------------------------DBConnector--------------------------------
DBConnector::DBConnector()
{
	if(!mysql_init(&mysql))
	{
		printf("mysql init failure\n");
	}
}

DBConnector::~DBConnector()
{
	mysql_close(&mysql);
}

bool DBConnector::Connect(const string& host, const string& user, const string& password, const string& database, int port)
{
	if (!mysql_real_connect(&mysql, host, user, password, database, 0, NULL, CLIENT_MULTI_RESULTS)) 
	{ 
		fprintf(stderr, "%s\n", mysql_error(conn)); 
		return false;
	}
}

void ResetProc()
{
	m_command.assign(NULL);
}

void SetProcName(const string& ProcName)
{
	m_command.assign("call ");
	m_command.append(ProcName);
	m_command.append("(");
}

void AddProcParam(const char* ParamName, int ParamValue)
{
	m_command.append(itoa(ParamValue));
}

void AddProcParam(const char* ParamName, const char* ParamValue)
{
	m_command.append(ParamValue);
}

bool Exec()
{
	m_command.append(")");
	return mysql_query(&mysql, m_command);
}

bool DBConnector::Exec(string& query)
{
	return mysql_query(&mysql, qurey);
}



