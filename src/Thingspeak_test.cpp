/*
 * Thingspeak_test.cpp
 *
 *  Created on: Apr 21, 2024
 *      Author: mateusz
 */

#include "Thingspeak.h"

#include <memory>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE THINGSPEAK
#include <boost/test/unit_test.hpp>

Thingspeak_Config config;

std::unique_ptr<Thingspeak> zm;

struct fixture {
	fixture() {

		config.channelId = 951169;
		config.temperatureFieldName = "Temperature";
		config.averageWindspeedFieldName = "Speed Avg";
		config.humidityFieldName = "Humidity";
		config.maximumWindspeedFieldName = "Speed Max";
		config.pressureFieldName = "Pressure";
		config.winddirectionFieldName = "Direction";
		config.runInterval = 600;

		zm = std::make_unique<Thingspeak>(config);

	}
};

BOOST_FIXTURE_TEST_CASE(download_skrzetla, fixture) {

	bool result;
	AprsWXData wx;

	result = zm->download();

	zm->getWeatherData(wx);

	BOOST_CHECK(result);

}
