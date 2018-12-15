/*
 * AprsPacket_test.cpp
 *
 *  Created on: 15.12.2018
 *      Author: mateusz
 */


#define BOOST_TEST_MODULE AprsPacket_test
#include <boost/test/included/unit_test.hpp>
#include <fstream>

#include "AprsPacket.h"

struct MyConfig
{
  MyConfig() : test_log( "aprspacket_test.log" )
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

BOOST_AUTO_TEST_CASE(PacketFromUncompressPositionData)
{
	std::string input = "SQ9GPS-1>APRX29,SR9DZB*,WIDE2-2,qAR,SR6NKB:!5037.74N/01837.78E-145.275MHz Lubomir";

	AprsPacket out;

	AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK(strcmp(out.Data, "!5037.74N/01837.78E-145.275MHz Lubomir") == 0);
	BOOST_CHECK(out.SrcAddr == "SQ9GPS");
	BOOST_CHECK(out.SrcSSID == 1);
	BOOST_CHECK(out.DestAddr == "APRX29");
	BOOST_CHECK(out.DstSSID == 0);
	BOOST_CHECK_EQUAL(out.Path.size(), 2);
	BOOST_CHECK(out.ToISOriginator.Call == "SR6NKB");
	BOOST_CHECK(out.ToISOriginator.SSID == 0);

}

BOOST_AUTO_TEST_CASE(FirstPacketWithCompressedData)
{
	std::string input = "SQ9FQJ>UP0S66,WIDE1-1,WIDE2-2,qAR,SR9NSK:`0,Cmq;>/]\"5v}=";

	AprsPacket out;

	AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK(strcmp(out.Data, "`0,Cmq;>/]\"5v}=") == 0);
	BOOST_CHECK(out.SrcAddr == "SQ9FQJ");
	BOOST_CHECK(out.SrcSSID == 0);
	BOOST_CHECK(out.DestAddr == "UP0S66");
	BOOST_CHECK(out.DstSSID == 0);
	BOOST_CHECK_EQUAL(out.Path.size(), 2);
	BOOST_CHECK(out.ToISOriginator.Call == "SR9NSK");
	BOOST_CHECK(out.ToISOriginator.SSID == 0);

}

