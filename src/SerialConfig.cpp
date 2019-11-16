/*
 * SerialConfig.cpp
 *
 *  Created on: 30.10.2019
 *      Author: mateusz
 */

#include "SerialConfig.h"

SerialConfig::SerialConfig() {
	// TODO Auto-generated constructor stub

}

SerialConfig::~SerialConfig() {
	// TODO Auto-generated destructor stub
}

bool SerialConfig::validateAprsPacket(AprsPacket& packet) {
	if (this->captureAll)
		return true;
	else {
		if (this->call == packet.SrcAddr && this->ssid == packet.SrcSSID)
			return true;
		else
			return false;
	}
}
