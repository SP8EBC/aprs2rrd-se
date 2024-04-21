/*
 * Thingspeak.h
 *
 *  Created on: Apr 21, 2024
 *      Author: mateusz
 */

#ifndef THINGSPEAK_H_
#define THINGSPEAK_H_

#include <string>
#include <optional>
#include "TimeTools.h"
#include "AprsWXData.h"
#include "../lib/nlohman_json_3_7_1/include/nlohmann/json.hpp"

/// @brief 
struct Thingspeak_Config {
	bool enable;

	int channelId;

	/// @brief 
	std::string temperatureFieldName;

	/// @brief 
	std::string humidityFieldName;

	/// @brief 
	std::string pressureFieldName;

	/// @brief 
	std::string averageWindspeedFieldName;

	/// @brief 
	std::string maximumWindspeedFieldName;

	/// @brief 
	std::string winddirectionFieldName;

	/// @brief 
	int runInterval;
};


/// @brief 
class Thingspeak {

	enum STR2INT_ERROR { SUCCESS, OVERFLOW, UNDERFLOW, INCONVERTIBLE };

	static STR2INT_ERROR str2int (int &i, char const *s, int base = 0)
	{
		char *end;
		long  l;
		errno = 0;
		l = strtol(s, &end, base);
		if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
			return OVERFLOW;
		}
		if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
			return UNDERFLOW;
		}
		if (*s == '\0' || *end != '\0') {
			return INCONVERTIBLE;
		}
		i = l;
		return SUCCESS;
	}

	/// @brief vector of formatters used to parse date/time string in API response
	static const std::vector<std::locale> formats;

	static uint64_t decodeTimestampFromSubject(std::string &dateTime);

	/// @brief 
	/// @param get_data 
	/// @param always_one 
	/// @param get_data_size 
	/// @param userdata 
	/// @return 
	static size_t static_write_callback(char *get_data, size_t always_one, size_t get_data_size, void *userdata);

	/// @brief 
	/// @param data 
	/// @param data_size 
	void write_callback(char* data, size_t data_size);

	/// @brief 
	std::string temperatureKey;

	/// @brief 
	std::string humidityKey;

	/// @brief 
	std::string pressureKey;

	/// @brief 
	std::string averageWindspeedKey;

	/// @brief 
	std::string maximumWindspeedKey;

	/// @brief 
	std::string winddirectionKey;

	/// @brief 
	Thingspeak * ptr;

	/// @brief 
	Thingspeak_Config &config;

	/// @brief 
	std::string response;

	/// @brief 
	std::string uri;

	/// @brief 
	long lastDataTimestamp;

	/// @brief API response parsed by JSON library
	nlohmann::json parsedResponse;

	bool downloadResult;

	/// @brief 
	/// @param name 
	/// @param objectName 
	/// @return 
	std::optional<std::string> getKeyNameFromChannelObjByValue(std::string name);

	/// @brief 
	/// @param keyName 
	/// @return 
	std::optional<std::string> getKeyValueFromFeeds(std::string keyName);

	/// @brief uses @link{parsedResponse} to get a timestamp of data, value of "create_at"
	bool getLastDataTimestamp();

	bool findAllFieldsNames();

public:

	Thingspeak(Thingspeak_Config &thingspeakConfig);
	virtual ~Thingspeak();

	/// @brief 
	/// @return 
	bool download();

	/// @brief 
	/// @param out 
	void getWeatherData(AprsWXData & out);
};

#endif /* THINGSPEAK_H_ */
