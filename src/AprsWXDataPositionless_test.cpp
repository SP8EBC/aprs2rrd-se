#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE APRS_WX_DATA_POSITIONLESS
#include <boost/test/unit_test.hpp>

#include "AprsWXDataPositionless.h"

#include <fstream>

struct MyConfig
{
  MyConfig() : test_log( "./test_reports/aprswxdata_positionless_test.log" )
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

BOOST_AUTO_TEST_CASE(first)
{
    const std::string input = "_10101355c...s...g...t079[0_1258_277][1_629_220][2_0_295][3_2516_264]";
    AprsWXData output;

    AprsWXDataPositionless::ParseData(input, &output);

    BOOST_CHECK_CLOSE(output.temperature, 26.1, 1);

    BOOST_CHECK_CLOSE(output.additionalTemperature.at(0).second, 27.7, 1);
    BOOST_CHECK_CLOSE(output.additionalTemperature.at(1).second, 22.0, 1);
    BOOST_CHECK_CLOSE(output.additionalTemperature.at(2).second, 29.5, 1);
    BOOST_CHECK_CLOSE(output.additionalTemperature.at(3).second, 26.4, 1);

}
