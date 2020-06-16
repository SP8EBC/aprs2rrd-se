/*
 * PressureCalculator.h
 *
 *  Created on: 15.06.2020
 *      Author: mateusz
 */

#ifndef PRESSURECALCULATOR_H_
#define PRESSURECALCULATOR_H_

#include <cstdint>
#include <string>

enum class PressureCalculatorType {
	TO_QNH,
	TO_QFE,
	NONE
};

class PressureCalculator {
private:
	uint16_t altitude;

	PressureCalculatorType stationMeasures;
	PressureCalculatorType convertTo;

	bool enable;

public:
	PressureCalculator();
	virtual ~PressureCalculator();

	float convertPressure(float input, float temperature);



	void setAltitude(uint16_t altitude) {
		this->altitude = altitude;
	}

	void setConvertTo(PressureCalculatorType type) {
		this->convertTo = type;
	}

	void setEnable(bool enable) {
		this->enable = enable;
	}

	PressureCalculatorType getStationMeasures() const {
		return stationMeasures;
	}

	void setStationMeasures(PressureCalculatorType stationMeasures) {
		this->stationMeasures = stationMeasures;
	}

	static std::string toString(PressureCalculatorType in) {
		switch (in) {
		case PressureCalculatorType::TO_QFE: return "QFE - real pressure at altitude where station is located";
		case PressureCalculatorType::TO_QNH: return "QNH - pressure recalculated to the sea level";
		case PressureCalculatorType::NONE: return "NONE";
		default: return " ";
		}
	}

	static PressureCalculatorType fromString(std::string & in) {

		if (in == "qfe" || in == "QFE")
			return PressureCalculatorType::TO_QFE;
		else if (in == "qnh" || in == "QNH")
			return PressureCalculatorType::TO_QNH;

		return PressureCalculatorType::NONE;
	}

	uint16_t getAltitude() const {
		return altitude;
	}

	PressureCalculatorType getConvertTo() const {
		return convertTo;
	}

	bool isEnable() const {
		return enable;
	}
};

#endif /* PRESSURECALCULATOR_H_ */
