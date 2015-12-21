

#include "dbconnector.h"

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

bool DBConnector::Exec(string& query)
{
	return mysql_query(&mysql, qurey);
}



