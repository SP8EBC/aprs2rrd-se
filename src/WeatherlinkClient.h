/*
 * WeatherlinkClient.h
 *
 *  Created on: Mar 21, 2023
 *      Author: mateusz
 */

#ifndef WEATHERLINKCLIENT_H_
#define WEATHERLINKCLIENT_H_

#include <boost/date_time/posix_time/posix_time.hpp>

#include <xercesc/dom/DOM.hpp>

#include "AprsWXData.h"

#ifdef USE_XERCES_3_2
using namespace xercesc_3_2;
#else
using namespace xercesc_3_1;
#endif

struct WeatherlinkClient_Config {
	std::string DID;
	std::string apiPassword;
	std::string apiToken;

	bool enable;
};

class WeatherlinkClient {

	/**
	 * URL to website with an information
	 */
	const std::string baseUrl = "https://api.weatherlink.com/v1/NoaaExt.xml";

	/**
	 * Response from HTTP server
	 */
	std::string responseBuffer;

	/**
	 * cURL http result
	 */
	bool downloadResult;

	float windspeed;
	float winddir;
	float windgusts;
	float temperature;
	float humidity;
	float pressure;

	boost::posix_time::ptime timestamp;

	/**
	 * Recursive method which goes through XML tree and check each node and element
	 * for interesting values
	 */
	void parseElement(DOMElement* element);

	/**
	 * Checker which looks up if given XML tree node has something interesting
	 */
	void checkAndRetrievieParameter(char* node_name, DOMNode* element);

	bool parsed;

public:

	/**
	 * Configuration of this module
	 */
	WeatherlinkClient_Config config;

	/**
	 * Downloads data from Weatherlink API
	 */
	void download();

	/**
	 * Parse response from Weatherlink to
	 */
	bool parse();

	/**
	 * copy weather data to universal adapter
	 */
	bool getWxData(AprsWXData & out);

	/**
	 * Write callback uset to store HTTP server response
	 */
	void writeCallback(char* data, size_t data_size);

	WeatherlinkClient();
	virtual ~WeatherlinkClient();

};

#endif /* WEATHERLINKCLIENT_H_ */
