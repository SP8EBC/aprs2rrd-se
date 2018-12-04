/*
 * ProgramConfig.cpp
 *
 *  Created on: 04.12.2018
 *      Author: mateusz
 */

#include "ProgramConfig.h"


ProgramConfig::ProgramConfig(std::string fn) : configFilename(fn) {
	// TODO Auto-generated constructor stub

}

ProgramConfig::~ProgramConfig() {
	// TODO Auto-generated destructor stub
}

void ProgramConfig::parseFile() {
	config.readFile("config.conf");
}

void ProgramConfig::getDbConfig(MySqlConnInterface& db) {
}

void ProgramConfig::getAprsThreadConfig(AprsThreadConfig& aprs) {
}

void ProgramConfig::getDataPresenceConfig(DataPresence& data) {
}
