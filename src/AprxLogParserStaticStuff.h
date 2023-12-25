/*
 * AprxLogParserStaticStuff.h
 *
 *  Created on: Aug 21, 2023
 *      Author: mateusz
 */

#ifndef APRXLOGPARSERSTATICSTUFF_H_
#define APRXLOGPARSERSTATICSTUFF_H_

#include "AprsWXData.h"

#include <optional>
#include <boost/date_time.hpp>

class AprxLogParser_StaticStuff {
private:
	AprxLogParser_StaticStuff();
	virtual ~AprxLogParser_StaticStuff();

public:

	static const std::vector<std::locale> formats;

	static boost::posix_time::ptime convertToFrameTimestamp(std::string & date, std::string & time);

	/**
	 * Method which parse APRS frame from APRX rf-log separated to segments by spaces.
	 * It throws runtime exception if frame cannot be parsed
	 *
	 * @param separatedLines single aprx rf-log entry separated by spaces
	 * @param out
	 * @return
	 */
	static std::optional<AprsWXData> parseFrame(std::vector<std::string> & separatedLines, AprsPacket & out);

};

#endif /* APRXLOGPARSERSTATICSTUFF_H_ */
