/*
 * SerialConfig.h
 *
 *  Created on: 30.10.2019
 *      Author: mateusz
 */

#ifndef SERIALCONFIG_H_
#define SERIALCONFIG_H_

#include <cstdint>
#include <string>

class SerialConfig {

public:
	bool enable;

	std::string serialPort;

	uint32_t baudrate;

	bool captureAll;

	std::string call;
	uint32_t ssid;

	SerialConfig();
	virtual ~SerialConfig();
};

#endif /* SERIALCONFIG_H_ */
