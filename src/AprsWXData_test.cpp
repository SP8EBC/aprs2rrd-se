
#define BOOST_TEST_MODULE AprsWXData_test
#include <boost/test/included/unit_test.hpp>
#include <fstream>

#include "AprsWXData.h"
#include "AprsPacket.h"

struct MyConfig
{
  MyConfig() : test_log( "aprswxdata_test.log" )
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

// !4946.09N/01902.33E_313/002g004t037r...p...P...b10030h99\r

BOOST_AUTO_TEST_CASE(correct_wx_frame_1) {
	AprsPacket packet;
	AprsWXData wx;

	memcpy(packet.Data, "!4946.09N/01902.33E_313/002g004t037r...p...P...b10030h99\r", 58);

	AprsWXData::ParseData(packet, &wx);

	BOOST_CHECK(wx.humidity == 99);
	BOOST_CHECK_CLOSE(wx.temperature, 2.7, 3);	// the third parameter is a maximum percentage deviation which is acceptable
	BOOST_CHECK(wx.pressure == 1003);
	BOOST_CHECK(wx.wind_direction == 313);

	BOOST_CHECK(wx.valid);

}

BOOST_AUTO_TEST_CASE(corrupted_frame_1) {
	AprsPacket packet;
	AprsWXData wx;

	memcpy(packet.Data, "!4946.09N/01902.33E_313/002g004t037r...p...P...b10030h99\r", 38);

	AprsWXData::ParseData(packet, &wx);

	BOOST_CHECK(!wx.valid);
}

BOOST_AUTO_TEST_CASE(corrupted_frame_2) {
	AprsPacket packet;
	AprsWXData wx;

	memcpy(packet.Data, "!4946.09N/01902.33E_313!002g004t037r...p...P...b10030h99\r", 58);

	AprsWXData::ParseData(packet, &wx);

	BOOST_CHECK(!wx.valid);
}

BOOST_AUTO_TEST_CASE(correct_wx_without_h) {
	AprsPacket packet;
	AprsWXData wx;

	memcpy(packet.Data, "!4946.09N/01902.33E_313/002g004t037r...p...P...b10030", 54);

	AprsWXData::ParseData(packet, &wx);

	//BOOST_CHECK(wx.humidity == 99);
	BOOST_CHECK_CLOSE(wx.temperature, 2.7, 3);	// the third parameter is a maximum percentage deviation which is acceptable
	BOOST_CHECK(wx.pressure == 1003);
	BOOST_CHECK(wx.wind_direction == 313);

	BOOST_CHECK(wx.valid);
}
