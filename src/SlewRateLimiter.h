/*
 * SlewRateLimiter.h
 *
 *  Created on: 27.11.2018
 *      Author: mateusz
 */

#ifndef SLEWRATELIMITER_H_
#define SLEWRATELIMITER_H_

#include <cstdint>

#include "AprsWXData.h"

#define MAX_TEMP_SLEW 3.0f
#define MAX_PRESSURE_SLEW 5.0f
#define MAX_SPEED_SLEW 5.0f
#define MAX_GUSTS_SLEW 12.0f
#define MAX_DIRECTION_SLEW 0

/**
 * This class is used to correct unwanted spikes in parameter values which sometimes
 * occur due to some RF noise present on site, or software bugs or another problems which
 * cannot be easly corrected.
 */
class SlewRateLimiter {
private:
	float maxTempSlew;
	int16_t maxPressureSlew;
	float maxSpeedSleew;
	float maxGustsSleew;
	int32_t maxDirectionSleew;

	uint32_t callCounter;
public:
	SlewRateLimiter();
	virtual ~SlewRateLimiter();

	void limitFromSingleFrame(const AprsWXData & previous, AprsWXData & current);
};

#endif /* SLEWRATELIMITER_H_ */
