/*
 * HolfuyClient.h
 *
 *  Created on: 29.04.2020
 *      Author: mateusz
 */

#ifndef HOLFUYCLIENT_H_
#define HOLFUYCLIENT_H_

#include <cstdint>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "AprsWXData.h"

class HolfuyClient {
private:
	uint32_t stationId;

	std::string stationLocation;

	boost::posix_time::ptime timestamp;

	float_t windspeed;

	float_t windgusts;

	uint16_t winddirection;

	uint8_t humidity;

	uint16_t pressure;

	uint8_t rain;

	std::string apiPassword;

	size_t write_callback(char *get_data, size_t always_one, size_t get_data_size, void *userdata);

	std::string getUrl() {

		std::string out = "http://api.holfuy.com/live/?s=" + std::to_string((int)stationId) + "&pass=" + apiPassword +  "&m=XML&tu=C&su=m/s";

		return out;
	}

public:
	HolfuyClient(uint32_t id, std::string apiPassword);
	~HolfuyClient();

	// this method will download the data from Holfuy API and then store in itnernal fields
	void download();

	// move that data to AprsWXData object
	void getWxData(AprsWXData & out);
};

#endif /* HOLFUYCLIENT_H_ */
