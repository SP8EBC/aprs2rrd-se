/*
 * DiffCalculator.cpp
 *
 *  Created on: 05.05.2020
 *      Author: mateusz
 */

#include "DiffCalculator.h"

#include <memory>
#include <iostream>

DiffCalculator::DiffCalculator() {
	// TODO Auto-generated constructor stub
	enable = false;
	temperatureFrom = WxDataSource::UNKNOWN;
	temperatureSubstract = WxDataSource::UNKNOWN;
	windFrom = WxDataSource::UNKNOWN;
	windSubstract = WxDataSource::UNKNOWN;

}

void DiffCalculator::calculate(const AprsWXData& aprsIS,
		const AprsWXData& serial,
		const AprsWXData& holfuy,
		const AprsWXData& zywiec,
		const AprsWXData& weatherlink,
		const Telemetry& telemetry,
		AprsWXData& out)
{
	if (!this->enable)
		return;

	std::cout << "--- DiffCalculator::calculate:31 - preparing the differential calculation" << std::endl;

	std::unique_ptr<AprsWXData> temporary = std::make_unique<AprsWXData>();
	std::unique_ptr<AprsWXData> temporary2 = std::make_unique<AprsWXData>();

	switch (this->temperatureFrom) {
		case WxDataSource::IS_PRIMARY: {
			if (aprsIS.is_primary) {
				temporary->temperature = aprsIS.temperature;
				temporary->useTemperature = true;
			}
			else {
				;
			}
			break;
		}
		case WxDataSource::IS_SECONDARY: {
			if (aprsIS.is_secondary) {
				temporary->temperature = aprsIS.temperature;
				temporary->useTemperature = true;
			}
			else {
				;
			}
			break;
		}
		case WxDataSource::SERIAL: temporary->temperature = serial.temperature; temporary->useTemperature = true; break;
		case WxDataSource::TELEMETRY: temporary->temperature = telemetry.getCh5(); temporary->useTemperature = true; break;
		case WxDataSource::HOLFUY: temporary->temperature = holfuy.temperature; temporary->useTemperature = true; break;
		case WxDataSource::ZYWIEC: temporary->temperature = zywiec.temperature; temporary->useTemperature = true; break;
		case WxDataSource::DAVIS: temporary->temperature = weatherlink.temperature; temporary->useTemperature = true; break;
		case WxDataSource::UNKNOWN: return;
	}

	switch (this->temperatureSubstract) {
		case WxDataSource::IS_PRIMARY: {
			if (aprsIS.is_primary) {
				temporary2->temperature = aprsIS.temperature;
				temporary2->useTemperature = true;
			}
			else {
				;
			}
			break;
		}
		case WxDataSource::IS_SECONDARY: {
			if (aprsIS.is_secondary) {
				temporary2->temperature = aprsIS.temperature;
				temporary2->useTemperature = true;
			}
			else {
				;
			}
			break;
		}
		case WxDataSource::SERIAL: temporary2->temperature = serial.temperature; temporary2->useTemperature = true; break;
		case WxDataSource::TELEMETRY: temporary2->temperature = telemetry.getCh5(); temporary2->useTemperature = true; break;
		case WxDataSource::HOLFUY: temporary2->temperature = holfuy.temperature; temporary2->useTemperature = true; break;
		case WxDataSource::ZYWIEC: temporary2->temperature = zywiec.temperature; temporary2->useTemperature = true; break;
		case WxDataSource::DAVIS: temporary2->temperature = weatherlink.temperature; temporary2->useTemperature = true; break;
		case WxDataSource::UNKNOWN: return;
	}


	switch (this->windFrom) {
		case WxDataSource::IS_PRIMARY: {
			if (aprsIS.is_primary) {
				temporary->wind_direction = aprsIS.wind_direction;
				temporary->wind_gusts = aprsIS.wind_gusts;
				temporary->wind_speed = aprsIS.wind_speed;
				temporary->useWind = true;
				break;
			}
			else {
				;
			}
			break;
		}
		case WxDataSource::IS_SECONDARY: {
			if (aprsIS.is_secondary) {
				temporary->wind_direction = aprsIS.wind_direction;
				temporary->wind_gusts = aprsIS.wind_gusts;
				temporary->wind_speed = aprsIS.wind_speed;
				temporary->useWind = true;
				break;
			}
			else {
				;
			}
			break;
		}
		case WxDataSource::SERIAL: {
			temporary->wind_direction = serial.wind_direction;
			temporary->wind_gusts = serial.wind_gusts;
			temporary->wind_speed = serial.wind_speed;
			break;
		}
		case WxDataSource::TELEMETRY: return;
		case WxDataSource::HOLFUY: {
			temporary->wind_direction = holfuy.wind_direction;
			temporary->wind_gusts = holfuy.wind_gusts;
			temporary->wind_speed = holfuy.wind_speed;
			break;
		}
		case WxDataSource::ZYWIEC: {
			temporary->wind_direction = zywiec.wind_direction;
			temporary->wind_gusts = zywiec.wind_gusts;
			temporary->wind_speed = zywiec.wind_speed;
			break;

		}
		case WxDataSource::DAVIS: {
			temporary->wind_direction = weatherlink.wind_direction;
			temporary->wind_gusts = weatherlink.wind_gusts;
			temporary->wind_speed = weatherlink.wind_speed;
			break;
		}
		case WxDataSource::UNKNOWN: return;
	}

	switch (this->windSubstract) {
	case WxDataSource::IS_PRIMARY: {
		if (aprsIS.is_primary) {
			temporary2->wind_direction = aprsIS.wind_direction;
			temporary2->wind_gusts = aprsIS.wind_gusts;
			temporary2->wind_speed = aprsIS.wind_speed;
			temporary2->useWind = true;
			break;
		}
		else {
			;
		}
		break;
	}
	case WxDataSource::IS_SECONDARY: {
		if (aprsIS.is_secondary) {
			temporary2->wind_direction = aprsIS.wind_direction;
			temporary2->wind_gusts = aprsIS.wind_gusts;
			temporary2->wind_speed = aprsIS.wind_speed;
			temporary2->useWind = true;
			break;
		}
		else {
			;
		}
		break;
	}
	case WxDataSource::SERIAL: {
		temporary2->wind_direction = serial.wind_direction;
		temporary2->wind_gusts = serial.wind_gusts;
		temporary2->wind_speed = serial.wind_speed;
		temporary2->useWind = true;
		break;
	}
	case WxDataSource::TELEMETRY: return;
	case WxDataSource::HOLFUY: {
		temporary2->wind_direction = holfuy.wind_direction;
		temporary2->wind_gusts = holfuy.wind_gusts;
		temporary2->wind_speed = holfuy.wind_speed;
		temporary2->useWind = true;
		break;
	}
	case WxDataSource::ZYWIEC: {
		temporary->wind_direction = zywiec.wind_direction;
		temporary->wind_gusts = zywiec.wind_gusts;
		temporary->wind_speed = zywiec.wind_speed;
		break;

	}
	case WxDataSource::DAVIS: {
		temporary2->wind_direction = weatherlink.wind_direction;
		temporary2->wind_gusts = weatherlink.wind_gusts;
		temporary2->wind_speed = weatherlink.wind_speed;
		break;
	}
	case WxDataSource::UNKNOWN: return;
	}

	out = *temporary - *temporary2;

	if (temporary->useWind && temporary2->useWind)
		out.useWind = true;

	if (temporary->useTemperature && temporary2->useTemperature)
		out.useTemperature = true;

	out.useHumidity = false;
	out.usePressure = false;

}

DiffCalculator::~DiffCalculator() {
}

