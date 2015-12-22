
/******************************************************************
File name:		dbconnector.h
Description:	database
Create Date:	2015-12-22

******************************************************************/

#ifndef __DBCONNECTOR_H__
#define __DBCONNECTOR_H__

#include "mysql/mysql.h"
#include <string>

using namespace std;

class DBResult
{
public:
	DBResult();
	
	~DBResult();
	
	bool Init(MYSQL* );
	
	void GetFieldValue(int FieldIndex, int* value);
	void GetFieldValue(int FieldIndex, char* pbuff, int bufflen);
	
	//得到结果
	bool GetResult();
	
	//得到下一行
	bool GetNextRow();
	//释放
	void Release();
private:
	int 		m_ColCount;
	MYSQL_RES* 	m_res;
	MYSQL_ROW  	m_row;
	MYSQL*     	m_pMysql;
};

class DBConnector
{
public:
	DBConnector();

	~DBConnector();
	
	bool Connect(const char* , const char* , const char* , const char* , int );
	
	void ResetProc();
	void SetProcName(const string& ProcName);
	void AddProcParam(const char* ParamName, int ParamValue);
	void AddProcParam(const char* ParamName, const char* ParamValue);
	
	bool Exec();
	bool Exec(string& );
	
	DBResult* GetResult(){return &m_result;}
	
	int GetErrno();

private:
	MYSQL mysql;
	DBResult	m_result;
	string 		m_command;
	
};

#endif

