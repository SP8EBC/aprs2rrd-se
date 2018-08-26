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

	char ParseData(AprsPacket* input);
	float getCh1();
	float getCh2();
	float getCh3();
	float getCh4();
	float getCh5();

	bool val;

	uint16_t num;

	uint8_t ch1;
	uint8_t ch2;
	uint8_t ch3;
	uint8_t ch4;
	uint8_t ch5;

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
