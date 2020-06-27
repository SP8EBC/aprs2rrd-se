/*
 * SlewRateLimiter.cpp
 *
 *  Created on: 27.11.2018
 *      Author: mateusz
 */

#include "SlewRateLimiter.h"
#include <cmath>

SlewRateLimiter::SlewRateLimiter() {
	// TODO Auto-generated constructor stub
	maxTempSlew = MAX_TEMP_SLEW;
	maxPressureSlew = MAX_PRESSURE_SLEW;
	maxSpeedSleew = MAX_SPEED_SLEW;
	maxDirectionSleew = MAX_DIRECTION_SLEW;
	maxGustsSleew = MAX_GUSTS_SLEW;

	callCounter = 0;
}

SlewRateLimiter::~SlewRateLimiter() {
	// TODO Auto-generated destructor stub
}

void SlewRateLimiter::limitFromSingleFrame(const AprsWXData& previous,
		AprsWXData& current) {

	this->callCounter++;

	if (this->callCounter < 8)
		return;

	// calculating differences
	float windDirDiff = current.wind_direction - previous.wind_direction;
	float windSpdDiff = current.wind_speed - previous.wind_speed;
	float windGstDiff = current.wind_gusts - previous.wind_gusts;
	float temperatureDiff = current.temperature - previous.temperature;
	float pressureDiff = current.pressure - previous.pressure;

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
		// if the wind speed changed above the maximum speed limit apply a correction
		if (temperatureDiff < 0)
			// if difference is negative it means that previous value of wind speed is greater than current
			// so a trend is negative (value is falling)
			current.temperature = previous.temperature - maxTempSlew;
		else
			current.temperature = previous.temperature + maxTempSlew;
	}

}
