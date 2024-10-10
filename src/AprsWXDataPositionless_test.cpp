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
    const std::string input = "_10101355c...s...g...t079[0_1258_277][1_629_270][2_0_265][3_2516_274]";
    AprsWXData output;

    AprsWXDataPositionless::ParseData(input, output);
}
