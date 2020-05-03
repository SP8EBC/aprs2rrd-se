/*
 * HolfuyClientConfig.h
 *
 *  Created on: 02.05.2020
 *      Author: mateusz
 */

#ifndef HOLFUYCLIENTCONFIG_H_
#define HOLFUYCLIENTCONFIG_H_

#include <string>

class HolfuyClientConfig {
public:
	bool enable;

	int stationId;

	std::string apiPassword;

	bool dumpIntoMysql;

	HolfuyClientConfig();
	virtual ~HolfuyClientConfig();
};

#endif /* HOLFUYCLIENTCONFIG_H_ */
