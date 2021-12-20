/*
 * ZywiecMeteo_test.cpp
 *
 *  Created on: Dec 19, 2021
 *      Author: mateusz
 */

#include "ZywiecMeteo.h"

#include <memory>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ZYWIEC_METEO
#include <boost/test/unit_test.hpp>

std::unique_ptr<ZywiecMeteo> zm;

struct fixture {
	fixture() {
		// empty
		zm.reset(new ZywiecMeteo("http://monitoring.zywiec.powiat.pl/"));


	}
};

BOOST_FIXTURE_TEST_CASE(download_rumcajs, fixture) {

	bool result;

	std::string response;

	result = zm->downloadLastMeasureForStation(505, response);

	BOOST_CHECK_EQUAL(result, true);

	std::cout << response << std::endl;

}

BOOST_FIXTURE_TEST_CASE(download_rumcajs_range, fixture) {
	bool result;

	std::string response;

	result = zm->downloadMeasuresFromRangeForStation(505, response);

	BOOST_CHECK_EQUAL(result, true);

	std::cout << response << std::endl;
}

BOOST_FIXTURE_TEST_CASE(parse_, fixture) {

	std::string json_data = "{\"result\":[{\"datetime\":\"2021-12-19 21:00:19\",\"rainfall_type\": \"mieszany\",\"temp\": \"3.30000\",\"humid\": \"84.00000\",\"wind_direct\": \"290.00\",\"wind_speed\": \"6.31\",\"rainfall_intensity\": \"3.00000\",\"pressure\": \"101300.00\",\"prtrend\": \"1\"}]}";

	AprsWXData out;

	std::cout << json_data << std::endl;

	zm->parseJson(json_data, out);
}
