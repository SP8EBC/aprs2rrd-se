/*
 * DataSourceConfig.h
 *
 *  Created on: 28.04.2020
 *      Author: mateusz
 */

#include <string>

#ifndef DATASOURCECONFIG_H_
#define DATASOURCECONFIG_H_

enum class WxDataSource {
	IS_PRIMARY,
	IS_SECONDARY,
	SERIAL,
	TELEMETRY,
	HOLFUY,
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
	else
		return "";
}

class DataSourceConfig {
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

	DataSourceConfig();
	virtual ~DataSourceConfig();
};

#endif /* DATASOURCECONFIG_H_ */
