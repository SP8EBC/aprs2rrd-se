/*
 * BannerCreator_test.cpp
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BANNER_CREATOR
#include <boost/test/unit_test.hpp>

#include "BannerCreator.h"

#include <fstream>

struct MyConfig
{
  MyConfig() : test_log( "./test_reports/bannercreator_test.log" )
  {
    boost::unit_test::unit_test_log.set_stream( test_log );
    boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_level::log_successful_tests);

  }
  ~MyConfig()
  {
    boost::unit_test::unit_test_log.set_stream( std::cout );
  }

  std::ofstream test_log;
};

BOOST_GLOBAL_FIXTURE (MyConfig);

BOOST_AUTO_TEST_CASE(one)
{
	BannerCreatorConfig config;
	BannerCreator creator(config);

	AprsWXData data;

	creator.createBanner(data);
	BOOST_CHECK(creator.saveToDisk("test.png"));
}
