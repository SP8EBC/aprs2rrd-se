/*
 * SlewRateLimiter_test.cpp
 *
 *  Created on: 27.11.2018
 *      Author: mateusz
 */

#include "SlewRateLimiter.h"

#define BOOST_TEST_MODULE SleewRateLmiter_test
#include <boost/test/included/unit_test.hpp>
#include <fstream>


struct MyConfig
{
  MyConfig() : test_log( "example.log" )
  {
    boost::unit_test::unit_test_log.set_stream( test_log );
  }
  ~MyConfig()
  {
    boost::unit_test::unit_test_log.set_stream( std::cout );
  }

  std::ofstream test_log;
};

BOOST_AUTO_TEST_CASE (temperatureRisingFast)
{
	AprsWXData c, p;

	c.temperature = 10.0f;
	p.temperature = 1.0f;

	SlewRateLimiter limiter;

	limiter.limitFromSingleFrame(p, c);

	BOOST_CHECK( c.temperature == 4.0f);

}
