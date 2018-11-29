#include "MySqlConnInterface.h"
#include "main.h"
#include <ctime>
#include <iostream>
#include <iomanip>

MySqlConnInterface::MySqlConnInterface() : dbConnection(true), dbQuery(&this->dbConnection, true)
{
	this->execBeforeInsert = false;
}

MySqlConnInterface::~MySqlConnInterface()
{
	this->CloseDBConnection();
}

void MySqlConnInterface::OpenDBConnection() {

	struct sockaddr_in sa;

	if (this->IP.empty() == true || this->Username.empty() == true || this->Password.empty() == true || this->dbName.empty() == true || this->tableName.empty() == true || this->port == 0)
		throw UnsufficientConfig();
	if (inet_pton(AF_INET, this->IP.c_str(), &(sa.sin_addr)) != 1)
		throw BadSrvAddr();

	if (!this->dbConnection.connect(this->dbName.c_str(), this->IP.c_str(), this->Username.c_str(), this->Password.c_str(), this->port)) {
		throw ConnError();
	}
	else
		throw OK();
}

void MySqlConnInterface::CloseDBConnection() {
	this->dbConnection.disconnect();
}

void MySqlConnInterface::Keepalive(void) {
	stringstream temp;
	Query dbQuery(&this->dbConnection, true);
	StoreQueryResult Result;

	try {
		temp << "SHOW DATABASES;" << endl;

		dbQuery = this->dbConnection.query(temp.str());
		Result = dbQuery.store();
	}
	catch (BadQuery &e) {
		cout << e.what() << endl;
	}
	catch (...) {
		cout << "--- Nieznany wyjątek rzucony z MySqlConnInterface::Keepalive(void)" << endl;
	}

}

void MySqlConnInterface::InsertIntoDb(AprsWXData* cInput) {
	stringstream temp;
	time_t currtime;
	struct tm* local;

	currtime = time(NULL);
	local = localtime(&currtime);

	if (this->execBeforeInsert == true) {
		cout << "--- Wykonanie: " << this->execBeforeInsertPath.c_str();
		system(this->execBeforeInsertPath.c_str());
		cout << endl;
	}

	temp << "INSERT INTO `" << this->dbName << "`.`" << this->tableName << "` (`Id`, `TimestampEpoch`, `TimestampDate`, `Temp`, `WindSpeed`, `WindGusts`, `WindDir`, `QNH`, `Humidity`) VALUES (NULL, CURRENT_TIMESTAMP, '" << \
		local->tm_year + 1900 << "-" << setw(2) <<  setfill('0') << \
		(local->tm_mon) + 1 << "-" << setw(2) << setfill('0') << \
		local->tm_mday << " " << setw(2) << setfill('0') << \
		local->tm_hour << ":" << setw(2) << setfill('0') << \
		local->tm_min << ":" << setw(2) << setfill('0') << \
		local->tm_sec << "', " << \
		cInput->temperature << ", " << \
		cInput->wind_speed << ", " <<  \
		cInput->wind_gusts << ", " << \
		cInput->wind_direction << ", " << \
		cInput->pressure << ", " << \
		cInput->humidity << ");" << endl;

	if (this->Debug == true)
		cout << temp.str();

	try {
		//this->dbQuery = new Query(&this->dbConnection, true);
		this->dbQuery = this->dbConnection.query(temp.str());
		this->dbSimpleResult = this->dbQuery.execute();
	}
	catch (const BadQuery& er) {
		cout << er.what();
		//delete this->dbQuery;
	}
	catch (const Exception& er) {
		cout << er.what();
		//delete this->dbQuery;
	}
	catch (...) {
		cout << "--- libmysql++ - nieznany wyjątek" << endl;
	}

	cout << this->dbSimpleResult.info();
	//delete this->dbQuery;

	this->dbQueryCounter++;

}
