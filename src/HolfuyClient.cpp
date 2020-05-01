/*
 * HolfuyClient.cpp
 *
 *  Created on: 29.04.2020
 *      Author: mateusz
 */

#include "HolfuyClient.h"
#include "XMLMemoryHandler.h"

#include <functional>
#include <array>
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

HolfuyClient::HolfuyClient(uint32_t id, std::string apiPassword) : stationId(id), apiPassword(apiPassword) {

	stationLocation = "";

	timestamp = boost::posix_time::microsec_clock::local_time();

	windspeed = 0.0f;

	windgusts = 0.0f;

	winddirection = 0;

	humidity = 0;

	pressure = 0;

	rain = 0;

	//result = CURLcode::CURLE_OBSOLETE50;

}

HolfuyClient::~HolfuyClient() {
}

void HolfuyClient::download() {
	// for now this is just a modification of an example from https://gist.github.com/whoshuu/2dc858b8730079602044

	CURLcode result = CURLcode::CURLE_OBSOLETE50;

    auto curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, this->getUrl().c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);

        std::array<char, 128> header_string;
        header_string.fill(0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &HolfuyClient::static_write_callback);	// needs to be bind
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_string.data());

        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        result = curl_easy_perform(curl);
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
	this->response.append(data, data_size);
}

void HolfuyClient::parse() {

	// Initializing the XML stuff in Xerces-C++ library
	xercesc_3_1::XMLPlatformUtils::Initialize();

	// casting the pointer to type using internally by Xerces
	const XMLByte* source = reinterpret_cast<const XMLByte*>(this->response.c_str());

	std::cout << this->response << std::endl;

	// Creating the DOM parser
	xercesc_3_1::XercesDOMParser* dom_parser = new xercesc_3_1::XercesDOMParser();

	// A source buffer for DOM parser
	xercesc_3_1::MemBufInputSource* memory_src = new xercesc_3_1::MemBufInputSource(
																	source,
																	this->response.length(),
																	"response",
																	false);

	// parsing the document from the memory
	dom_parser->parse(*memory_src);

	// retrieving the document
	xercesc_3_1::DOMDocument* document =  dom_parser->getDocument();

	// retrieving the root element. maybe SAX parser would be more convenient for
	// this purpose but anyway..
	xercesc_3_1::DOMElement* root = document->getDocumentElement();

	// start parsing on the tree
	this->parseElement(root);

	xercesc_3_1::XMLPlatformUtils::Terminate();

}

void HolfuyClient::parseElement(xercesc_3_1::DOMElement* element) {

	// decoding the node name
	char* node_name = xercesc_3_1::XMLString::transcode(element->getNodeName());

	std::cout << node_name << " - " << element->getNodeType() << " - " /*<< xercesc_3_1::XMLString::transcode(element->getNodeValue())*/ << std::endl;

	// checking if this is somehow interesting parameter
	this->checkAndRetrievieParameter(node_name, element);

	// retrieving a list of all child nodes
	xercesc_3_1::DOMNodeList* node_list = element->getChildNodes();

	// casting the list size (amount of child nodes)
	int nodes = static_cast<int>(node_list->getLength());

	// iterating through each of them
	for (int i = 0; i < nodes; i++) {

		// checking the child type
		xercesc_3_1::DOMNode::NodeType type = node_list->item(i)->getNodeType();

		// if this is an element node
		if (type == xercesc_3_1::DOMNode::NodeType::ELEMENT_NODE) {
			// retrievind it..
			xercesc_3_1::DOMElement* elem = dynamic_cast<xercesc_3_1::DOMElement*>( node_list->item(i));

			// ...and go deeper
			this->parseElement(elem);
		}
	}

}

void HolfuyClient::checkAndRetrievieParameter(char* node_name,
		xercesc_3_1::DOMElement* element) {
	if (strcmp(node_name, "wind") == 0) {
		XMLCh* windspeed_str = const_cast<XMLCh*>(element->getAttribute(xercesc_3_1::XMLString::transcode("speed")));
		XMLCh* windgust_str = const_cast<XMLCh*>(element->getAttribute(xercesc_3_1::XMLString::transcode("gust")));
		XMLCh* direction_str = const_cast<XMLCh*>(element->getAttribute(xercesc_3_1::XMLString::transcode("dir")));

		float windspeed_ = boost::lexical_cast<float>(xercesc_3_1::XMLString::transcode(windspeed_str));
		float windgusts_ = boost::lexical_cast<float>(xercesc_3_1::XMLString::transcode(windgust_str));
		float winddir_ = boost::lexical_cast<float>(xercesc_3_1::XMLString::transcode(direction_str));

		std::cout << "windspeed_ " << windspeed_ << ", windgusts_ " << windgusts_ << ", winddir_ " << winddir_ <<  std::endl;
	}
	else if (strcmp(node_name, "temp") == 0) {
		XMLCh* temperature_str = const_cast<XMLCh*>(element->getAttribute(xercesc_3_1::XMLString::transcode("temp")));

		float temperature_ = boost::lexical_cast<float>(xercesc_3_1::XMLString::transcode(temperature_str));

		std::cout << "temperature_ " << temperature_ << std::endl;
	}
	else if (strcmp(node_name, "pressure") == 0) {

	}
	else {
		;
	}

	return;
}

void HolfuyClient::getWxData(AprsWXData& out) {
}
