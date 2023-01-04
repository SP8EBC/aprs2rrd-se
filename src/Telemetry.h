/*
 * Telemetry.h
 *
 *  Created on: 26.08.2018
 *      Author: mateusz
 */

#include "AprsPacket.h"
#include <stdint.h>

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

class Telemetry {
public:
	Telemetry();
	virtual ~Telemetry();

	static int ParseData(AprsPacket input, Telemetry* output);

	static bool Debug;

	float getCh1() const;
	float getCh2() const;
	float getCh3() const;
	float getCh4() const;
	float getCh5() const;

	uint8_t getCh5Raw() const {
		return ch5;
	}

	float getLastTemperature() const;
	float getBatteryVoltage() const;
	uint16_t getRawMeasurement() const;
	float getTemperatureFromRawMeasurement() const;

	bool getLSERDY() const;
	bool getRTCEN() const;
	bool getMAXOK() const;
	bool getSLEEP() const;
	bool getSPIER() const;
	bool getSPIOK() const;

	std::string call;

	bool valid;

	uint16_t num;

	uint8_t ch1;
	uint8_t ch2;
	uint8_t ch3;
	uint8_t ch4;
	uint8_t ch5;
	uint8_t digital;

	float ch1a;
	float ch1b;
	float ch1c;

	float ch2a;
	float ch2b;
	float ch2c;

	float ch3a;
	float ch3b;
	float ch3c;

	float ch4a;
	float ch4b;
	float ch4c;

	float ch5a;
	float ch5b;
	float ch5c;


};

#endif /* TELEMETRY_H_ */
