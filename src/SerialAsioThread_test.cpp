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

	SerialAsioThread serial("/dev/ttyUSB0", 9600, boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none),
													boost::asio::serial_port_base::character_size(),
													boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none),
													boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

	serial.openPort();
	serial.waitForRx();

	BOOST_CHECK(a == b);
}
