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
#include "ReturnValues.h"

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

BOOST_AUTO_TEST_CASE(SecondPacketFromUncompressPositionData)
{
	std::string input = "SQ9MYX-9>APMT01,WIDE1-1,WIDE2-1,qAR,SR9NSK:=4937.44N/01911.13E>PHG1030172";

	AprsPacket out;

	AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK(strcmp(out.Data, "=4937.44N/01911.13E>PHG1030172") == 0);
	BOOST_CHECK(out.SrcAddr == "SQ9MYX");
	BOOST_CHECK(out.SrcSSID == 9);
	BOOST_CHECK(out.DestAddr == "APMT01");
	BOOST_CHECK(out.DstSSID == 0);
	BOOST_CHECK_EQUAL(out.Path.size(), 2);
	BOOST_CHECK(out.ToISOriginator.Call == "SR9NSK");
	BOOST_CHECK(out.ToISOriginator.SSID == 0);

}

BOOST_AUTO_TEST_CASE(FirstPacketWithCompressedData)
{
	std::string input = "SQ9FQJ>UP0S66,WIDE1-1,WIDE2-2,qAR,SR9NSK:`0,Cmq;>/]\"5v}=";

	AprsPacket out;

	AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK(strcmp(out.Data, "`0,Cmq;>/]\"5v}=") == 0);	// this will fail becaue this frame conssist '>' in compressed data
	BOOST_CHECK(out.SrcAddr == "SQ9FQJ");
	BOOST_CHECK(out.SrcSSID == 0);
	BOOST_CHECK(out.DestAddr == "UP0S66");
	BOOST_CHECK(out.DstSSID == 0);
	BOOST_CHECK_EQUAL(out.Path.size(), 2);
	BOOST_CHECK(out.ToISOriginator.Call == "SR9NSK");
	BOOST_CHECK(out.ToISOriginator.SSID == 0);

}

BOOST_AUTO_TEST_CASE(SecondPacketWithCompressedData)
{
	std::string input = "SQ9FQJ>UP0S16,WIDE1-1,WIDE2-2,qAS,SQ9ZAY-3:`0,Yl w>/]\"6%}=";

	AprsPacket out;

	AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK(strcmp(out.Data, "`0,Yl w>/]\"6%}=") == 0);	// this will fail becaue this frame conssist '>' in compressed data
	BOOST_CHECK(out.SrcAddr == "SQ9FQJ");
	BOOST_CHECK(out.SrcSSID == 0);
	BOOST_CHECK(out.DestAddr == "UP0S16");
	BOOST_CHECK(out.DstSSID == 0);
	BOOST_CHECK_EQUAL(out.Path.size(), 2);
	BOOST_CHECK(out.ToISOriginator.Call == "SQ9ZAY");
	BOOST_CHECK(out.ToISOriginator.SSID == 3);

}

BOOST_AUTO_TEST_CASE(wxPacketWoPath)
{
	std::string input = "SR8WXO>AKLPRZ,qAR,SR9NFB:!4944.20N/02150.45E_000/000g000t...r...p...P...b...";

	AprsPacket out;

	AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK(strcmp(out.Data, "!4944.20N/02150.45E_000/000g000t...r...p...P...b...") == 0);
	BOOST_CHECK(out.SrcAddr == "SR8WXO");
	BOOST_CHECK(out.SrcSSID == 0);
	BOOST_CHECK(out.DestAddr == "AKLPRZ");
	BOOST_CHECK(out.DstSSID == 0);
	BOOST_CHECK_EQUAL(out.Path.size(), 0);
	BOOST_CHECK(out.ToISOriginator.Call == "SR9NFB");
	BOOST_CHECK(out.ToISOriginator.SSID == 0);

}

BOOST_AUTO_TEST_CASE(packetWithBrokenSrcCall)
{
	std::string input = "SQ99Q_>UP0S16,WIDE1-1,WIDE2-2,qAS,SQ9ZAY-3:`0,Yl w>/]\"6%}=";

	AprsPacket out;

	int parsingResult = -1;

	parsingResult = AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK_EQUAL(parsingResult, NOT_VALID_APRS_PACKET);


}

BOOST_AUTO_TEST_CASE(packetWithBrokenPath)
{
	std::string input = "SQ9FQJ>UP0S16,WIDE1-1=IDE2-2,qAS,SQ9ZAY-3:`0,Yl w>/]\"6%}=";

	AprsPacket out;

	int parsingResult = -1;

	parsingResult = AprsPacket::ParseAPRSISData((char*)input.c_str(), input.size(), &out);

	BOOST_CHECK_EQUAL(parsingResult, NOT_VALID_APRS_PACKET);


}
