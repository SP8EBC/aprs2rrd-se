#ifndef MYSQLCONNINTERFACE_H
#define MYSQLCONNINTERFACE_H

#include "AprsWXData.h"
#include "DataSourceConfig.h"
#include "DiffCalculator.h"
#include "Telemetry.h"
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

class AlreadyDisconnected: public exception {
public:
	virtual const char* what() const throw() {
		return "\n--- libmysql++: This connection has been already closed\n";
    }
};

class ConnError: public exception {
	public:
	virtual const char* what() const throw() {
		return "\n--- libmysql++: Error connecting to MySQL server!!\n";
    }
};

class OK: public exception {
	public:
	virtual const char* what() const throw() {
		return "\n--- libmysql++: Connection has been established\n";
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
	
	bool schema_v2;
	bool schema_v1;

	bool dumpHolfuy;
	bool dumpDiff;
	bool dumpTelemetry;

	MySqlConnInterface();
	~MySqlConnInterface();
	
	void OpenDBConnection();
	void CloseDBConnection();
	void InsertIntoDb(const AprsWXData* const cInput);
	void InsertIntoDbSchema2(const AprsWXData& cInput, const DataSourceConfig& config, std::string station_name);
	void InsertDiff(const AprsWXData& input, const DiffCalculator& diffCalculator, std::string station_name);
	void InsertTelmetry(const Telemetry& input, std::string station_name);
	void Keepalive(void);
	
	bool execBeforeInsert;
	string execBeforeInsertPath;
	
	bool Debug = false;

};

#endif // MYSQLCONNINTERFACE_H
