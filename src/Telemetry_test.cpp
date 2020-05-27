/*
 * Telemetry_test.cpp
 *
 *  Created on: 26.08.2018
 *      Author: mateusz
 */

#include "Telemetry.h"
#include "AprsPacket.h"
#include "ReturnValues.h"


#define BOOST_TEST_MODULE Telemetry_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(first) {

	const char* input = "SR9WXS>AKLPRZ,WIDE2-1,qAR,SP9UVG-13:T#008,084,012,001,003,152,10001000";

	AprsPacket target;
	Telemetry output;

	int parsing_result = AprsPacket::ParseAPRSISData(const_cast<char*>(input), ::strlen(input), &target);

	if (parsing_result == OK) {
		Telemetry::ParseData(target, &output);
	}

	BOOST_CHECK_EQUAL(output.num, 8);
	BOOST_CHECK_EQUAL(output.ch1, 84);
	BOOST_CHECK_EQUAL(output.ch2, 12);
	BOOST_CHECK_EQUAL(output.ch3, 1);
	BOOST_CHECK_EQUAL(output.ch4, 3);
	BOOST_CHECK_EQUAL(output.ch5, 152);
	BOOST_CHECK_EQUAL(output.digital, 136);
	BOOST_CHECK_EQUAL(output.valid, true);
}
