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
