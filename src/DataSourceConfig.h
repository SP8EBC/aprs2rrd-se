/*
 * DataSourceConfig.h
 *
 *  Created on: 28.04.2020
 *      Author: mateusz
 */

#include "ZywiecMeteoConfig.h"
#include "HolfuyClientConfig.h"

#include <string>


#ifndef DATASOURCECONFIG_H_
#define DATASOURCECONFIG_H_

enum class WxDataSource {
	IS_PRIMARY,
	IS_SECONDARY,
	SERIAL,
	TELEMETRY,
	HOLFUY,
	ZYWIEC,
	DAVIS,
	UNKNOWN
};

constexpr const char* wxDataSourceToStr(WxDataSource in) {
	if (in == WxDataSource::IS_PRIMARY)
		return "IS_PRIMARY";
	else if (in == WxDataSource::IS_SECONDARY)
		return "IS_SECONDARY";
	else if (in == WxDataSource::HOLFUY)
		return "HOLFUY";
	else if (in == WxDataSource::SERIAL)
		return "SERIAL";
	else if (in == WxDataSource::TELEMETRY)
		return "TELEMETRY";
	else if (in == WxDataSource::ZYWIEC)
		return "ZYWIEC";
	else if (in == WxDataSource::DAVIS)
		return "DAVIS";
	else
		return "";
}

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

	std::string getTemperatureSource() const;
	std::string getPressureSource() const;
	std::string getWindSource() const;
	std::string getRainSource() const;
	std::string getHumiditySource() const;

	DataSourceConfig();
	virtual ~DataSourceConfig();
};

#endif /* DATASOURCECONFIG_H_ */
