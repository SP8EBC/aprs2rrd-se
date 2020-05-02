/*
 * HolfuyClientConfig.h
 *
 *  Created on: 02.05.2020
 *      Author: mateusz
 */

#ifndef HOLFUYCLIENTCONFIG_H_
#define HOLFUYCLIENTCONFIG_H_

class HolfuyClientConfig {
public:

	std::string apiPassword;

	int stationId;

	HolfuyClientConfig();
	virtual ~HolfuyClientConfig();
};

#endif /* HOLFUYCLIENTCONFIG_H_ */
