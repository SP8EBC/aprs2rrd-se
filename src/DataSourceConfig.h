/*
 * DataSourceConfig.h
 *
 *  Created on: 28.04.2020
 *      Author: mateusz
 */

#include "ZywiecMeteoConfig.h"
#include "HolfuyClientConfig.h"
#include "WxDataSource.h"

#include <string>


#ifndef DATASOURCECONFIG_H_
#define DATASOURCECONFIG_H_



/**
 * This class represents a configuration which controls which weather parameter (like
 * temperature) is get from which data source (like APRS-IS, serial port, weatherlink).
 * Fields within this class are set by configuration file parser
 */
class DataSourceConfig {

private:
	std::string get(const WxDataSource& source) const;

public:
	WxDataSource temperature;
	WxDataSource pressure;
	WxDataSource wind;
	WxDataSource rain;
	WxDataSource humidity;
	WxDataSource globalBackup;

	std::string primaryCall;
	unsigned primarySsid;

	std::string secondaryCall;
	unsigned secondarySsid;

	std::string serialCall;
	unsigned serialSsid;
	bool serialCaptureAll;

	unsigned holfuyNumber;
	unsigned zywiecNumber;
	std::string davisWeatherlinkNumber;

	/**
	 * Methods used to generate string which represents data source in MySQL database. For data
	 * coming from APRS this will be a callsign.
	 */
	std::string getTemperatureSource() const;
	std::string getPressureSource() const;
	std::string getWindSource() const;
	std::string getRainSource() const;
	std::string getHumiditySource() const;

	DataSourceConfig();
	virtual ~DataSourceConfig();
};

#endif /* DATASOURCECONFIG_H_ */
