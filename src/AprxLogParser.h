/*
 * AprxLogParser.h
 *
 * Class used to parse data from APRX rf-log file and get last (newest) wx packet from
 *
 *  Created on: Dec 3, 2022
 *      Author: mateusz
 */

#ifndef APRXLOGPARSER_H_
#define APRXLOGPARSER_H_

#include "AprxLogParserStaticStuff.h"

#include <string>
#include <cstdint>
#include <fstream>

#include "AprsWXData.h"

class AprxLogParser {

	uint16_t parsedLines;

    std::ifstream file;

	std::string fileName;

	const std::string APRSIS;

	bool timestampsAreInLocal;

public:

	void openFile();

	void closeFile();

	void rewindFile();

	std::optional<std::string> getNextLine(std::string call, uint8_t ssid, bool wxOnly);

	std::optional<AprsWXData> getLastPacketForStation(std::string call, uint8_t ssid);

	std::vector<AprsWXData> getAllWeatherPacketsInTimerange(boost::posix_time::ptime from, boost::posix_time::ptime to);
	std::vector<AprsWXData> getAllWeatherPacketsInTimerange(int64_t epochFrom, int64_t epochTo);
	std::vector<AprsWXData> getAllWeatherPacketsInTimerange(uint16_t agoFrom, uint16_t agoTo, bool hoursOrMinutes);

	std::vector<AprsPacket> getAllPacketsInTimerange(boost::posix_time::ptime from, boost::posix_time::ptime to);
	std::vector<AprsPacket> getAllPacketsInTimerange(int64_t epochFrom, int64_t epochTo);
	std::vector<AprsPacket> getAllPacketsInTimerange(uint16_t agoFrom, uint16_t agoTo, bool hoursOrMinutes);

	AprxLogParser();
	AprxLogParser(std::string fn, bool inLocal);
	virtual ~AprxLogParser();

	void setFileName(const std::string &fileName) {
		this->fileName = fileName;
	}

	static std::vector<AprsWXData> filterPacketsPerCallsign(std::string call, uint8_t ssid, std::vector<AprsWXData> & input) {

		std::vector<AprsWXData> output;

		for (AprsWXData & packet : input) {
			if (packet.call == call && packet.ssid == ssid) {
				output.push_back(packet);
			}
		}

		return output;
	}

	void setTimestampsAreInLocal(bool timestampsAreInLocal) {
		this->timestampsAreInLocal = timestampsAreInLocal;
	}
};

#endif /* APRXLOGPARSER_H_ */
