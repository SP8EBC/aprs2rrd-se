/*
 * AprsAsioThread_test.cpp
 *
 *  Created on: 20.10.2018
 *      Author: mateusz
 */

#include "AprsAsioThread.h"
#include "AprsThreadConfig.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ASIO_THREAD
#include <boost/test/unit_test.hpp>

bool Debug = false;

BOOST_AUTO_TEST_CASE(basic) {
	int a = 1, b = 1;

	AprsThreadConfig conf;
	conf.Call = "SP8EBC";
	conf.Passwd = 23220;
	conf.ServerURL = "euro.aprs2.net";
	conf.ServerPort = 14580;
	conf.StationCall = "SR9WXS";
	conf.StationSSID = 0;

	AprsAsioThread t(conf);
	t.connect();
	for (;;) {
		t.receive();
	}

	BOOST_CHECK(a == b);
}

