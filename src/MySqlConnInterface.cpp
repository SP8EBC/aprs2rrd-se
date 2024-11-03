#include "MySqlConnInterface.h"
#include "main.h"
#include "TimeTools.h"

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

	this->dumpDiff = false;
	this->dumpHolfuy = false;
	this->dumpTelemetry = false;

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
	if (this-dbConnection.connected())
		this->dbConnection.disconnect();
	else throw AlreadyDisconnected();
}

void MySqlConnInterface::InsertIntoDbSchema2(AprsPacket& packet, AprsWXData& cInput, const DataSourceConfig& config, std::string station_name) {

	if (!this->schema_v2)
		return;

	const bool hasTimestamp = (cInput.packetUtcTimestamp != 0);

	std::stringstream temp;
	temp.str("");

	boost::posix_time::ptime current_epoch = boost::posix_time::second_clock::universal_time();
	//boost::date_time::second_clock<boost::posix_time::ptime>::local_time();	// static access should be here??

	boost::posix_time::time_duration epoch_seconds_duration =
												(current_epoch - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)));
	int64_t epoch_seconds = epoch_seconds_duration.total_seconds();

	if (hasTimestamp) {
		epoch_seconds = cInput.packetUtcTimestamp;
		struct tm time = TimeTools::getTmStructFromUndefinedTzPtime(cInput.packetLocalTimestmp);

		temp << "INSERT INTO `" << this->dbName << "`.`data_station`";
		temp << "(`epoch`, `datetime`, `station`, `temperature`, `humidity`, `pressure`, `winddir`, `windspeed`, `windgusts`, " <<
									"`tsource`, `wsource`, `psource`, `hsource`, `rsource`) VALUES (";
		temp << epoch_seconds << ", ";
		temp << "'" << \
		time.tm_year + 1900 << "-" << setw(2) <<  setfill('0') << \
		(time.tm_mon) + 1 << "-" << setw(2) << setfill('0') << \
		time.tm_mday << " " << setw(2) << setfill('0') << \
		time.tm_hour << ":" << setw(2) << setfill('0') << \
		time.tm_min << ":" << setw(2) << setfill('0') << \
		time.tm_sec << 
		"',";
		temp << "'" << station_name << "', ";
		temp << cInput.temperature << ", ";
		temp << cInput.humidity << ", ";
		temp << cInput.pressure << ", ";
		temp << cInput.wind_direction << ", ";
		temp << cInput.wind_speed << ", ";
		temp << cInput.wind_gusts << ", ";
		temp << "'" << config.getTemperatureSource() << "', ";
		temp << "'" << config.getWindSource() << "', ";
		temp << "'" << config.getPressureSource() << "', ";
		temp << "'" << config.getHumiditySource() << "', ";
		temp << "'" << config.getRainSource() << "');" << endl;
	}
	else {
		temp << "INSERT INTO `" << this->dbName << "`.`data_station`";
		temp << "(`epoch`, `datetime`, `station`, `temperature`, `humidity`, `pressure`, `winddir`, `windspeed`, `windgusts`, " <<
									"`tsource`, `wsource`, `psource`, `hsource`, `rsource`) VALUES (";
		temp << epoch_seconds << ", ";
		temp << "CURRENT_TIMESTAMP, ";
		temp << "'" << station_name << "', ";
		temp << cInput.temperature << ", ";
		temp << cInput.humidity << ", ";
		temp << cInput.pressure << ", ";
		temp << cInput.wind_direction << ", ";
		temp << cInput.wind_speed << ", ";
		temp << cInput.wind_gusts << ", ";
		temp << "'" << config.getTemperatureSource() << "', ";
		temp << "'" << config.getWindSource() << "', ";
		temp << "'" << config.getPressureSource() << "', ";
		temp << "'" << config.getHumiditySource() << "', ";
		temp << "'" << config.getRainSource() << "');" << endl;
	}

	if (this->Debug == true) {
		cout << temp.str() << endl;
	}

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
		cout << "--- MysqlConnInterface::InsertIntoDbSchema2:135 - unknown exception" << endl;
	}

	if (cInput.additionalTemperature.size() > 0) {
		cout << "--- MysqlConnInterface::InsertIntoDbSchema2:139 - cInput.additionalTemperature.size() : " << cInput.additionalTemperature.size() << endl;

		for (std::pair<uint64_t, float> elem : cInput.additionalTemperature) {
			const uint64_t elemTimestamp = elem.first; 
			const struct tm elemTime = TimeTools::getTmStructFromUndefinedTzEpoch(elemTimestamp);

			const float elemTemperature = elem.second;

			if ((int)(elemTemperature * 10.0f) == 0 ) {
				continue;
			}
			
			InsertIntoDbSchemaTatry(packet, elemTimestamp, elemTemperature, station_name);

			std::stringstream queryStr;

			queryStr << "INSERT INTO `" << this->dbName << "`.`data_station`";
			queryStr << "(`epoch`, `datetime`, `station`, `temperature`, `humidity`, `pressure`, `winddir`, `windspeed`, `windgusts`, " <<
										"`tsource`, `wsource`, `psource`, `hsource`, `rsource`) VALUES (";
			queryStr << elemTimestamp << ", ";
			queryStr << "'" << \
			elemTime.tm_year + 1900 << "-" << setw(2) <<  setfill('0') << \
			(elemTime.tm_mon) + 1 << "-" << setw(2) << setfill('0') << \
			elemTime.tm_mday << " " << setw(2) << setfill('0') << \
			elemTime.tm_hour << ":" << setw(2) << setfill('0') << \
			elemTime.tm_min << ":" << setw(2) << setfill('0') << \
			elemTime.tm_sec << 
			"',";
			queryStr << "'" << station_name << "', ";
			queryStr << elemTemperature << ", ";
			queryStr << 0.0f << ", ";
			queryStr << 0 << ", ";
			queryStr << 0 << ", ";
			queryStr << 0.0f << ", ";
			queryStr << 0.0f << ", ";
			queryStr << "'" << config.getTemperatureSource() << "', ";
			queryStr << "'" << config.getWindSource() << "', ";
			queryStr << "'" << config.getPressureSource() << "', ";
			queryStr << "'" << config.getHumiditySource() << "', ";
			queryStr << "'" << config.getRainSource() << "');" << endl;

			if (this->Debug == true) {
				cout << "--- MysqlConnInterface::InsertIntoDbSchema2 - Inserting historical data from positionless weather frame" << endl;
				cout << queryStr.str() << endl;
			}

			try {
				this->dbQuery = this->dbConnection.query(queryStr.str());
				this->dbSimpleResult = this->dbQuery.execute();
			}
			catch (const BadQuery& er) {
				cout << er.what();
			}
			catch (const Exception& er) {
				cout << er.what();
			}
			catch (...) {
				cout << "--- MysqlConnInterface::InsertIntoDbSchema2:189 - unknown exception" << endl;
			}
		}

		cInput.additionalTemperature.clear();
	}

	if (this->Debug == true) {
		cout << "--- MysqlConnInterface::InsertIntoDbSchema2:198 - cInput.additionalTemperature.size() : " << cInput.additionalTemperature.size() << endl;
	}

	cout << this->dbSimpleResult.info();

}

void MySqlConnInterface::InsertDiff(const AprsWXData& input, const DiffCalculator& diffCalculator, std::string station_name) {
	if (!this->dumpDiff)
		return;

	if (!diffCalculator.enable)
		return;

	std::stringstream temp;
	temp.str("");

	boost::posix_time::ptime current_epoch = boost::posix_time::second_clock::universal_time();
	//boost::date_time::second_clock<boost::posix_time::ptime>::local_time();	// static access should be here??

	boost::posix_time::time_duration epoch_seconds_duration =
												(current_epoch - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)));
	int64_t epoch_seconds = epoch_seconds_duration.total_seconds();

	temp << "INSERT INTO `" << this->dbName << "`.`data_diff`";
	temp << "(`epoch`, `datetime`, `station`, `temperaturefrom`, `temperaturesubtract`, `windfrom`, `windsubtract`, `temperature`, `winddir`, `windspeed`, " <<
								"`windgusts`) VALUES (";

	temp << epoch_seconds << ", ";
	temp << "CURRENT_TIMESTAMP, ";
	temp << "'" << station_name << "', ";
	temp << "'" << ::wxDataSourceToStr(diffCalculator.temperatureFrom) << "', ";
	temp << "'" << ::wxDataSourceToStr(diffCalculator.temperatureSubstract) << "', ";
	temp << "'" << ::wxDataSourceToStr(diffCalculator.windFrom) << "', ";
	temp << "'" << ::wxDataSourceToStr(diffCalculator.windSubstract) << "', ";
	temp << input.temperature << ", ";
	temp << input.wind_direction << ", ";
	temp << input.wind_speed << ", ";
	temp << input.wind_gusts << ");" << endl;

	if (this->Debug == true)
		cout << temp.str() << endl;

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
		cout << "--- MysqlConnInterface::InsertDiff:252 - unknown exception" << endl;
	}

	cout << this->dbSimpleResult.info();

}

void MySqlConnInterface::InsertTelmetry(const Telemetry& input, std::string station_name) {

	if (!this->dumpTelemetry || !input.valid)
		return;

	if (!this->dbConnection.connected()) {
		this->OpenDBConnection();
	}

	std::stringstream temp;
	temp.str("");

	boost::posix_time::ptime current_epoch = boost::posix_time::second_clock::universal_time();
	//boost::date_time::second_clock<boost::posix_time::ptime>::local_time();	// static access should be here??

	boost::posix_time::time_duration epoch_seconds_duration =
												(current_epoch - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)));
	int64_t epoch_seconds = epoch_seconds_duration.total_seconds();

	temp << "INSERT INTO `" << this->dbName << "`.`data_telemetry`";
	temp << "(`epoch`, `datetime`, `station`, `callsign`, `number`, `first`, `second`, `third`, `fourth`, `fifth`, " <<
								"`digital`, `type`, `rawfirst`, `rawsecond`, `rawthird`, `rawfourth`, `rawfifth`) VALUES (";

	temp << epoch_seconds << ", ";
	temp << "CURRENT_TIMESTAMP, ";
	temp << "'" << station_name << "', ";
	temp << "'" << input.call << "', ";
	temp << (int)input.num << ", ";
	temp << input.getCh1() << ", ";
	temp << input.getCh2() << ", ";
	temp << input.getCh3() << ", ";
	temp << input.getCh4() << ", ";
	temp << input.getCh5() << ", ";
	temp << (int)input.digital  << ", ";
	temp << "10, ";
	temp << (int)input.ch1 << ", ";
	temp << (int)input.ch2 << ", ";
	temp << (int)input.ch3 << ", ";
	temp << (int)input.ch4 << ", ";
	temp << (int)input.ch5 << ");" << endl;

	if (this->Debug == true)
		cout << temp.str() << endl;

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
		cout << "--- MysqlConnInterface::InsertTelmetry:320 - unknown exception" << endl;
	}

	cout << this->dbSimpleResult.info();

	cout << "--- MysqlConnInterface::InsertTelmetry:325 - Data inserted successfully" << endl;

	if (this->dbConnection.connected()) {
		this->CloseDBConnection();
	}

}

void MySqlConnInterface::InsertIntoDbSchemaTatry(AprsPacket& packet, uint64_t timestamp, float temperature, std::string station_name) {

	std::string source = "HISTORIC";

	std::stringstream temp;
	temp.str("");

	cout << "--- MysqlConnInterface::InsertIntoDbSchemaTatry:340 - historical data, timestamp: " << timestamp << ", temperature: " << temperature << endl;

	temp << "INSERT INTO `" << this->dbName << "`.`data_tatry`";
	temp << "(`epoch`, `station`, `wxtemperature`, `rawmeasurement`, `rawmeasurementrecalc`, " <<
			"`voltage`, `maxstatus`, `lserdy`, `rtcen`, `maxok`, `sleep`, `spier`, `spiok`, " <<
			"`source`, `frame`, `originatorcall`, `originatorssid`, `insertepoch`) VALUES (";

	temp << timestamp << ", ";
	temp << "'" << station_name << "', ";
	temp << temperature << ", ";
	temp << 0 << ", ";
	temp << temperature << ", ";
	temp << 0 << ", ";
	temp << 0 << ", ";
	temp << (false ? "TRUE" : "FALSE") << ", ";
	temp << (false ? "TRUE" : "FALSE") << ", ";
	temp << (false ? "TRUE" : "FALSE") << ", ";
	temp << (false ? "TRUE" : "FALSE") << ", ";
	temp << (false ? "TRUE" : "FALSE") << ", ";
	temp << (false ? "TRUE" : "FALSE") << ", ";
	temp << "'" << source << "', ";
	temp << "'" << packet.DataAsStr << "', ";
	temp << "'" << packet.ToISOriginator.Call << "', ";
	temp << (int)packet.ToISOriginator.SSID << ", ";
	temp << TimeTools::getEpoch() << " ";
	temp << ");" << endl;	if (this->Debug == true)
		cout << temp.str() << endl;

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
		cout << "--- MysqlConnInterface::InsertIntoDbSchemaTatry:370 - unknown exception" << endl;
	}

	cout << this->dbSimpleResult.info();
}

void MySqlConnInterface::InsertIntoDbSchemaTatry(AprsPacket& packet, const AprsWXData &wx,
		const Telemetry &input, std::string station_name, bool wxParsed, bool telemetryParsed) {

	if (!this->schema_v2)
		return;

	std::stringstream temp;
	temp.str("");

	std::string source;

	if (wxParsed) {
		source = "WX";
	}
	else if (telemetryParsed) {
		source = "TELEMETRY";
	}
	else {
		source = "?";
	}

	float temperatureFromRawMeasurement;

	if (telemetryParsed) {
		temperatureFromRawMeasurement = input.getTemperatureFromRawMeasurement();
	}
	else {
		temperatureFromRawMeasurement = wx.temperature;
	}

	boost::posix_time::ptime current_epoch = boost::posix_time::second_clock::universal_time();
	//boost::date_time::second_clock<boost::posix_time::ptime>::local_time();	// static access should be here??

	boost::posix_time::time_duration epoch_seconds_duration =
												(current_epoch - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)));
	int64_t epoch_seconds = epoch_seconds_duration.total_seconds();

	cout << "--- MysqlConnInterface::InsertIntoDbSchemaTatry:392 - epoch_seconds: " << epoch_seconds << endl;

	temp << "INSERT INTO `" << this->dbName << "`.`data_tatry`";
	temp << "(`epoch`, `station`, `wxtemperature`, `rawmeasurement`, `rawmeasurementrecalc`, " <<
			"`voltage`, `maxstatus`, `lserdy`, `rtcen`, `maxok`, `sleep`, `spier`, `spiok`, " <<
			"`source`, `frame`, `originatorcall`, `originatorssid`, `insertepoch`) VALUES (";

	temp << epoch_seconds << ", ";
	temp << "'" << station_name << "', ";
	temp << wx.temperature << ", ";
	temp << (int)input.getRawMeasurement() << ", ";
	temp << temperatureFromRawMeasurement << ", ";
	temp << input.getBatteryVoltage() << ", ";
	temp << input.getCh5Raw() << ", ";
	temp << (input.getLSERDY() ? "TRUE" : "FALSE") << ", ";
	temp << (input.getRTCEN() ? "TRUE" : "FALSE") << ", ";
	temp << (input.getMAXOK() ? "TRUE" : "FALSE") << ", ";
	temp << (input.getSLEEP() ? "TRUE" : "FALSE") << ", ";
	temp << (input.getSPIER() ? "TRUE" : "FALSE") << ", ";
	temp << (input.getSPIOK() ? "TRUE" : "FALSE") << ", ";
	temp << "'" << source << "', ";
	temp << "'" << packet.DataAsStr << "', ";
	temp << "'" << packet.ToISOriginator.Call << "', ";
	temp << (int)packet.ToISOriginator.SSID << ", ";
	temp << TimeTools::getEpoch() << " ";
	temp << ");" << endl;

	if (this->Debug == true)
		cout << temp.str() << endl;

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
		cout << "--- MysqlConnInterface::InsertIntoDbSchemaTatry:424 - unknown exception" << endl;
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
		cout << "--- MysqlConnInterface::Keepalive:445 - Unknown exception has been thrown" << endl;
	}

}

void MySqlConnInterface::InsertIntoDb(const AprsWXData* cInput) {

	if (!this->schema_v1)
		return;

	const bool hasTimestamp = (cInput->packetUtcTimestamp != 0);

	stringstream temp;
	time_t currtime;
	struct tm* local;

	temp.str("");



	if (this->execBeforeInsert == true) {
		cout << "--- MysqlConnInterface::InsertIntoDb:466 - Executing: " << this->execBeforeInsertPath.c_str();
		(void)system(this->execBeforeInsertPath.c_str());
		cout << endl;
	}

	if (hasTimestamp) {
		// get tm struct from datetime parsed from APRX rf-log record 
		struct tm time = TimeTools::getTmStructFromUndefinedTzPtime(cInput->packetLocalTimestmp);

		temp << "INSERT INTO `" << this->dbName << "`.`" << this->tableName << "` (`Id`, `TimestampEpoch`, `TimestampDate`, `Temp`, `WindSpeed`, `WindGusts`, `WindDir`, `QNH`, `Humidity`) VALUES (NULL, " <<
			"'" << \
			time.tm_year + 1900 << "-" << setw(2) <<  setfill('0') << \
			(time.tm_mon) + 1 << "-" << setw(2) << setfill('0') << \
			time.tm_mday << " " << setw(2) << setfill('0') << \
			time.tm_hour << ":" << setw(2) << setfill('0') << \
			time.tm_min << ":" << setw(2) << setfill('0') << \
			time.tm_sec << 
			"', '" << \
			time.tm_year + 1900 << "-" << setw(2) <<  setfill('0') << \
			(time.tm_mon) + 1 << "-" << setw(2) << setfill('0') << \
			time.tm_mday << " " << setw(2) << setfill('0') << \
			time.tm_hour << ":" << setw(2) << setfill('0') << \
			time.tm_min << ":" << setw(2) << setfill('0') << \
			time.tm_sec << 
			"', " << \
			cInput->temperature << ", " << \
			cInput->wind_speed << ", " <<  \
			cInput->wind_gusts << ", " << \
			cInput->wind_direction << ", " << \
			cInput->pressure << ", " << \
			cInput->humidity << ");" << endl;

	}
	else {
		currtime = time(NULL);
		local = localtime(&currtime);

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
	}

	if (this->Debug == true)
		cout << temp.str() << endl;

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
		cout << "--- MysqlConnInterface::InsertIntoDb:535 - unknown exception" << endl;
	}

	cout << this->dbSimpleResult.info();
	//delete this->dbQuery;

	this->dbQueryCounter++;

}
