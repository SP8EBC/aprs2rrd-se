/*
 * ZywiecMeteo_test.cpp
 *
 *  Created on: Dec 19, 2021
 *      Author: mateusz
 */

#include "ZywiecMeteo.h"

#include <memory>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ZYWIEC_METEO
#include <boost/test/unit_test.hpp>

std::unique_ptr<ZywiecMeteo> zm;

struct fixture {
	fixture() {
		// empty
		//zm.reset(new ZywiecMeteo("http://monitoring.zywiec.powiat.pl/", true));
		zm = std::make_unique<ZywiecMeteo>("http://monitoring.zywiec.powiat.pl/", false);

	}
};

BOOST_FIXTURE_TEST_CASE(download_rumcajs, fixture) {

	bool result;

	std::string response;

	result = zm->downloadLastMeasureForStation(505, response);

	BOOST_CHECK_EQUAL(result, true);

	std::cout << response << std::endl;

}

BOOST_FIXTURE_TEST_CASE(download_rumcajs_range, fixture) {
	bool result;

	std::string response;
	AprsWXData out;

	result = zm->downloadMeasuresFromRangeForStation(505, response);

	BOOST_CHECK_EQUAL(result, true);

	std::cout << response << std::endl;

	zm->parseJson(response, out);

	BOOST_CHECK_EQUAL(out.useTemperature, true);
}

BOOST_FIXTURE_TEST_CASE(parse_, fixture) {

	std::string json_data = "{\"result\":[{\"datetime\":\"2021-12-19 21:00:19\",\"rainfall_type\": \"mieszany\",\"temp\": \"3.30000\",\"humid\": \"84.00000\",\"wind_direct\": \"290.00\",\"wind_speed\": \"6.31\",\"rainfall_intensity\": \"3.00000\",\"pressure\": \"101300.00\",\"prtrend\": \"1\"}]}";

	AprsWXData out;

	std::cout << json_data << std::endl;

	zm->parseJson(json_data, out);
}

BOOST_FIXTURE_TEST_CASE(parse_multi, fixture) {
	std::string json_data =  "{\"result\":[{\"datetime\":\"2021-12-23 19:10\",\"temp\":\"0.00\",\"temp2m\":\"-1.60\",\"tempmin2m\":null,\"tempmax2m\":null,\"tempmin\":null,\"tempmax\":null,\"dew_point\":\"-5.87\",\"humid\":\"65.00\",\"humid2m\":\"99.90\",\"humidmax\":\"0\",\"humidmax2m\":\"0\",\"humidmin\":\"0\",\"humidmin2m\":\"0\",\"wind_min\":\"0\",\"wind_direct\":\"177.78\",\"wind_speed\":\"1.61\",\"wind_max\":\"7.16\",\"rainfall_type\":\"brak\",\"rainfall\":\"0\",\"rainfall_intensity\":\"0.00\",\"pressure\":\"101130.00\"},{\"datetime\":\"2021-12-23 19:20\",\"temp\":\"0.10\",\"temp2m\":\"-1.60\",\"tempmin2m\":null,\"tempmax2m\":null,\"tempmin\":null,\"tempmax\":null,\"dew_point\":\"-5.57\",\"humid\":\"66.00\",\"humid2m\":\"99.90\",\"humidmax\":\"0\",\"humidmax2m\":\"0\",\"humidmin\":\"0\",\"humidmin2m\":\"0\",\"wind_min\":\"0\",\"wind_direct\":\"175.31\",\"wind_speed\":\"1.39\",\"wind_max\":\"7.16\",\"rainfall_type\":\"brak\",\"rainfall\":\"0\",\"rainfall_intensity\":\"0.00\",\"pressure\":\"101110.00\"},{\"datetime\":\"2021-12-23 19:30\",\"temp\":\"0.10\",\"temp2m\":\"-1.50\",\"tempmin2m\":null,\"tempmax2m\":null,\"tempmin\":null,\"tempmax\":null,\"dew_point\":\"-5.78\",\"humid\":\"65.00\",\"humid2m\":\"99.90\",\"humidmax\":\"0\",\"humidmax2m\":\"0\",\"humidmin\":\"0\",\"humidmin2m\":\"0\",\"wind_min\":\"0\",\"wind_direct\":\"188.80\",\"wind_speed\":\"1.47\",\"wind_max\":\"5.91\",\"rainfall_type\":\"brak\",\"rainfall\":\"0\",\"rainfall_intensity\":\"0.00\",\"pressure\":\"101100.00\"},{\"datetime\":\"2021-12-23 19:40\",\"temp\":\"0.10\",\"temp2m\":\"-1.50\",\"tempmin2m\":null,\"tempmax2m\":null,\"tempmin\":null,\"tempmax\":null,\"dew_point\":\"-5.57\",\"humid\":\"66.00\",\"humid2m\":\"99.90\",\"humidmax\":\"0\",\"humidmax2m\":\"0\",\"humidmin\":\"0\",\"humidmin2m\":\"0\",\"wind_min\":\"0\",\"wind_direct\":\"218.17\",\"wind_speed\":\"2.82\",\"wind_max\":\"12.11\",\"rainfall_type\":\"\\u015bnieg\",\"rainfall\":\"0\",\"rainfall_intensity\":\"0.00\",\"pressure\":\"101130.00\"},{\"datetime\":\"2021-12-23 19:50\",\"temp\":\"0.00\",\"temp2m\":\"-1.40\",\"tempmin2m\":null,\"tempmax2m\":null,\"tempmin\":null,\"tempmax\":null,\"dew_point\":\"-5.87\",\"humid\":\"65.00\",\"humid2m\":\"99.90\",\"humidmax\":\"0\",\"humidmax2m\":\"0\",\"humidmin\":\"0\",\"humidmin2m\":\"0\",\"wind_min\":\"0\",\"wind_direct\":\"194.92\",\"wind_speed\":\"1.99\",\"wind_max\":\"12.11\",\"rainfall_type\":\"brak\",\"rainfall\":\"0\",\"rainfall_intensity\":\"0.00\",\"pressure\":\"101140.00\"},{\"datetime\":\"2021-12-23 20:00\",\"temp\":\"0.00\",\"temp2m\":\"-1.50\",\"tempmin2m\":null,\"tempmax2m\":null,\"tempmin\":null,\"tempmax\":null,\"dew_point\":\"-6.08\",\"humid\":\"64.00\",\"humid2m\":\"99.90\",\"humidmax\":\"0\",\"humidmax2m\":\"0\",\"humidmin\":\"0\",\"humidmin2m\":\"0\",\"wind_min\":\"0\",\"wind_direct\":\"162.21\",\"wind_speed\":\"1.88\",\"wind_max\":\"6.86\",\"rainfall_type\":\"brak\",\"rainfall\":\"0\",\"rainfall_intensity\":\"0.00\",\"pressure\":\"101130.00\"}]}\r\n";

	AprsWXData out;

	zm->parseJson(json_data, out);

	BOOST_CHECK_CLOSE(out.temperature, 0.00, 0.1);
	BOOST_CHECK_CLOSE(out.pressure, 1011.3, 0.1);
	BOOST_CHECK_CLOSE(out.humidity, 64.0, 0.1);

}

/**
 * {"result":[{"datetime":"2021-12-23 19:10","temp":"0.00","temp2m":"-1.60","tempmin2m":null,"tempmax2m":null,"tempmin":null,"tempmax":null,"dew_point":"-5.87","humid":"65.00","humid2m":"99.90","humidmax":"0","humidmax2m":"0","humidmin":"0","humidmin2m":"0","wind_min":"0","wind_direct":"177.78","wind_speed":"1.61","wind_max":"7.16","rainfall_type":"brak","rainfall":"0","rainfall_intensity":"0.00","pressure":"101130.00"},{"datetime":"2021-12-23 19:20","temp":"0.10","temp2m":"-1.60","tempmin2m":null,"tempmax2m":null,"tempmin":null,"tempmax":null,"dew_point":"-5.57","humid":"66.00","humid2m":"99.90","humidmax":"0","humidmax2m":"0","humidmin":"0","humidmin2m":"0","wind_min":"0","wind_direct":"175.31","wind_speed":"1.39","wind_max":"7.16","rainfall_type":"brak","rainfall":"0","rainfall_intensity":"0.00","pressure":"101110.00"},{"datetime":"2021-12-23 19:30","temp":"0.10","temp2m":"-1.50","tempmin2m":null,"tempmax2m":null,"tempmin":null,"tempmax":null,"dew_point":"-5.78","humid":"65.00","humid2m":"99.90","humidmax":"0","humidmax2m":"0","humidmin":"0","humidmin2m":"0","wind_min":"0","wind_direct":"188.80","wind_speed":"1.47","wind_max":"5.91","rainfall_type":"brak","rainfall":"0","rainfall_intensity":"0.00","pressure":"101100.00"},{"datetime":"2021-12-23 19:40","temp":"0.10","temp2m":"-1.50","tempmin2m":null,"tempmax2m":null,"tempmin":null,"tempmax":null,"dew_point":"-5.57","humid":"66.00","humid2m":"99.90","humidmax":"0","humidmax2m":"0","humidmin":"0","humidmin2m":"0","wind_min":"0","wind_direct":"218.17","wind_speed":"2.82","wind_max":"12.11","rainfall_type":"\u015bnieg","rainfall":"0","rainfall_intensity":"0.00","pressure":"101130.00"},{"datetime":"2021-12-23 19:50","temp":"0.00","temp2m":"-1.40","tempmin2m":null,"tempmax2m":null,"tempmin":null,"tempmax":null,"dew_point":"-5.87","humid":"65.00","humid2m":"99.90","humidmax":"0","humidmax2m":"0","humidmin":"0","humidmin2m":"0","wind_min":"0","wind_direct":"194.92","wind_speed":"1.99","wind_max":"12.11","rainfall_type":"brak","rainfall":"0","rainfall_intensity":"0.00","pressure":"101140.00"},{"datetime":"2021-12-23 20:00","temp":"0.00","temp2m":"-1.50","tempmin2m":null,"tempmax2m":null,"tempmin":null,"tempmax":null,"dew_point":"-6.08","humid":"64.00","humid2m":"99.90","humidmax":"0","humidmax2m":"0","humidmin":"0","humidmin2m":"0","wind_min":"0","wind_direct":"162.21","wind_speed":"1.88","wind_max":"6.86","rainfall_type":"brak","rainfall":"0","rainfall_intensity":"0.00","pressure":"101130.00"}]}
 *
 *
 */
