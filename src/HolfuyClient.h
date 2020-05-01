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

#include <xercesc/dom/DOM.hpp>

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

	static size_t static_write_callback(char *get_data, size_t always_one, size_t get_data_size, void *userdata);

	void write_callback(char* data, size_t data_size);

	bool downloadResult = false;

	std::string response;

	std::string apiUrl;

	std::string getUrl() {

		this->apiUrl = "http://api.holfuy.com/live/?s=" + std::to_string((int)stationId) + "&pw=" + apiPassword +  "&m=XML&tu=C&su=m/s";

		return this->apiUrl;
	}

	void parseElement(xercesc_3_1::DOMElement* element);

	void checkAndRetrievieParameter(char* node_name, xercesc_3_1::DOMElement* element);

public:
	HolfuyClient(uint32_t id, std::string apiPassword);
	~HolfuyClient();

	// this method will download the data from Holfuy API
	void download();

	// this will parse the XML from Holfiu API and then
	void parse();

	// move that data to AprsWXData object
	void getWxData(AprsWXData & out);
};

#endif /* HOLFUYCLIENT_H_ */
