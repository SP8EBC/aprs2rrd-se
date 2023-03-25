/*
 * WeatherlinkClient.cpp
 *
 *  Created on: Mar 21, 2023
 *      Author: mateusz
 */

#include "WeatherlinkClient.h"

#include "curl/curl.h"
#include "curl_code_to_str.h"

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_facet.hpp>

/**
 * Static write callback used to pass to
 */
size_t WeatherlinkClient_WriteFunction(char* get_data, size_t always_one, size_t get_data_size, void* userdata) {
		WeatherlinkClient* class_ptr = static_cast<WeatherlinkClient*>(userdata);

		class_ptr->writeCallback(get_data, get_data_size * always_one);

		return get_data_size * always_one;
}


WeatherlinkClient::WeatherlinkClient() {
	downloadResult = false;

	// Initializing the XML stuff in Xerces-C++ library
	XMLPlatformUtils::Initialize();

	windspeed = 0.0f;
	winddir = 0.0f;
	windgusts = 0.0f;
	temperature = 0.0f;
	humidity = 0.0f;
	pressure = 0.0f;

	parsed = false;

	observationAge = 0;
}

void WeatherlinkClient::download() {

    std::array<char, 128> header_string;


    char* effectiveUrl;
    long response_code;

    std::string&& url = this->baseUrl + "?user=" + this->config.DID + "&pass=" + this->config.apiPassword + "&apiToken=" + this->config.apiToken;

	CURLcode result = CURLcode::CURLE_OBSOLETE50;

	// initialize cURL
    auto curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());		// set URL to query
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);			// shutdown progress meter and do not call progress function cbk
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");	// UserAgent identification
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);			// how many HTTP redirects are allowed before cURL will gave up
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);			// maximum HTTP transaction timeout in seconds
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000L);		// the same but in miliseconds ???? why doubled ??

    header_string.fill(0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WeatherlinkClient_WriteFunction);	// needs to be bind
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_string.data());

    // send HTTP request and wait for response
    result = curl_easy_perform(curl);

    // get all results
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effectiveUrl);

    std::cout << "--- WeatherlinkClient::download:65 - CURL response_code: " << boost::lexical_cast<std::string>(response_code) << std::endl;
    std::cout << "--- WeatherlinkClient::download:66 - CURL query elapsed time: " << boost::lexical_cast<std::string>(elapsed) << std::endl;
    std::cout << "--- WeatherlinkClient::download:67 - CURL result: " << curlCodeToStr(result) << std::endl;

    // deinitialize cURL
    curl_easy_cleanup(curl);
    curl = NULL;

    if (result == CURLcode::CURLE_OK) {
    	downloadResult = true;
    }
    else {
    	downloadResult = false;
    }

}

bool WeatherlinkClient::parse() {

	bool out = false;

	// if download wasn't successfull abort here
	if (!this->downloadResult)
		return out;

	// casting the pointer to type using internally by Xerces
	const XMLByte* source = reinterpret_cast<const XMLByte*>(this->responseBuffer.c_str());

	//std::cout << this->response << std::endl;

	// Creating the DOM parser
	std::unique_ptr<XercesDOMParser> dom_parser = std::make_unique<XercesDOMParser>(); //new XercesDOMParser();

	// A source buffer for DOM parser
	std::unique_ptr<MemBufInputSource> memory_src =	std::make_unique<MemBufInputSource>(
															source,
															this->responseBuffer.length(),
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
	this->downloadResult = true;

	return out;

}

void WeatherlinkClient::parseElement(DOMElement* element) {

	// read this: https://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html

	// retrieving a list of all child nodes
	DOMNodeList* node_list = element->getChildNodes();

	// casting the list size (amount of child nodes)
	int nodes = static_cast<int>(node_list->getLength());

	// get this node name
	char* node_name = XMLString::transcode(element->getNodeName());

	// iterating through each of them
	for (int i = 0; i < nodes; i++) {

		auto node = node_list->item(i);

		// checking the child type
		DOMNode::NodeType type = node->getNodeType();

		// if this is an element node
		if (type == DOMNode::NodeType::ELEMENT_NODE) {
			// retrievind it..
			DOMElement* elem = dynamic_cast<DOMElement*>( node);

			// ...and go deeper
			this->parseElement(elem);
		}
		else if (type == DOMNode::NodeType::ATTRIBUTE_NODE) {
			;
		}
		else if (type == DOMNode::NodeType::TEXT_NODE) {

			// checking if this is somehow interesting parameter
			this->checkAndRetrievieParameter(node_name, node);
		}
	}

}

void WeatherlinkClient::writeCallback(char *data, size_t data_size) {
	this->responseBuffer.append(data, data_size);
}

/**
 * copy weather data to universal adapter class
 */
bool WeatherlinkClient::getWxData(AprsWXData &out) {
	out.temperature = this->temperature;
	out.pressure = this->pressure;
	out.humidity = this->humidity;

	out.wind_direction = this->winddir;
	out.wind_speed = this->windspeed * 0.44f;
	out.wind_gusts = this->windgusts * 0.44f;

	// check if that data has been parsed OK and they are not too old
	if (this->parsed) {
		out.valid = true;
		out.useHumidity = true;
		out.useTemperature = true;
		out.useWind = true;
		out.usePressure = true;
	}
	else {
		out.valid = false;
		out.useHumidity = false;
		out.useTemperature = false;
		out.useWind = false;
		out.usePressure = false;
	}


	out.dataSource = WxDataSource::DAVIS;

	return true;
}

WeatherlinkClient::~WeatherlinkClient() {
	XMLPlatformUtils::Terminate();
}

/**
 * Check if this is parameter consist important measurement and parse it eventually
 */
void WeatherlinkClient::checkAndRetrievieParameter(char *node_name,
		DOMNode *element)
{

	char * text = XMLString::transcode(element->getNodeValue());

	if (strcmp(node_name, "pressure_mb") == 0) {
		this->pressure = boost::lexical_cast<float>(text);
	}

	else if (strcmp(node_name, "temp_c") == 0) {
		this->temperature = boost::lexical_cast<float>(text);
	}

	else if (strcmp(node_name, "wind_ten_min_avg_mph") == 0) {
		this->windspeed = boost::lexical_cast<float>(text);
	}

	else if (strcmp(node_name, "wind_ten_min_gust_mph") == 0) {
		this->windgusts = boost::lexical_cast<float>(text);
	}

	else if (strcmp(node_name, "wind_degrees") == 0) {
		this->winddir = boost::lexical_cast<float>(text);
	}

	else if (strcmp(node_name, "wind_degrees") == 0) {
		this->winddir = boost::lexical_cast<float>(text);
	}

	else if (strcmp(node_name, "relative_humidity") == 0) {
		this->humidity = boost::lexical_cast<float>(text);
	}

	else if (strcmp(node_name, "observation_age") == 0) {
		observationAge = boost::lexical_cast<float>(text);

		if (observationAge > maximumGoodObservationAge) {
			parsed = false;
		}
		else {
			parsed = true;
		}

		std::cout << "--- WeatherlinkClient::checkAndRetrievieParameter:283 - observationAge: " << observationAge << std::endl;


	}
	else if (strcmp(node_name, "observation_time_rfc822") == 0) {

		boost::posix_time::time_facet * timefacet = new boost::posix_time::time_facet();
		timefacet->format("%a, %d %b %Y %H:%M:%S %Z");

		std::stringstream ss;
		ss.str(text);
		ss.imbue(std::locale(std::locale::classic(), timefacet));
		boost::posix_time::ptime t;
		ss >> t;

		timestamp = t;

		std::cout << "--- WeatherlinkClient::checkAndRetrievieParameter:300 - timestamp: " << boost::posix_time::to_simple_string(t) << std::endl;


	}

}
