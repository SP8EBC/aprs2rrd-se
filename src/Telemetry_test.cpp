/*
 * Telemetry_test.cpp
 *
 *  Created on: 26.08.2018
 *      Author: mateusz
 */

#include "Telemetry.h"
#include "AprsPacket.h"
#include <string>
#include <stdio.h>
#include <string.h>

bool Debug = false;
bool DebugToFile = false;
//std::ofstream fDebug;

int main() {

	std::string str = "T#103,056,003,004,000,147,10000000";

	Telemetry test;
	AprsPacket packet;

	const char* testo = str.c_str();
	char* testpacket = packet.Data;

	strcpy(testpacket, testo);

//	test.ParseData(&packet);
}


