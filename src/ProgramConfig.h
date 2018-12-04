/*
 * ProgramConfig.h
 *
 *  Created on: 04.12.2018
 *      Author: mateusz
 */

#ifndef PROGRAMCONFIG_H_
#define PROGRAMCONFIG_H_

#include <string>
#include <libconfig.h++>

#include "MySqlConnInterface.h"
#include "AprsThreadConfig.h"
#include "DataPresence.h"

class ProgramConfig {
	std::string configFilename;
	libconfig::Config config;

public:
	ProgramConfig(std::string fn);
	virtual ~ProgramConfig();

	void parseFile();
	void getDbConfig(MySqlConnInterface& db);
	void getAprsThreadConfig(AprsThreadConfig& aprs);
	void getDataPresenceConfig(DataPresence& data);



};

#endif /* PROGRAMCONFIG_H_ */
