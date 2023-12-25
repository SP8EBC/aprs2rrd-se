/*
 * AprxLogParserStaticStuff.cpp
 *
 *  Created on: Aug 21, 2023
 *      Author: mateusz
 */

#include "AprxLogParserStaticStuff.h"
#include "ReturnValues.h"

#include <algorithm>
#include <numeric>


const std::vector<std::locale> AprxLogParser_StaticStuff::formats(
	{
	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%Y-%m-%d %H:%M:%S")),
	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%Y/%m/%d %H:%M:%S")),
	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%d.%m.%Y %H:%M:%S")),
	std::locale(std::locale::classic(),new boost::posix_time::time_input_facet("%Y-%m-%d"))
	}
);

AprxLogParser_StaticStuff::AprxLogParser_StaticStuff() {
	// TODO Auto-generated constructor stub

}

AprxLogParser_StaticStuff::~AprxLogParser_StaticStuff() {
	// TODO Auto-generated destructor stub
}

boost::posix_time::ptime AprxLogParser_StaticStuff::convertToFrameTimestamp(
		std::string &date, std::string &time)
{
	boost::posix_time::ptime out;

	const std::string joined = date + " " + time;

	for (auto format : formats)
	{
        std::istringstream is(joined);
        is.imbue(format);
        is >> out;
        if (out != boost::posix_time::ptime()) {
        	break;
        }
	}

	return out;

}

std::optional<AprsWXData>  AprxLogParser_StaticStuff::parseFrame(std::vector<std::string> & separatedLines, AprsPacket & out) {

	std::string aprsFrame;

	// packet data created from an entry
	//AprsPacket aprsPacket;

	// value to be returned from this method
	AprsWXData ret;

	std::string lastLine;

	const std::string r = "R";
	const std::string d = "d";

	// look for 'R' across all results
	for (unsigned long i = 0; i < separatedLines.size(); i++) {

		// get current element
		const std::string & elem = separatedLines.at(i);

		if (elem == r || elem == d) {
			// check if this is the before-last element
			if (i == separatedLines.size() - 2) {
				aprsFrame = std::move(separatedLines.at(i + 1));
			}
			else {
				// there are more elements of APRS frame separated by space

				// glue all segments together
				aprsFrame = std::accumulate(separatedLines.begin() + i + 1, separatedLines.end(), std::string(""), [](std::string a, std::string b) {
								return std::move(a) + ' ' + std::move(b);
							});

				// trim any leading and trailing whitespaces inplace
				boost::algorithm::trim(aprsFrame);

			}

			break;
		}
	}

	// last element should consist frame
	lastLine = std::move(separatedLines.at(separatedLines.size() - 1));

	// try convert this do APRSpacket
	const int result = AprsPacket::ParseAPRSISData(aprsFrame.c_str(), aprsFrame.size(), &out);

	if (result == OK) {
		// get frame timestamp
		boost::posix_time::ptime timestamp = AprxLogParser_StaticStuff::convertToFrameTimestamp(separatedLines.at(0), separatedLines.at(1));

		// copy it to output object
		ret.packetTimestmp = timestamp;

		// calculate an age of this frame
		boost::posix_time::time_duration ageUniversal = boost::posix_time::second_clock::universal_time() - timestamp;
		boost::posix_time::time_duration ageLocal = boost::posix_time::second_clock::local_time() - timestamp;

		ret.packetAgeInSecondsLocal = ageLocal.total_seconds();
		ret.packetAgeInSecondsUtc = ageUniversal.total_seconds();

		// extract WX data
		const int resultWx = AprsWXData::ParseData(out, &ret);

		if (resultWx == 0) {
			// data has been parsed correctly
			return ret;
		}
		else {
			// data hasn't been prased, maybe this isn't weather frame?
			return std::nullopt;
		}
	}
	else {
		throw std::runtime_error("This isn't valid APRX rf log entry!");
	}
}
