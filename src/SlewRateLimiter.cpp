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

}

SlewRateLimiter::~SlewRateLimiter() {
	// TODO Auto-generated destructor stub
}

void SlewRateLimiter::limitFromSingleFrame(const AprsWXData& previous,
		AprsWXData& current) {

	// calculating differences
	float windDirDiff = current.wind_direction - previous.wind_direction;
	float windSpdDiff = current.wind_speed - previous.wind_speed;
	float windGstDiff = current.wind_gusts - previous.wind_gusts;
	float temperatureDiff = current.temperature - previous.temperature;
	float pressureDiff = current.pressure - previous.pressure;

	if (abs(windSpdDiff) > maxSpeedSleew)
	{
		// if the wind speed changed above the maximum speed limit apply a correction
		if (windSpdDiff < 0)
			// if difference is negative it means that previous value of wind speed is greater than current
			// so a trend is negative (value is falling)
			current.wind_speed -= maxSpeedSleew;
		else
			current.wind_speed += maxSpeedSleew;
	}

	// simmilar thing for gusts
	if (abs(windGstDiff) > maxSpeedSleew) {
		// if the wind speed changed above the maximum speed limit apply a correction
		if (windGstDiff < 0)
			// if difference is negative it means that previous value of wind speed is greater than current
			// so a trend is negative (value is falling)
			current.wind_gusts -= maxSpeedSleew;
		else
			current.wind_gusts += maxSpeedSleew;
	}

	// simmilar thing for gusts
	if (abs(temperatureDiff) > maxTempSlew) {
		// if the wind speed changed above the maximum speed limit apply a correction
		if (temperatureDiff < 0)
			// if difference is negative it means that previous value of wind speed is greater than current
			// so a trend is negative (value is falling)
			current.temperature -= maxSpeedSleew;
		else
			current.temperature += maxSpeedSleew;
	}

}
