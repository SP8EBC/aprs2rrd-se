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
    //boost::unit_test::unit_test_log.set_stream( test_log );
    boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_level::log_successful_tests);

  }
  ~MyConfig()
  {
    boost::unit_test::unit_test_log.set_stream( std::cout );
  }

  std::ofstream test_log;
};

BOOST_GLOBAL_FIXTURE (MyConfig);

BOOST_AUTO_TEST_CASE (temperatureRisingFast)
{
	AprsWXData t1, t2, t3, t4;

	t1.temperature = 1.0f;
	t2.temperature = 10.0f;
	t3.temperature = 12.0f;
	t4.temperature = 9.0f;

	t1.useTemperature = true;
	t2.useTemperature = true;
	t3.useTemperature = true;
	t4.useTemperature = true;

	SlewRateLimiter limiter;

	limiter.limitFromSingleFrame(t1, t2);
	BOOST_CHECK( t2.temperature == 4.0f);

	limiter.limitFromSingleFrame(t2, t3);
	BOOST_CHECK( t3.temperature == 7.0f);

	limiter.limitFromSingleFrame(t3, t4);
	BOOST_CHECK( t4.temperature == 9.0f);
}

BOOST_AUTO_TEST_CASE (windSpeedPeaks)
{
	AprsWXData t1, t2, t3, t4, t5;
	SlewRateLimiter limiter;

	t1.useWind = true;
	t2.useWind = true;
	t3.useWind = true;
	t4.useWind = true;
	t5.useWind = true;

	t1.wind_speed = 3.0f;
	t1.wind_gusts = 7.2f;

	t2.wind_speed = 30.0f;
	t2.wind_gusts = 55.0f;

	t3.wind_speed = 20.0f;
	t3.wind_gusts = 25.0f;

	t4.wind_speed = 4.2f;
	t4.wind_gusts = 8.9f;

	t5.wind_speed = 3.0f;
	t5.wind_gusts = 5.2f;

	BOOST_CHECK( t1.wind_speed == 3.0f);
	BOOST_CHECK( t1.wind_gusts == 7.2f);

	limiter.limitFromSingleFrame(t1, t2);
	BOOST_CHECK( t2.wind_speed == 8.0f);
	BOOST_CHECK( t2.wind_gusts == 19.2f);

	limiter.limitFromSingleFrame(t2, t3);
	BOOST_CHECK( t3.wind_speed == 13.0f);
	BOOST_CHECK( t3.wind_gusts == 25.0f);

	limiter.limitFromSingleFrame(t3, t4);
	BOOST_CHECK( t4.wind_speed == 8.0f);
	BOOST_CHECK( t4.wind_gusts == 13.0f);

	limiter.limitFromSingleFrame(t4, t5);
	BOOST_CHECK( t5.wind_speed == 3.0f);
	BOOST_CHECK( t5.wind_gusts == 5.2f);


}
