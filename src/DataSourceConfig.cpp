/*
 * DataSourceConfig.cpp
 *
 *  Created on: 28.04.2020
 *      Author: mateusz
 */

#include "DataSourceConfig.h"



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

}

DataSourceConfig::~DataSourceConfig() {
	// TODO Auto-generated destructor stub
}

