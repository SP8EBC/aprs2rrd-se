/*
 * WxDataSource.h
 *
 *  Created on: Mar 25, 2023
 *      Author: mateusz
 */

#ifndef WXDATASOURCE_H_
#define WXDATASOURCE_H_


/**
 *
 */
enum class WxDataSource {
	IS_PRIMARY,
	IS_SECONDARY,
	SERIAL,
	TELEMETRY,
	HOLFUY,
	ZYWIEC,
	DAVIS,
	THINGSPEAK,
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
	else if (in == WxDataSource::THINGSPEAK)
		return "THINGSPEAK";
	else
		return "";
}


#endif /* WXDATASOURCE_H_ */
