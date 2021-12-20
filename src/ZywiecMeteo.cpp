/*
 * ZywiecMeteo.cpp
 *
 *  Created on: Dec 19, 2021
 *      Author: mateusz
 */

#include "ZywiecMeteo.h"

#include "curl/curl.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <string>
#include <array>

#include <boost/algorithm/string/replace.hpp>
#include <boost/locale.hpp>
#include <locale>

ZywiecMeteo::ZywiecMeteo(std::string _base_url) :baseUrl(_base_url), hour(1, 0, 0, 0) {
	// TODO Auto-generated constructor stub

	//formatter = std::make_unique<boost::posix_time::time_facet>("%Y-%m-%d %H:%M:%S");
	formatter = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S");

	ptr = this;

}

ZywiecMeteo::~ZywiecMeteo() {
	// TODO Auto-generated destructor stub
}

ZywiecMeteo::ZywiecMeteo(const ZywiecMeteo &other) {
	// TODO Auto-generated constructor stub

}

ZywiecMeteo::ZywiecMeteo(ZywiecMeteo &&other) {
	// TODO Auto-generated constructor stub

}

ZywiecMeteo& ZywiecMeteo::operator=(const ZywiecMeteo &other) {
	return const_cast<ZywiecMeteo&>(other);
	// TODO Auto-generated method stub

}

ZywiecMeteo& ZywiecMeteo::operator=(ZywiecMeteo &&other) {
	return other;
	// TODO Auto-generated method stub

}

bool ZywiecMeteo::downloadLastMeasureForStation(int stationId, std::string &response) {

	// endpoint: getLastMeasure

	bool out = false;

	// for now this is just a modification of an example from https://gist.github.com/whoshuu/2dc858b8730079602044

	CURLcode result = CURLcode::CURLE_OBSOLETE50;

	std::string url = this->baseUrl + "/server.php?action=getLastMeasure&deviceID=" + to_string(stationId);

    std::array<char, 128> header_string;

    auto curl = curl_easy_init();
    if (curl) {

    	std::cout << "--- ZywiecMeteo::downloadForStation:64 - Downloading data for stationId " << stationId << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);

        header_string.fill(0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ZywiecMeteo::static_write_callback);	// needs to be bind
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
        	out = true;

        	response = std::move(responseBuffer);
        }
        else {
        	out = false;
        }
    }

    return out;

}

bool ZywiecMeteo::downloadMeasuresFromRangeForStation(int stationId,
		std::string &respons) {

	bool out = false;

	// for now this is just a modification of an example from https://gist.github.com/whoshuu/2dc858b8730079602044

	//  * http://monitoring.zywiec.powiat.pl/server.php?action=getMeasuresFromRange&deviceID=509&fromDate=2021-12-19%2018%3A00%3A00&toDate=2021-12-19%2023%3A59%3A59&timeStep=10minutowy

	std::string response;

	std::stringstream urlBuilder;

	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	boost::posix_time::ptime past = now - hour;
	boost::posix_time::ptime future = now + hour;

	urlBuilder << this->baseUrl << "/server.php?action=getMeasuresFromRange&deviceID=" << to_string(stationId) << "&fromDate=";
	urlBuilder.imbue(std::locale(std::locale::classic(), formatter));
	urlBuilder << past << "&toDate=" << future << "&timeStep=10minutowy";

	CURLcode result = CURLcode::CURLE_OBSOLETE50;

	std::string url = urlBuilder.str();

	boost::algorithm::ireplace_all(url, " ", "%20");
	boost::algorithm::ireplace_all(url, ":", "%3A");
	boost::algorithm::ireplace_first(url, "%3A", ":");

    std::array<char, 128> header_string;

    char* effectiveUrl;
    long response_code;

    auto curl = curl_easy_init();
    if (curl) {

    	std::cout << "--- ZywiecMeteo::downloadForStation:64 - Downloading data for stationId " << stationId << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);

        header_string.fill(0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ZywiecMeteo::static_write_callback);	// needs to be bind
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_string.data());

        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effectiveUrl);

        result = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl = NULL;

        if (result == CURLcode::CURLE_OK) {
        	out = true;

        	respons = std::move(responseBuffer);
        }
        else {
        	out = false;
        }
    }

    return out;

}

void ZywiecMeteo::parseJson(std::string &in, AprsWXData &out) {

	nlohmann::json j = nlohmann::json::parse(in);

	std::string temporary;

	float temperature = 0.0f;
	float humidity = 0.0f;
	float windspeed = 0.0f;
	float windgust = 0.0f;
	float winddirection = 0.0f;

	if (j.contains("result")) {
		nlohmann::json arr = j["result"];

		nlohmann::json measObject = arr[0];

		if (measObject.contains("temp")) {
			temporary = measObject["temp"];
			temperature = ::stof(temporary, 0);
		}

		if (measObject.contains("humid")) {
			temporary = measObject["humid"];
			humidity = ::stof(temporary, 0);
		}
	}



}

size_t ZywiecMeteo::static_write_callback(char* get_data, size_t always_one,
		size_t get_data_size, void* userdata) {

	ZywiecMeteo* class_ptr = static_cast<ZywiecMeteo*>(userdata);

	class_ptr->write_callback(get_data, get_data_size * always_one);

	return get_data_size * always_one;
}

void ZywiecMeteo::write_callback(char* data, size_t data_size) {
	if (this->ptr == this) {
		this->responseBuffer.append(data, data_size);
	}
}
