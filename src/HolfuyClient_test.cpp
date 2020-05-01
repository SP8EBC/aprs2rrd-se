/*
 * HolfuyClient_test.cpp
 *
 *  Created on: 29.04.2020
 *      Author: mateusz
 */

#include "HolfuyClient.h"
#include <memory>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE HOLFUY
#include <boost/test/unit_test.hpp>

std::unique_ptr<HolfuyClient> ptr;

struct fixture1 {

	fixture1() {
		ptr.reset(new HolfuyClient(1161, "J00DjmU5wouaUKf"));
	}
};

BOOST_FIXTURE_TEST_CASE(connect, fixture1) {
	ptr->download();

	ptr->parse();
};
