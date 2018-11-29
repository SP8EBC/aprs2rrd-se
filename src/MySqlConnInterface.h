#ifndef MYSQLCONNINTERFACE_H
#define MYSQLCONNINTERFACE_H

#include "AprsWXData.h"
#include <string>
#include <exception>
#include <arpa/inet.h>
#include <mysql++.h>
#include <result.h>

using namespace mysqlpp;
using namespace std;

class UnsufficientConfig : public exception {
	
};

class BadSrvAddr : public exception {
	
};

class ConnError: public exception {
	public:
	virtual const char* what() const throw() {
		return "\n--- libmysql++: Blad polaczenia z baza danych!!\n";
    }
};

class OK: public exception {
	public:
	virtual const char* what() const throw() {
		return "\n--- libmysql++: Polaczono z baza danych\n";
    }	
};

class MySqlConnInterface
{
private:
	Connection dbConnection;
	Query dbQuery;
	StoreQueryResult dbResult;
	SimpleResult dbSimpleResult;
	int dbQueryCounter;
public:

	bool enable;

	string IP;
	unsigned port;
	
	string Username;
	string Password;
	
	string dbName;
	string tableName;
	
	MySqlConnInterface();
	~MySqlConnInterface();
	
	void OpenDBConnection();
	void CloseDBConnection();
	void InsertIntoDb(AprsWXData* cInput);
	void Keepalive(void);
	
	bool execBeforeInsert;
	string execBeforeInsertPath;
	
	bool Debug = false;

};

#endif // MYSQLCONNINTERFACE_H
