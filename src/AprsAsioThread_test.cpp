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
	conf.ServerURL = "euro.aprs2.net";
	conf.ServerPort = 14580;

	AprsAsioThread t(conf);
	t.connect();

	BOOST_CHECK(a == b);
}

