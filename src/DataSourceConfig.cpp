/*
 * DataSourceConfig.cpp
 *
 *  Created on: 28.04.2020
 *      Author: mateusz
 */

#include "DataSourceConfig.h"

#include <boost/lexical_cast.hpp>



DataSourceConfig::DataSourceConfig() {
	this->wind = WxDataSource::UNKNOWN;
	this->humidity = WxDataSource::UNKNOWN;
	this->pressure = WxDataSource::UNKNOWN;
	this->rain = WxDataSource::UNKNOWN;
	this->globalBackup = WxDataSource::UNKNOWN;
	this->temperature = WxDataSource::UNKNOWN;

	this->primaryCall = "";
	this->primarySsid = 0;
	this->secondaryCall = "";
	this->secondarySsid = 0;

	this->serialCall = "";
	this->serialSsid = 0;
	this->serialCaptureAll = false;

	this->holfuyNumber = 0;

}

std::string DataSourceConfig::getTemperatureSource() const {

	return this->get(this->temperature);

}

std::string DataSourceConfig::getPressureSource() const {

	return this->get(this->pressure);

}

std::string DataSourceConfig::getWindSource() const {

	return this->get(this->wind);

}

std::string DataSourceConfig::getRainSource() const {

	return this->get(this->rain);

}

std::string DataSourceConfig::getHumiditySource() const {

	return this->get(this->humidity);

}

std::string DataSourceConfig::get(const WxDataSource& source) const {

	std::string out = "";

	std::string ssid;

	switch (source) {
	case WxDataSource::HOLFUY: out = boost::lexical_cast<std::string>("H" + this->holfuyNumber); break;
	case WxDataSource::IS_PRIMARY: {
		ssid = boost::lexical_cast<std::string>(this->primarySsid);
		out = boost::lexical_cast<std::string>(this->primaryCall + "-" + ssid); break;
	}
	case WxDataSource::IS_SECONDARY: {
		ssid = boost::lexical_cast<std::string>(this->secondarySsid);
		out = boost::lexical_cast<std::string>(this->secondaryCall + "-" + ssid); break;

		break;
	}
	case WxDataSource::SERIAL: {
		if (this->serialCaptureAll) {
			out = "SERIAL";
		}
		else {
			ssid = boost::lexical_cast<std::string>(this->secondarySsid);
			out = boost::lexical_cast<std::string>(this->secondaryCall + "-" + ssid); break;
		}
		break;
	}
	case WxDataSource::TELEMETRY: {
		ssid = boost::lexical_cast<std::string>(this->primarySsid);
		out = boost::lexical_cast<std::string>("T_" + this->primaryCall + "-" + ssid); break;
	}
	case WxDataSource::UNKNOWN: break;
	}

	return out;
}

DataSourceConfig::~DataSourceConfig() {
	// TODO Auto-generated destructor stub
}

