
#ifndef __DBCONNECTOR_H__
#define __DBCONNECTOR_H__

#include "mysql/mysql.h"

class DBConnector
{
public:
	DBConnector();

	~DBConnector();

	bool Exec(string& );

	
private:
	MYSQL mysql;
	MYSQL_RES* ret;
	MYSQL_ROW row;
};

#endif

