/*
 * ProgramConfig.h
 *
 *  Created on: 04.12.2018
 *      Author: mateusz
 */

#ifndef PROGRAMCONFIG_H_
#define PROGRAMCONFIG_H_

#include <string>
#include <libconfig.h++>

#include <boost/algorithm/string.hpp>

#include "MySqlConnInterface.h"
#include "AprsThreadConfig.h"
#include "DataPresentation.h"
#include "SerialConfig.h"
#include "Telemetry.h"
#include "DataSourceConfig.h"
#include "HolfuyClientConfig.h"
#include "DiffCalculator.h"


class ProgramConfig {
	std::string configFilename;
	libconfig::Config config;

	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	bool Debug = false;
	bool DebugToFile = false;
	std::string DebugLogFn = "";

	constexpr static int swstring(const char* _in) {
		int out = 0;

		for (int i = 0; *(_in + i) != '\0'; i++) {
			out += (int)*(_in + i);
		}

		return out;
	}

	WxDataSource getTemperatureSource();
	WxDataSource getPressureSource();
	WxDataSource getWindSource();
	WxDataSource getRainSource();
	WxDataSource getHumiditySource();
	WxDataSource getGlobalBackup();



public:
	ProgramConfig(std::string fn);
	virtual ~ProgramConfig();

	void parseFile();
	void getDbConfig(MySqlConnInterface& db);
	void getAprsThreadConfig(AprsThreadConfig& aprs);
	void getSerialConfig(SerialConfig& serial);
	void getDataPresentationConfig(DataPresentation& data, int& rrdCount, int& plotCount);
	void getTelemetryConfig(Telemetry& data, bool& useAsTemperature);
	void getDataSourceConfig(DataSourceConfig & config);
	void getHolfuyConfig(HolfuyClientConfig & config);
	void getDiffConfiguration(DiffCalculator & calculator);

	void configureLogOutput();

	bool getDebug();
	bool getDebugToFile();
	std::string getDebugLogFn();

	static WxDataSource wxDataSourceFromStr(std::string in) {
		boost::algorithm::to_upper(in);
		switch(swstring(in.c_str())) {
		case swstring("APRSIS"): return WxDataSource::IS_PRIMARY;
		case swstring("SECONDARYAPRSIS"): return WxDataSource::IS_SECONDARY;
		case swstring("TELEMETRY"): return WxDataSource::TELEMETRY;
		case swstring("SERIAL"): return WxDataSource::SERIAL;
		case swstring("HOLFUY"): return WxDataSource::HOLFUY;
		}

		return WxDataSource::UNKNOWN;
	}

	static void printConfigInPl(	MySqlConnInterface& mysqlDb,
									AprsThreadConfig& aprsConfig,
									DataPresentation& dataPresence,
									int& rrdCount,
									int& plotCount,
									Telemetry& telemetry,
									bool& useAsTemperature,
									HolfuyClientConfig& holfuy,
									DiffCalculator & calculator);



};

#endif /* PROGRAMCONFIG_H_ */
