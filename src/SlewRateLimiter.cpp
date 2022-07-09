/*
 * SlewRateLimiter.cpp
 *
 *  Created on: 27.11.2018
 *      Author: mateusz
 */

#include "SlewRateLimiter.h"
#include <cmath>
#include <iostream>

SlewRateLimiter::SlewRateLimiter() {
	//
	maxTempSlew = MAX_TEMP_SLEW;
	maxPressureSlew = MAX_PRESSURE_SLEW;
	maxSpeedSleew = MAX_SPEED_SLEW;
	maxDirectionSleew = MAX_DIRECTION_SLEW;
	maxGustsSleew = MAX_GUSTS_SLEW;
	maxHumiditySlew = MAX_HUMIDITY_SLEW;

	changedFromDefault = false;

	hmZeroAs100 = false;

	callCounter = 0;
}

SlewRateLimiter::~SlewRateLimiter() {
	//
}

void SlewRateLimiter::limitFromSingleFrame(const AprsWXData& previous,
		AprsWXData& current) {

	bool humidityUpdated = false;

	this->callCounter++;

	if (hmZeroAs100 && current.humidity == 0) {
		std::cout << "SlewRateLimiter::limitFromSingleFrame:38 - " << std::endl;

		current.humidity = 100;
		humidityUpdated = true;
	}

	if (this->callCounter < 8) {
		return;
	}

	// calculating differences
	float windSpdDiff = current.wind_speed - previous.wind_speed;
	float windGstDiff = current.wind_gusts - previous.wind_gusts;
	float temperatureDiff = current.temperature - previous.temperature;
	float pressureDiff = current.pressure - previous.pressure;
	int16_t humidityDiff = current.humidity - previous.humidity;

	if (previous.wind_direction == 0 &&
			previous.wind_gusts == 0.0f &&
			previous.wind_speed == 0.0f &&
			previous.pressure == 0 &&
			previous.temperature == 0.0f)
	{
		// if previous values of measurements are only zeros assume that this is
		// first call after program startup
		return;
	}

	if (current.useWind && abs(windSpdDiff) > maxSpeedSleew)
	{
		std::cout << "SlewRateLimiter::limitFromSingleFrame:71 - Limiting windspeed" << std::endl;

		// if the wind speed changed above the maximum speed limit apply a correction
		if (windSpdDiff < 0)
			// if difference is negative it means that previous value of wind speed is greater than current
			// so a trend is negative (value is falling)
			current.wind_speed = previous.wind_speed - maxSpeedSleew;
		else
			current.wind_speed = previous.wind_speed + maxSpeedSleew;
	}

	// simmilar thing for gusts
	if (current.useWind && abs(windGstDiff) > maxGustsSleew) {
		std::cout << "SlewRateLimiter::limitFromSingleFrame:84 - Limiting wind gusts" << std::endl;

		// if the wind speed changed above the maximum speed limit apply a correction
		if (windGstDiff < 0)
			// if difference is negative it means that previous value of wind speed is greater than current
			// so a trend is negative (value is falling)
			current.wind_gusts = previous.wind_gusts - maxGustsSleew;
		else
			current.wind_gusts = previous.wind_gusts + maxGustsSleew;
	}

	// simmilar thing for gusts
	if (current.useTemperature && abs(temperatureDiff) > maxTempSlew) {
		std::cout << "SlewRateLimiter::limitFromSingleFrame:97 - Limiting temperature" << std::endl;

		// if the wind speed changed above the maximum speed limit apply a correction
		if (temperatureDiff < 0)
			// if difference is negative it means that previous value of wind speed is greater than current
			// so a trend is negative (value is falling)
			current.temperature = previous.temperature - maxTempSlew;
		else
			current.temperature = previous.temperature + maxTempSlew;
	}

	if (current.usePressure && abs(pressureDiff) > maxPressureSlew) {
		std::cout << "SlewRateLimiter::limitFromSingleFrame:109 - Limiting pressure" << std::endl;

		if (pressureDiff < 0)
			current.pressure = previous.pressure - maxPressureSlew;
		else
			current.pressure = previous.pressure + maxPressureSlew;

	}

	if (!humidityUpdated && current.useHumidity && abs(humidityDiff) > maxHumiditySlew) {
		std::cout << "SlewRateLimiter::limitFromSingleFrame:119 - Limiting humidity" << std::endl;

		if (humidityDiff < 0)
			current.humidity = previous.humidity - maxHumiditySlew;
		else
			current.humidity = previous.humidity + maxHumiditySlew;
	}

}
