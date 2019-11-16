/*
 * SerialAsioThread_test.cpp
 *
 *  Created on: 30.10.2019
 *      Author: mateusz
 */

#include "SerialAsioThread.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SERIAL_THREAD
#include <boost/test/unit_test.hpp>

bool Debug = false;

BOOST_AUTO_TEST_CASE(basic) {
	int a = 1, b = 1;

	int counter = 0;

	AprsPacket packet;

	SerialAsioThread serial("/dev/ttyUSB0", 9600);

	serial.openPort();
	serial.receive(false);
	serial.waitForRx();

	packet = serial.getPacket();

	BOOST_CHECK(a == b);
}
