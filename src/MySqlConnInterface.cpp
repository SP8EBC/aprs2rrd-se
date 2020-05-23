#include "MySqlConnInterface.h"
#include "main.h"
#include <ctime>
#include <iostream>
#include <iomanip>

#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/date_time.hpp>

MySqlConnInterface::MySqlConnInterface() : dbConnection(true), dbQuery(&this->dbConnection, true)
{
	this->execBeforeInsert = false;
	this->port = 0;
	this->enable = false;
	this->dbQueryCounter = 0;

	this->schema_v1 = true;
	this->schema_v2 = false;

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
	else;
}

void MySqlConnInterface::CloseDBConnection() {
	this->dbConnection.disconnect();
}

void MySqlConnInterface::InsertIntoDbSchema2(const AprsWXData& cInput, const DataSourceConfig& config, std::string station_name) {

	if (!this->schema_v2)
		return;

	std::stringstream temp;

	boost::posix_time::ptime current_epoch = boost::posix_time::second_clock::local_time();
	//boost::date_time::second_clock<boost::posix_time::ptime>::local_time();	// static access should be here??

	boost::posix_time::time_duration epoch_seconds_duration =
												(current_epoch - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)));
	int64_t epoch_seconds = epoch_seconds_duration.ticks();

	temp << "INSERT INTO `" << this->dbName << "`.`data_station`";
	temp << "(`epoch`, `station`, `temperature`, `humidity`, `pressure`, `winddir`, `windspeed`, `windgusts`, " <<
								"`tsource`, `wsource`, `psource`, `hsource`, `rsource`) VALUES";
	temp << epoch_seconds << ", ";
	temp << "'" << station_name << "', ";
	temp << cInput.temperature << ", ";
	temp << cInput.humidity << ", ";
	temp << cInput.pressure << ", ";
	temp << cInput.wind_direction << ", ";
	temp << cInput.wind_speed << ", ";
	temp << cInput.wind_gusts << ", ";
	temp << config.getTemperatureSource() << ", ";
	temp << config.getWindSource() << ", ";
	temp << config.getPressureSource() << ", ";
	temp << config.getHumiditySource() << ", ";
	temp << config.getRainSource() << ");";

	if (this->Debug == true)
		cout << temp.str();

	try {
		this->dbQuery = this->dbConnection.query(temp.str());
		this->dbSimpleResult = this->dbQuery.execute();
	}
	catch (const BadQuery& er) {
		cout << er.what();
	}
	catch (const Exception& er) {
		cout << er.what();
	}
	catch (...) {
		cout << "--- MysqlConnInterface::InsertIntoDbSchema2:91 - unknown exception" << endl;
	}

	cout << this->dbSimpleResult.info();

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
		cout << "--- MysqlConnInterface::Keepalive:113 - Unknown exception has been thrown" << endl;
	}

}

void MySqlConnInterface::InsertIntoDb(const AprsWXData* cInput) {

	if (!this->schema_v1)
		return;

	stringstream temp;
	time_t currtime;
	struct tm* local;

	currtime = time(NULL);
	local = localtime(&currtime);

	if (this->execBeforeInsert == true) {
		cout << "--- MysqlConnInterface::InsertIntoDb:132 - Executing: " << this->execBeforeInsertPath.c_str();
		(void)system(this->execBeforeInsertPath.c_str());
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
		cout << "--- MysqlConnInterface::InsertIntoDb:167 - unknown exception" << endl;
	}

	cout << this->dbSimpleResult.info();
	//delete this->dbQuery;

	this->dbQueryCounter++;

}
