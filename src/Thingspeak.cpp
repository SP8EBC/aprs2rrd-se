/*
 * Thingspeak.cpp
 *
 *  Created on: Apr 21, 2024
 *      Author: mateusz
 */

#include "Thingspeak.h"
#include "TimeTools.h"

#include <iostream>
#include <iomanip>

#include "curl/curl.h"
#include "curl_code_to_str.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


const std::vector<std::locale> Thingspeak::formats(
	{
//	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%m/%d/%Y %H:%M")),
//	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%m/%d/%Y %I:%M%p")),
//	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%b %d, %Y %I:%M%p")),
	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%Y-%m-%dT%H:%M:%SZ"))
	}
);

uint64_t Thingspeak::decodeTimestampFromSubject(std::string &dateTime) {

	uint64_t out = 0u;

	boost::posix_time::ptime dateTimeFromSubject;

	// iterate through configured date/time formats
	for (auto format : formats)
	{
		std::istringstream is(dateTime);
		is.imbue(format);
		is >> dateTimeFromSubject;
		if (dateTimeFromSubject != boost::posix_time::ptime()) {
			out = TimeTools::getEpochFromPtime(dateTimeFromSubject, false);
			break;
		}
	}

	return out;
}

size_t Thingspeak::static_write_callback(char* get_data, size_t always_one,
		size_t get_data_size, void* userdata) {

	Thingspeak* class_ptr = static_cast<Thingspeak*>(userdata);

	class_ptr->write_callback(get_data, get_data_size * always_one);

	return get_data_size * always_one;
}

void Thingspeak::write_callback(char* data, size_t data_size) {
	if (this->ptr == this) {
		this->response.append(data, data_size);
	}
}

Thingspeak::Thingspeak(Thingspeak_Config &thingspeakConfig) : config(thingspeakConfig), lastDataTimestamp(0LL), downloadResult(false)
{
    std::stringstream urlBuilder;

	urlBuilder << "https://api.thingspeak.com/channels/" << config.channelId << "/feeds.json?results=1";

	this->uri = urlBuilder.str();

	ptr = this;
}

Thingspeak::~Thingspeak() {
	// TODO Auto-generated destructor stub
}

std::optional<std::string> Thingspeak::getKeyNameFromChannelObjByValue(std::string name)
{
	if (parsedResponse.contains("channel")) {
		nlohmann::json channel = parsedResponse["channel"];

		// iterate through content of 'channel' object to find any key
		// with value matching provided parameter. Found key name will be then used 
		// to get measurement value from array of 'feeds'
		for (auto it = channel.begin(); it != channel.end(); it++) {
			if (it.value() == name) {
				return std::optional(it.key());
			}
		}
	}

	return std::nullopt;
}

std::optional<std::string> Thingspeak::getKeyValueFromFeeds(std::string keyName) {
	if (parsedResponse.contains("feeds")) {

		nlohmann::json feeds = parsedResponse["feeds"];

		try {
			nlohmann::json zero = feeds.at(0);

			std::string value = zero[keyName];

			return std::optional(value);
		}
		catch (nlohmann::json::type_error & ex) {
			std::cout << "--- Thingspeak::getKeyValueFromFeeds:166 - type_error : " << ex.what() << std::endl;
			return std::nullopt;

		}
		catch (nlohmann::json::out_of_range & ex) {
			std::cout << "--- Thingspeak::getKeyValueFromFeeds:166 - out_of_range : " << ex.what() << std::endl;
			return std::nullopt;
		}
	}

	return std::nullopt;
}

bool Thingspeak::getLastDataTimestamp()
{
	bool result = false;

	if (parsedResponse.contains("feeds")) {

		nlohmann::json feeds = parsedResponse["feeds"];

		nlohmann::json zero = feeds[0];

		std::string createdAt = zero["created_at"];

		if (createdAt.length() > 0) {
			std::cout << "--- Thingspeak::getLastDataTimestamp:95 - value of 'created_at' field, read as string : " << createdAt << std::endl;

			uint64_t ts = decodeTimestampFromSubject(createdAt);

			std::cout << "--- Thingspeak::getLastDataTimestamp:99 - parsed date/time from 'created_at' field : " << boost::posix_time::to_simple_string(TimeTools::getPtimeFromEpoch(ts)) << std::endl;

			if (ts != 0) {
				result = true;

				this->lastDataTimestamp = ts;
			}
		}
	}

	return result;
}

bool Thingspeak::findAllFieldsNames()
{
	bool result = true;

    std::optional<std::string> temperatureField = getKeyNameFromChannelObjByValue(config.temperatureFieldName);
	if (temperatureField.has_value()) {
		temperatureKey = temperatureField.value();
	}
	else {
		std::cout << "--- Thingspeak::download:134 - WARNING - temperature field name cannot be found in 'channel' JSON object" << std::endl;
	}

	std::optional<std::string> averageWindspeedField = getKeyNameFromChannelObjByValue(config.averageWindspeedFieldName);
	if (averageWindspeedField.has_value()) {
		averageWindspeedKey = averageWindspeedField.value();
	}
	else {
		result = false;
		std::cout << "--- Thingspeak::download:134 - WARNING - average windspeed field name cannot be found in 'channel' JSON object" << std::endl;
	}

	std::optional<std::string> humidityField = getKeyNameFromChannelObjByValue(config.humidityFieldName);
	if (humidityField.has_value()) {
		humidityKey = humidityField.value();
	}
	else {
		result = false;
		std::cout << "--- Thingspeak::download:134 - WARNING - humidity field name cannot be found in 'channel' JSON object" << std::endl;
	}

	std::optional<std::string> maximumWindspeedField = getKeyNameFromChannelObjByValue(config.maximumWindspeedFieldName);
	if (maximumWindspeedField.has_value()) {
		maximumWindspeedKey = maximumWindspeedField.value();
	}
	else {
		result = false;
		std::cout << "--- Thingspeak::download:134 - WARNING - maximum windspeed field name cannot be found in 'channel' JSON object" << std::endl;
	}

	std::optional<std::string> pressureField = getKeyNameFromChannelObjByValue(config.pressureFieldName);
	if (pressureField.has_value()) {
		pressureKey = pressureField.value();
	}
	else {
		result = false;
		std::cout << "--- Thingspeak::download:134 - WARNING - pressure field name cannot be found in 'channel' JSON object" << std::endl;
	}

	std::optional<std::string> winddirectionField = getKeyNameFromChannelObjByValue(config.winddirectionFieldName);
	if (winddirectionField.has_value()) {
		winddirectionKey = winddirectionField.value();
	}
	else {
		result = false;
		std::cout << "--- Thingspeak::download:134 - WARNING - wind direction field name cannot be found in 'channel' JSON object" << std::endl;
	}

}

bool Thingspeak::download()
{
	bool out = false;
	
	CURLcode result = CURLcode::CURLE_OBSOLETE50;

    std::array<char, 128> header_string;

	auto curl = curl_easy_init();
    if (curl) {
    	std::cout << "--- Thingspeak::download:91 - Downloading data from " << this->uri << std::endl;


        curl_easy_setopt(curl, CURLOPT_URL, this->uri.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000L);

        header_string.fill(0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Thingspeak::static_write_callback);	// needs to be bind
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_string.data());

        result = curl_easy_perform(curl);

        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        std::cout << "--- Thingspeak::download:147 - response_code : " << boost::lexical_cast<std::string>(response_code) << std::endl;
        std::cout << "--- Thingspeak::download:148 - elapsed : " << boost::lexical_cast<std::string>(elapsed) << std::endl;
        std::cout << "--- Thingspeak::download:149 - result : " << curlCodeToStr(result) << std::endl;

        curl_easy_cleanup(curl);
        curl = NULL;

        if (result == CURLcode::CURLE_OK) {

        	std::cout << this->response << std::endl;

			try {
				parsedResponse = nlohmann::json::parse(this->response);

				// overall result of this operation depends on if 'created_at' timestamp could be
				// correctly read
				out = getLastDataTimestamp();

				out = findAllFieldsNames() ? out : false;
			}
			catch (nlohmann::json::parse_error & ex) {
				std::cout << "--- Thingspeak::download:166 - parse_error : " << ex.what() << std::endl;

				out = false;
			}
        }
        else {
        	out = false;
        }
	}

	downloadResult = out;

	return out;
}

void Thingspeak::getWeatherData(AprsWXData &out)
{
	const long howOldAreYou = TimeTools::getEpoch() - this->lastDataTimestamp;

	if (!downloadResult ) {
		std::cout << "--- Thingspeak::getWeatherData:184 - WARNING - nothing has been downloaded from the API yet!" << std::endl;
	}
	else if (howOldAreYou > this->config.runInterval) {
		std::cout << "--- Thingspeak::getWeatherData:184 - WARNING - data seems to be very old. Maybe station is not sending new data?" << std::endl;
	}
	else {
		out.valid = true;
		out.dataSource = WxDataSource::THINGSPEAK;

		// temperature
		std::string temperatureValStr = getKeyValueFromFeeds(temperatureKey).value();
		out.temperature = std::atof(temperatureValStr.c_str());
		out.useTemperature = true;

		// windspeed
		std::string windspeedValStr = getKeyValueFromFeeds(averageWindspeedKey).value();
		out.wind_speed = std::atof(windspeedValStr.c_str());
		out.useWind = true;

		// windgusts
		std::string windgustsValStr = getKeyValueFromFeeds(maximumWindspeedKey).value();
		out.wind_gusts = std::atof(windgustsValStr.c_str());
		out.useWind = true;

		// winddirection
		std::string winddirValStr = getKeyValueFromFeeds(winddirectionKey).value();
		out.wind_direction = (int)std::atof(winddirValStr.c_str());
		out.useWind = true;

		// pressure
		std::string pressureValStr = getKeyValueFromFeeds(pressureKey).value();
		out.pressure = std::atof(pressureValStr.c_str());
		out.usePressure = true;

		// humidity
		std::string humidityValStr = getKeyValueFromFeeds(humidityKey).value();
		out.humidity = std::atof(humidityValStr.c_str());
		out.useHumidity = true;

		std::cout << std::setprecision(3) <<  "--- Thingspeak::getWeatherData:184 - " <<
													"temperature: " << out.temperature << ", " <<
													"windspeed: " << out.wind_speed << ", " <<
													"windgusts: " << out.wind_gusts << ", " <<
													"winddirection: " << out.wind_direction << ", " <<
													std::endl;

	}

}
