/*
 * PressureCalculator.cpp
 *
 *  Created on: 15.06.2020
 *      Author: mateusz
 */

#include "PressureCalculator.h"
#include <math.h>

PressureCalculator::PressureCalculator() {
	this->convertTo = PressureCalculatorType::NONE;
	this->stationMeasures = PressureCalculatorType::NONE;
	altitude = 1;
	enable = false;
}

PressureCalculator::~PressureCalculator() {
}

float PressureCalculator::convertPressure(float input, float temperature) {

	float out = 0.0f;
	float h_isa = 0.0f;

	if (!enable)
		return input;

	switch (this->convertTo) {
	case PressureCalculatorType::TO_QFE:
		out = input * ::pow(M_E, -(0.0289644 * 9.94 * this->altitude)/(8.3144598 * (temperature + 273) ));
		break;
	case PressureCalculatorType::TO_QNH:
		h_isa = 44330.77 - 11880.32 * ::pow(input, 0.190263);
		out = 1013.25f * ::pow((1 - 0.0065 * ((h_isa - this->altitude) / 288.15f)), 5.24488);
		break;
	default: break;
	}

	return out;
}
