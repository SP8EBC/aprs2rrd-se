/*
 * WeatherlinkClient_test.cpp
 *
 *  Created on: Mar 21, 2023
 *      Author: mateusz
 */

#include "WeatherlinkClient.h"

#include <memory>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE WEATHERLINK
#include <boost/test/unit_test.hpp>

std::unique_ptr<WeatherlinkClient> ptr;

BOOST_AUTO_TEST_CASE(connect) {

    // https://api.weatherlink.com/v1/NoaaExt.xml?user=001D0A00C5C3&pass=Zwirowa22&apiToken=404045B3CAAD47689DDFA91094A87F67


	ptr = std::make_unique<WeatherlinkClient>();

	ptr->config.DID = "001D0A00C5C3";
	ptr->config.apiPassword = "Zwirowa22";
	ptr->config.apiToken = "404045B3CAAD47689DDFA91094A87F67";

	ptr->download();

	ptr->parse();
};
