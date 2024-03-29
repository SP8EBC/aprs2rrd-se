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
#define MAX_HUMIDITY_SLEW	2

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
	int32_t maxHumiditySlew;
	bool changedFromDefault;
	bool hmZeroAs100;

	uint32_t callCounter;
public:
	SlewRateLimiter();
	virtual ~SlewRateLimiter();

	void limitFromSingleFrame(const AprsWXData & previous, AprsWXData & current);

	void setMaxDirectionSleew(int32_t maxDirectionSleew) {
		changedFromDefault = true;
		this->maxDirectionSleew = maxDirectionSleew;
	}

	void setMaxGustsSleew(float maxGustsSleew) {
		if (maxPressureSlew > 20.0f || maxPressureSlew < -20.0f) {
			return;
		}

		changedFromDefault = true;
		this->maxGustsSleew = maxGustsSleew;
	}

	void setMaxPressureSlew(int16_t maxPressureSlew) {
		if (maxPressureSlew > 40 || maxPressureSlew < -40) {
			return;
		}

		changedFromDefault = true;

		this->maxPressureSlew = maxPressureSlew;
	}

	void setMaxSpeedSleew(float maxSpeedSleew) {
		if (maxPressureSlew > 20.0f || maxPressureSlew < -20.0f) {
			return;
		}

		changedFromDefault = true;

		this->maxSpeedSleew = maxSpeedSleew;
	}

	void setMaxTempSlew(float maxTempSlew) {
		if (maxPressureSlew > 20.0f || maxPressureSlew < -20.0f) {
			return;
		}

		changedFromDefault = true;

		this->maxTempSlew = maxTempSlew;
	}

	bool isChangedFromDefault() const {
		return changedFromDefault;
	}

	int32_t getMaxDirectionSleew() const {
		return maxDirectionSleew;
	}

	float getMaxGustsSleew() const {
		return maxGustsSleew;
	}

	int16_t getMaxPressureSlew() const {
		return maxPressureSlew;
	}

	float getMaxSpeedSleew() const {
		return maxSpeedSleew;
	}

	float getMaxTempSlew() const {
		return maxTempSlew;
	}

	int32_t getMaxHumiditySlew() const {
		return maxHumiditySlew;
	}

	void setMaxHumiditySlew(int32_t maxHumiditySlew) {
		this->maxHumiditySlew = maxHumiditySlew;

		changedFromDefault = true;
	}

	bool isHmZeroAs100() const {
		return hmZeroAs100;
	}

	void setHmZeroAs100(bool hmZeroAs100) {
		this->hmZeroAs100 = hmZeroAs100;
	}
};

#endif /* SLEWRATELIMITER_H_ */
