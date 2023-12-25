/*
 * AprxLogParser.cpp
 *
 *  Created on: Dec 3, 2022
 *      Author: mateusz
 */

#include "AprxLogParser.h"
#include "ReturnValues.h"
#include "TimeTools.h"


#include <boost/algorithm/string.hpp>
#include <sstream>
#include <exception>

#pragma push_macro("U")
#undef U
// pragma required as a workaround of possible conflict with cpprestsdk.
// more info here: https://github.com/fmtlib/fmt/issues/3330
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#pragma pop_macro("U")


#define SOURCE_OFFSET		24
#define RTD_OFFSET			34
#define CALLSIGN_OFFSET		36
#define MINIMAL_LINE_LN		45

#define SOURCE_LN			6

AprxLogParser::AprxLogParser(std::string fn, bool inLocal) : fileName(fn), APRSIS("APRSIS"), timestampsAreInLocal(inLocal) {
	parsedLines = 0;
}

AprxLogParser::AprxLogParser() : APRSIS("APRSIS") {
	parsedLines = 0;
	timestampsAreInLocal = false;

}

AprxLogParser::~AprxLogParser() {
}

std::vector<AprsWXData> AprxLogParser::getAllWeatherPacketsInTimerange(
		boost::posix_time::ptime from, boost::posix_time::ptime to) {

	// convert to epoch
	const long fromEpoch = TimeTools::getEpochFromPtime(from, timestampsAreInLocal);
	const long toEpoch = TimeTools::getEpochFromPtime(to, timestampsAreInLocal);

	return this->getAllWeatherPacketsInTimerange(fromEpoch, toEpoch);
}

std::vector<AprsWXData> AprxLogParser::getAllWeatherPacketsInTimerange(
		int64_t epochFrom, int64_t epochTo) {

	std::vector<AprsWXData> out;

	// local buffer to extract data from log file
	std::string localBuffer;

	AprsPacket aprsPacket;

	// single line of aprx rf log separated to components by space
	std::vector<std::string> separated;

	// if at least one line has been found
	bool hasBeenFound = false;

	// epoch timestmp of current line
	long epochFromLine;

	// check if file has been opened (and used) previously
	//if (file.is_open()) {
		this->closeFile();

		this->openFile();

		this->rewindFile();
	//}

	if (file.is_open()) {

		// read line by line
		while (std::getline(file, localBuffer)) {

			// parse line
			boost::split(separated, localBuffer, boost::is_any_of(" "));

			// check if result has a meaningfull size
			if (separated.size() > 4) {

				// get frame timestamp
				boost::posix_time::ptime timestamp = AprxLogParser_StaticStuff::convertToFrameTimestamp(separated.at(0), separated.at(1));

				// convert into epoch
				epochFromLine = TimeTools::getEpochFromPtime(timestamp, timestampsAreInLocal);

				// check if this line fits within given range
				if (epochFrom < epochFromLine && epochTo > epochFromLine) {
					try {
						std::optional<AprsWXData> weatherFrame = AprxLogParser_StaticStuff::parseFrame(separated, aprsPacket);

						// check if a weather data has been found in this line
						if (weatherFrame.has_value()) {
							// if yes add this to output vector
							out.push_back(weatherFrame.value());

							if (!hasBeenFound) {
								SPDLOG_DEBUG("Range has begun");
							}

							// and set a flag that we have entered into a range
							hasBeenFound = true;
						}
					}
					catch (std::runtime_error & ex) {
						SPDLOG_WARN("Runtime error thrown during parsing APRX rf log entry");
						SPDLOG_WARN(localBuffer);
					}

				}
				else {
					// check if at least one frame has been found
					if (hasBeenFound) {
						break;	// exit the loop because we are after the range
					}
					else {
						;	// continue further as
					}
				}
			}
		}

	}
	else {
		SPDLOG_ERROR("APRX rf log file is not opened!");
	}

	SPDLOG_DEBUG("Last epochFromLine: {}, epochFrom: {}, epochTo: {}", epochFromLine, epochFrom, epochTo);

	return out;
}

std::vector<AprsWXData> AprxLogParser::getAllWeatherPacketsInTimerange(uint16_t agoFrom,
		uint16_t agoTo, bool hoursOrMinutes) {

	// get current epoch
	const long currentEpoch = TimeTools::getEpoch();

	// get time to look for packet from
	const long epochFrom = currentEpoch - agoFrom * (hoursOrMinutes ? 60 : 3600);

	// get time to look for packet to
	const long epochTo = currentEpoch - agoTo * (hoursOrMinutes ? 60 : 3600);

	SPDLOG_DEBUG("epochFrom: {}, epochTo: {}", epochFrom, epochTo);

	return this->getAllWeatherPacketsInTimerange(epochFrom, epochTo);
}

std::optional<AprsWXData> AprxLogParser::getLastPacketForStation(std::string call,
		uint8_t ssid) {

	std::optional<AprsWXData> out;

	AprsPacket packet;

	// value returned from method which looks across a log for next entry for given station
	std::optional<std::string> line;

	// the last log entry for given station
	std::string lastLine;

	// line separated by space to timestamp and rest of things
	std::vector<std::string> seprated;

	//if (file.is_open()) {
		this->closeFile();

		this->openFile();

		this->rewindFile();
	//}

	// iterate through file
	do {
		// copy previous line
		lastLine = line.value_or("");

		// get next line
		line = getNextLine(call, ssid, true);
	} while(line.has_value());

	// check if any line has been found
	if (lastLine.length() > 1) {
		// parse line
		boost::split(seprated, lastLine, boost::is_any_of(" "));

		if (seprated.size() > 4) {

			try {
				// parse line read from aprx rf log
				std::optional<AprsWXData> parsedFrame = AprxLogParser_StaticStuff::parseFrame(seprated, packet);

				// check if this line contains valid WX data
				if (parsedFrame.has_value()) {
					// if yes return it
					out = parsedFrame.value();
				}
				else {
					// if not maybe this isn't
					out = std::nullopt;
					SPDLOG_WARN("APRX rf log doesn't contain any weather data for callsign {}", call);
				}
			}
			catch (std::runtime_error & ex) {
				SPDLOG_WARN("Runtime error thrown during parsing APRX rf log entry");
				SPDLOG_WARN(lastLine);
			}
		}

	}
	else {
		// nothing found for that callsign in that APRX log file
		out = std::nullopt;
		SPDLOG_WARN("APRX rf log doesn't contain any weather data for callsign {}", call);

	}

    return out;

}

std::optional<std::string> AprxLogParser::getNextLine(std::string call,
		uint8_t ssid, bool wxOnly) {

	// local buffer for string line
	std::string buffer;

	if (file.is_open()) {

		// read line by line
		while (std::getline(file, buffer)) {

			// local buffer for packet source
			std::string bufferSource;

			// local buffer to extract callsign
			std::string bufCallsign;

			int _ssid = -1;

			// local buffer for SSID
			std::string bufSSID;

			//
			char rtd = ' ';

			// check line length
			if (buffer.length() > MINIMAL_LINE_LN) {

				// get R/T/d indicator
				rtd = buffer.at(RTD_OFFSET);

				// continue to next line if this is not received packet
				if (rtd != 'R' && rtd != 'd') {
					continue;
				}

				// copy source
				bufferSource = buffer.substr(SOURCE_OFFSET, SOURCE_LN);

				// check source
				if (bufferSource.compare(APRSIS) == 0) {
					continue;
				}

				// get iterator to beginning of string
				auto it = buffer.begin();

				// rewind to beginning of a callsign
				it += CALLSIGN_OFFSET;

				// copy callsign to separate buffer
				while ( (*it != '-') && (*it != '>') && it != buffer.end()) {
					bufCallsign.append(1, *it);

					it++;
				}

				// check SSID
				if (*it == '-') {
					// copy if exist
					it++;

					while ((*it != '>') && it != buffer.end()) {
						bufSSID.append(1, *it);

						it++;
					}

					try {
						// and convert to integer
						_ssid = std::stoi(bufSSID);
					}
					catch(std::invalid_argument & ex) {
						SPDLOG_ERROR("std::invalid_argument thrown while pasing source SSID");
						SPDLOG_ERROR(buffer);
						continue;
					}
				}
				else {
					_ssid = 0;
				}

				// optional check
				if (wxOnly) {
					// detect if this is a weather frame
					// although this is not a perfect way of detecting
					// type of frame. non-wx frame may consist
					// '!' or '@' in a content (payload), but
					// weather frame will ALWAYS consist '!' or '@'
					if (buffer.find("!") != std::string::npos) {
						;	// it may be wx frame, do nothing
					}
					else if (buffer.find("@")!= std::string::npos) {
						;	// the same
					}
					else {
						continue;	// this is not wx frame
					}
				}

				// check ig callsign contains '*' at the begining
				if (bufCallsign.at(0) == '*') {
					bufCallsign.erase(0, 1);
				}

				// continue to next line if this is not a callsign we're looking for
				if (bufCallsign.compare(call) != 0) {
					continue;
				}
				else {
					// check SSID
					if (_ssid == ssid) {
						return buffer;
					}
					else {
						continue;
					}
				}
			}
		}
	}
	else {
		SPDLOG_ERROR("APRX log file is not opened! Check if path provided in configuration is correct");

		throw std::runtime_error("APRX log file is not opened!");
	}

	return {};

}

void AprxLogParser::closeFile() {
	file.close();
}

void AprxLogParser::openFile() {
	SPDLOG_INFO("Opening log file from path: {}", fileName);

	// open file for reading
    file = std::ifstream(fileName);
}

void AprxLogParser::rewindFile() {
	// rewind to the end
	file.seekg(0, file.end);

	// get buffer lenght
	std::streampos lenght = file.tellg();

	// ???
	file.seekg((int)lenght / 2, file.beg);
}

std::vector<AprsPacket> AprxLogParser::getAllPacketsInTimerange(
		int64_t epochFrom, int64_t epochTo) {

	std::vector<AprsPacket> out;

	// local buffer to extract data from log file
	std::string localBuffer;

	// single line of aprx rf log separated to components by space
	std::vector<std::string> separated;

	// if at least one line has been found
	bool hasBeenFound = false;

	// epoch timestmp of current line
	long epochFromLine;

	// check if file has been opened (and used) previously
	this->closeFile();

	this->openFile();

	this->rewindFile();

	if (file.is_open()) {

		// read line by line
		while (std::getline(file, localBuffer)) {

			// parse line
			boost::split(separated, localBuffer, boost::is_any_of(" "));

			// check if result has a meaningfull size
			if (separated.size() > 4) {

				// get frame timestamp
				boost::posix_time::ptime timestamp = AprxLogParser_StaticStuff::convertToFrameTimestamp(separated.at(0), separated.at(1));

				// convert into epoch
				epochFromLine = TimeTools::getEpochFromPtime(timestamp, timestampsAreInLocal);

				// check if this line fits within given range
				if (epochFrom < epochFromLine && epochTo > epochFromLine) {
					try {
						AprsPacket aprsPacket;

						std::optional<AprsWXData> weatherFrame = AprxLogParser_StaticStuff::parseFrame(separated, aprsPacket);

						// if yes add this to output vector
						out.push_back(aprsPacket);

						if (!hasBeenFound) {
							SPDLOG_DEBUG("Range has begun");
						}

						// and set a flag that we have entered into a range
						hasBeenFound = true;
					}
					catch (std::runtime_error & ex) {
						SPDLOG_WARN("Runtime error thrown during parsing APRX rf log entry");
						SPDLOG_WARN(localBuffer);
					}

				}
				else {
					// check if at least one frame has been found
					if (hasBeenFound) {
						break;	// exit the loop because we are after the range
					}
					else {
						;	// continue further as
					}
				}
			}
		}

	}
	else {
		SPDLOG_ERROR("APRX rf log file is not opened!");
	}

	SPDLOG_DEBUG("Last epochFromLine: {}, epochFrom: {}, epochTo: {}", epochFromLine, epochFrom, epochTo);

	return out;
}

std::vector<AprsPacket> AprxLogParser::getAllPacketsInTimerange(
		boost::posix_time::ptime from, boost::posix_time::ptime to) {

	// convert to epoch
	const long fromEpoch = TimeTools::getEpochFromPtime(from, timestampsAreInLocal);
	const long toEpoch = TimeTools::getEpochFromPtime(to, timestampsAreInLocal);

	return this->getAllPacketsInTimerange(fromEpoch, toEpoch);
}

std::vector<AprsPacket> AprxLogParser::getAllPacketsInTimerange(
		uint16_t agoFrom, uint16_t agoTo, bool hoursOrMinutes) {

	// get current epoch
	const long currentEpoch = TimeTools::getEpoch();

	// get time to look for packet from
	const long epochFrom = currentEpoch - agoFrom * (hoursOrMinutes ? 60 : 3600);

	// get time to look for packet to
	const long epochTo = currentEpoch - agoTo * (hoursOrMinutes ? 60 : 3600);

	SPDLOG_DEBUG("epochFrom: {}, epochTo: {}", epochFrom, epochTo);

	return this->getAllPacketsInTimerange(epochFrom, epochTo);
}
