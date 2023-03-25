/*
 * HolfuyClient.cpp
 *
 *  Created on: 29.04.2020
 *      Author: mateusz
 */

#include "HolfuyClient.h"
#include "XMLMemoryHandler.h"

#include <functional>
#include <iostream>

#include <curl/curl.h>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <boost/lexical_cast.hpp>

#include "curl_code_to_str.h"


HolfuyClient::HolfuyClient(uint32_t id, std::string apiPassword) : stationId(id), apiPassword(apiPassword) {

	stationLocation = "";

	timestamp = boost::posix_time::microsec_clock::local_time();

	windspeed = 0.0f;

	windgusts = 0.0f;

	winddirection = 0;

	humidity = 0;

	pressure = 0;

	rain = 0;

	temperature = 0.0f;

	response.reserve(255);

	// Initializing the XML stuff in Xerces-C++ library
	XMLPlatformUtils::Initialize();

	dateCh = XMLString::transcode("date");

	timeCh = XMLString::transcode("time");

	windspeedCh = XMLString::transcode("speed");

	windgustsCh = XMLString::transcode("gust");

	winddirCh = XMLString::transcode("dir");

	pressureCh = XMLString::transcode("pressure");

	temperatureCh = XMLString::transcode("temp");

    header_string.fill(0);

    header_string_data = header_string.data();

    ptr = this;

}

HolfuyClient::~HolfuyClient() {
	XMLPlatformUtils::Terminate();

}

void HolfuyClient::download() {
	// for now this is just a modification of an example from https://gist.github.com/whoshuu/2dc858b8730079602044

	CURLcode result = CURLcode::CURLE_OBSOLETE50;

    auto curl = curl_easy_init();
    if (curl) {

    	std::cout << "--- HolfuyClient::download:81 - Downloading data from the API" << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, this->getUrl().c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);

        header_string.fill(0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &HolfuyClient::static_write_callback);	// needs to be bind
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_string_data);

        result = curl_easy_perform(curl);

        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        std::cout << "--- HolfuyClient::download:117 - response_code : " << boost::lexical_cast<std::string>(response_code) << std::endl;
        std::cout << "--- HolfuyClient::download:118 - elapsed : " << boost::lexical_cast<std::string>(elapsed) << std::endl;
        std::cout << "--- HolfuyClient::download:119 - result : " << curlCodeToStr(result) << std::endl;

        curl_easy_cleanup(curl);
        curl = NULL;

        if (result == CURLcode::CURLE_OK) {
        	downloadResult = true;
        }
        else {
        	downloadResult = false;
        }
    }
}

size_t HolfuyClient::static_write_callback(char* get_data, size_t always_one,
		size_t get_data_size, void* userdata) {

	HolfuyClient* class_ptr = static_cast<HolfuyClient*>(userdata);

	class_ptr->write_callback(get_data, get_data_size * always_one);

	return get_data_size * always_one;
}

void HolfuyClient::write_callback(char* data, size_t data_size) {
	if (this->ptr == this) {
		this->response.append(data, data_size);
	}
}

void HolfuyClient::parse() {

	// if download wasn't successfull abort here
	if (!this->downloadResult)
		return;

	// casting the pointer to type using internally by Xerces
	const XMLByte* source = reinterpret_cast<const XMLByte*>(this->response.c_str());

	std::cout << this->response << std::endl;

	// Creating the DOM parser
	XercesDOMParser* dom_parser = new XercesDOMParser();

	// A source buffer for DOM parser
	MemBufInputSource* memory_src = new MemBufInputSource(
															source,
															this->response.length(),
															"response",
															false);

	// parsing the document from the memory
	dom_parser->parse(*memory_src);

	// retrieving the document
	DOMDocument* document =  dom_parser->getDocument();

	// retrieving the root element. maybe SAX parser would be more convenient for
	// this purpose but anyway..
	DOMElement* root = document->getDocumentElement();

	// start parsing on the tree
	this->parseElement(root);

	// clear the buffer
	this->response = "";
	this->downloadResult = true;

	delete memory_src;
	delete dom_parser;

}

void HolfuyClient::parseElement(DOMElement* element) {

	// decoding the node name
	char* node_name = XMLString::transcode(element->getNodeName());

	// checking if this is somehow interesting parameter
	this->checkAndRetrievieParameter(node_name, element);

	// retrieving a list of all child nodes
	DOMNodeList* node_list = element->getChildNodes();

	// casting the list size (amount of child nodes)
	int nodes = static_cast<int>(node_list->getLength());

	// iterating through each of them
	for (int i = 0; i < nodes; i++) {

		// checking the child type
		DOMNode::NodeType type = node_list->item(i)->getNodeType();

		// if this is an element node
		if (type == DOMNode::NodeType::ELEMENT_NODE) {
			// retrievind it..
			DOMElement* elem = dynamic_cast<DOMElement*>( node_list->item(i));

			// ...and go deeper
			this->parseElement(elem);
		}
	}

}

void HolfuyClient::checkAndRetrievieParameter(char* node_name,
		DOMElement* element) {
	if (strcmp(node_name, "timestamp") == 0) {
		XMLCh* date = const_cast<XMLCh*>(element->getAttribute(dateCh));
		XMLCh* time = const_cast<XMLCh*>(element->getAttribute(timeCh));

		char* decoded_date = XMLString::transcode(date);
		char* decoded_time = XMLString::transcode(time);

		std::string date_time_string = std::string(decoded_date) + " " + std::string(decoded_time);

		boost::posix_time::ptime timestamp = boost::posix_time::time_from_string(date_time_string);

		this->timestamp = timestamp;

		XMLString::release(&decoded_date);
		XMLString::release(&decoded_time);


	}
	else if (strcmp(node_name, "wind") == 0) {
		XMLCh* windspeed_str = const_cast<XMLCh*>(element->getAttribute(windspeedCh));
		XMLCh* windgust_str = const_cast<XMLCh*>(element->getAttribute(windgustsCh));
		XMLCh* direction_str = const_cast<XMLCh*>(element->getAttribute(winddirCh));

		float windspeed_ = boost::lexical_cast<float>(XMLString::transcode(windspeed_str));
		float windgusts_ = boost::lexical_cast<float>(XMLString::transcode(windgust_str));
		float winddir_ = boost::lexical_cast<float>(XMLString::transcode(direction_str));

		this->windspeed = windspeed_;
		this->windgusts = windgusts_;
		this->winddirection = winddir_;


		//std::cout << "windspeed_ " << windspeed_ << ", windgusts_ " << windgusts_ << ", winddir_ " << winddir_ <<  std::endl;
	}
	else if (strcmp(node_name, "temp") == 0) {
		XMLCh* temperature_str = const_cast<XMLCh*>(element->getAttribute(temperatureCh));

		float temperature_ = boost::lexical_cast<float>(XMLString::transcode(temperature_str));

		this->temperature = temperature_;

		//std::cout << "temperature_ " << temperature_ << std::endl;
	}
	else if (strcmp(node_name, "pressure") == 0) {
		XMLCh* pressure_str = const_cast<XMLCh*>(element->getAttribute(pressureCh));

		float pressure_ = boost::lexical_cast<float>(XMLString::transcode(pressure_str));

		this->pressure = pressure_;

		//std::cout << "pressure_ " << pressure_ << std::endl;

	}
	else {
		;
	}

	return;
}

bool HolfuyClient::getWxData(AprsWXData& out) {
	if (!this->downloadResult) {
		out.useHumidity = false;
		out.usePressure = false;
		out.useTemperature = false;
		out.useWind = false;

		out.valid = false;

		return false;
	}

	out.wind_speed = this->windspeed;
	out.wind_gusts = this->windgusts;
	out.wind_direction = this->winddirection;
	out.pressure = this->pressure;
	out.temperature = this->temperature;
	out.pressure = this->pressure;

	out.useHumidity = false;
	out.usePressure = true;
	out.useTemperature = true;
	out.useWind = true;

	out.valid = true;
	out.dataSource = WxDataSource::HOLFUY;

	this->downloadResult = false;

	return true;
}
