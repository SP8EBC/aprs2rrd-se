/*
 * HolfuyClient.cpp
 *
 *  Created on: 29.04.2020
 *      Author: mateusz
 */

#include "HolfuyClient.h"

#include <functional>
#include <curl/curl.h>


HolfuyClient::HolfuyClient(uint32_t id, std::string apiPassword) : stationId(id), apiPassword(apiPassword) {

	stationLocation = "";

	timestamp = boost::posix_time::microsec_clock::local_time();

	windspeed = 0.0f;

	windgusts = 0.0f;

	winddirection = 0;

	humidity = 0;

	pressure = 0;

	rain = 0;

}

HolfuyClient::~HolfuyClient() {
}

void HolfuyClient::download() {
	// for now this is just a modification of an example from https://gist.github.com/whoshuu/2dc858b8730079602044

	auto callback = std::bind(&HolfuyClient::write_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    auto curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, this->getUrl());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);	// needs to be bind
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl = NULL;
    }
}

size_t HolfuyClient::write_callback(char* get_data, size_t always_one,
		size_t get_data_size, void* userdata) {
}

void HolfuyClient::getWxData(AprsWXData& out) {
}
