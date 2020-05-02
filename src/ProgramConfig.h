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

#include "MySqlConnInterface.h"
#include "AprsThreadConfig.h"
#include "DataPresentation.h"
#include "SerialConfig.h"
#include "Telemetry.h"
#include "DataSourceConfig.h"
#include "HolfuyClientConfig.h"


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

	void configureLogOutput();

	bool getDebug();
	bool getDebugToFile();
	std::string getDebugLogFn();

	static void printConfigInPl(	MySqlConnInterface& mysqlDb,
									AprsThreadConfig& aprsConfig,
									DataPresentation& dataPresence,
									int& rrdCount,
									int& plotCount,
									Telemetry& telemetry,
									bool& useAsTemperature);



};

#endif /* PROGRAMCONFIG_H_ */
