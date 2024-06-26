/*
 * ProgramConfig.h
 *
 *  Created on: 04.12.2018
 *      Author: mateusz
 */

#ifndef PROGRAMCONFIG_H_
#define PROGRAMCONFIG_H_

#include <string>
#include <functional>
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
#include "PressureCalculator.h"
#include "SlewRateLimiter.h"
#include "Locale.h"
#include "ZywiecMeteoConfig.h"
#include "WeatherlinkClient.h"
#include "BannerCreatorConfig.h"
#include "AprxLogParserConfig.h"
#include "Thingspeak.h"

class ProgramConfig {
	std::string configFilename;
	libconfig::Config config;

	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	bool Debug = false;
	bool DebugToFile = false;
	std::string DebugLogFn = "";

	std::string stationName;

	std::function<const DataPresentationParametersPrint(std::string&&)> parseParameterPrinting;

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
	void getDataSourceConfig(DataSourceConfig & config);
	void getHolfuyConfig(HolfuyClientConfig & config);
	void getDiffConfiguration(DiffCalculator & calculator);
	void getPressureCalcConfig(PressureCalculator & pressureCalc);
	void getSlewRateLimitConfig(SlewRateLimiter & limiter);
	void getLocaleStaticString(Locale& l);
	void getZywiecMeteoConfig(ZywiecMeteoConfig & z);
	void getWeatherlinkConfig(WeatherlinkClient_Config & _config);
	void getBannerConfig(BannerCreatorConfig & bannerCreator);
	void getAprxLogParserConfig(AprxLogParserConfig & aprxLogConfig);
	bool getDateTimeLocale(char * localeString, basic_string<char>::size_type ln);
	void getThingspeakConfig(Thingspeak_Config & config);

	bool configureLogOutput();

	bool getDebug();
	bool getDebugToFile();
	std::string getDebugLogFn();
	bool getBatchMode();
	bool getExitOnException();


	std::string getStationName();

	static WxDataSource wxDataSourceFromStr(std::string in) {
		boost::algorithm::to_upper(in);
		switch(swstring(in.c_str())) {
			case swstring("APRSIS"): return WxDataSource::IS_PRIMARY;
			case swstring("SECONDARYAPRSIS"): return WxDataSource::IS_SECONDARY;
			case swstring("TELEMETRY"): return WxDataSource::TELEMETRY;
			case swstring("SERIAL"): return WxDataSource::SERIAL;
			case swstring("HOLFUY"): return WxDataSource::HOLFUY;
			case swstring("ZYWIEC"): return WxDataSource::ZYWIEC;
			case swstring("THINGSPEAK"): return WxDataSource::THINGSPEAK;
			case swstring("DAVIS"): return WxDataSource::DAVIS;
		}

		return WxDataSource::UNKNOWN;
	}

	static void printConfigInPl(	MySqlConnInterface& mysqlDb,
									AprsThreadConfig& aprsConfig,
									DataPresentation& dataPresence,
									int& rrdCount,
									int& plotCount,
									ZywiecMeteoConfig & zywiec,
									HolfuyClientConfig& holfuy,
									DiffCalculator & calculator,
									DataSourceConfig & source,
									PressureCalculator& pressureCalc,
									SlewRateLimiter & limiter,
									Locale & locale,
									WeatherlinkClient_Config &_config,
									BannerCreatorConfig &bannerCreator,
									Thingspeak_Config &thingspeak);



};

#endif /* PROGRAMCONFIG_H_ */
