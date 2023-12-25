/*
 * TimeTools.h
 *
 *  Created on: Apr 8, 2023
 *      Author: mateusz
 */

#ifndef TIMETOOLS_H_
#define TIMETOOLS_H_

#include <boost/date_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include <time.h>

// Some time zones (in minutes)
enum TimeTools_TimeZones {

	GMT_12 = -720,   // GMT-12h
	GMT_11 = -660,   // GMT-11h
	GMT_10 = -600,   // GMT-10h
	GMT_9 = -540,    // GMT-9h
	GMT_8 = -480,    // GMT-8h
	GMT_7 = -420,    // GMT-7h
	GMT_6 = -360,    // GMT-6h
	GMT_5 = -300,    // GMT-5h
	GMT_4 = -240,    // GMT-4h
	GMT_3 = -180,    // GMT-3h
	GMT_2 = -120,    // GMT-2h
	GMT_1 = -60,     // GMT-1h
	GMT = 0,         // GMT
	GMT1 = 60,       // GMT+1h
	GMT2 = 120,      // GMT+2h
	GMT3 = 180,      // GMT+3h
	GMT4 = 240,      // GMT+4h
	GMT5 = 300,      // GMT+5h
	GMT6 = 360,      // GMT+6h
	GMT7 = 420,      // GMT+7h
	GMT8 = 480,      // GMT+8h
	GMT9 = 540,      // GMT+9h
	GMT10 = 600,     // GMT+10h
	GMT11 = 660,     // GMT+11h
	GMT12 = 720,     // GMT+12h
};

class TimeTools {
private:
	static boost::local_time::tz_database timezones;

	static bool timezonesInit;

public:

	static void initBoostTimezones() {
		TimeTools::timezones.load_from_file("date_time_zonespec.csv");

		TimeTools::timezonesInit = true;

	}

	static void initBoostTimezones(std::string fn) {
		TimeTools::timezones.load_from_file(fn);

		TimeTools::timezonesInit = true;

	}

	static long getEpoch() {
		// get currrent timestamp
		boost::posix_time::ptime current = boost::posix_time::second_clock::universal_time();

		// epoch
		boost::posix_time::ptime epoch = boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::time_duration(0,0,0,0));

		// timestamp
		long ts = (current - epoch).total_seconds();

		return ts;
	}

	/**
	 *
	 * @param in ptime to convert to epoch timestamp
	 * @param universalOrLocal set to true if ptime contains time in local timezone instead of UTC
	 */
	static long getEpochFromPtime(boost::posix_time::ptime & in, bool universalOrLocal) {

		// get current local and utc time
		boost::posix_time::ptime currentUniversal = boost::posix_time::second_clock::universal_time();
		boost::posix_time::ptime currentLocal = boost::posix_time::second_clock::local_time();

		// calculate current TZ offset in second
		boost::posix_time::time_duration offset = currentLocal - currentUniversal;
		const long seconds = offset.total_seconds();

		// epoch
		boost::posix_time::ptime epoch = boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::time_duration(0,0,0,0));

		// timestamp
		long ts = (in - epoch).total_seconds();

		// if a input ptime constains local time
		if (universalOrLocal) {
			// but we subtracted it from January 1st 1970 UTC, not a local tz

			// so we need to subtract timezone offset
			ts = ts - seconds;
		}

		return ts;
	}

	static boost::posix_time::ptime getPtimeFromEpoch (uint64_t epoch) {

		std::time_t _time = (std::time_t)epoch;

		return boost::posix_time::from_time_t(_time);
	}

	static long getEpochFromBoostLocalTime(const boost::local_time::local_date_time & in) {

		boost::posix_time::ptime utc = in.utc_time();

		long ts = TimeTools::getEpochFromPtime(utc, false);

		return ts;
	}

	/**
	 * Converts time from 'tm' structure type into boost local_date_time, taking into account sone offset. 'tm' could contains
	 * either local or utc time. Boost c++ assumes internally that ptime given as a parameter to local_date_time constructors
	 * contains UTC time (hence ptime name), so if 'date' contains local time an adjustment is done internally by this method
	 * to obtain correct date and time with matching timezone. To work correctly this require custom date_time_zonespec.csv
	 * file be present in ParaGADACZ working directory. Please remember that this methog operates on UTC offset, not a timezone
	 * so it may not handle DST correctly.
	 * @param date
	 * @param dateIsLocalTime The default assumption is that 'date' contains UTC time, set this to true if this is local one
	 * @param offsetInMinsFromUtc Offset in minutes from UTC
	 * @return
	 */
	static boost::local_time::local_date_time getLocalTimeFromTmStructAndTzOffset(tm & date, bool dateIsLocalTime, int offsetInMinsFromUtc) {

		boost::local_time::local_date_time out(boost::local_time::pos_infin);

		boost::local_time::time_zone_ptr timezone;

		if (timezonesInit) {
			switch (offsetInMinsFromUtc) {
				case GMT_12: timezone = TimeTools::timezones.time_zone_from_region("UTC-12"); break;
				case GMT_11: timezone = TimeTools::timezones.time_zone_from_region("UTC-11"); break;
				case GMT_10: timezone = TimeTools::timezones.time_zone_from_region("UTC-10"); break;
				case GMT_9: timezone = TimeTools::timezones.time_zone_from_region("UTC-9"); break;
				case GMT_8: timezone = TimeTools::timezones.time_zone_from_region("UTC-8"); break;
				case GMT_7: timezone = TimeTools::timezones.time_zone_from_region("UTC-7"); break;
				case GMT_6: timezone = TimeTools::timezones.time_zone_from_region("UTC-6"); break;
				case GMT_5: timezone = TimeTools::timezones.time_zone_from_region("UTC-5"); break;
				case GMT_4: timezone = TimeTools::timezones.time_zone_from_region("UTC-4"); break;
				case GMT_3: timezone = TimeTools::timezones.time_zone_from_region("UTC-3"); break;
				case GMT_2: timezone = TimeTools::timezones.time_zone_from_region("UTC-2"); break;
				case GMT_1: timezone = TimeTools::timezones.time_zone_from_region("UTC-1"); break;
				case GMT: timezone = TimeTools::timezones.time_zone_from_region("UTC"); break;
				case GMT1: timezone = TimeTools::timezones.time_zone_from_region("UTC+1"); break;
				case GMT2: timezone = TimeTools::timezones.time_zone_from_region("UTC+2"); break;
				case GMT3: timezone = TimeTools::timezones.time_zone_from_region("UTC+3"); break;
				case GMT4: timezone = TimeTools::timezones.time_zone_from_region("UTC+4"); break;
				case GMT5: timezone = TimeTools::timezones.time_zone_from_region("UTC+5"); break;
				case GMT6: timezone = TimeTools::timezones.time_zone_from_region("UTC+6"); break;
				case GMT7: timezone = TimeTools::timezones.time_zone_from_region("UTC+7"); break;
				case GMT8: timezone = TimeTools::timezones.time_zone_from_region("UTC+8"); break;
				case GMT9: timezone = TimeTools::timezones.time_zone_from_region("UTC+9"); break;
				case GMT10: timezone = TimeTools::timezones.time_zone_from_region("UTC+10"); break;
				case GMT11: timezone = TimeTools::timezones.time_zone_from_region("UTC+11"); break;
				case GMT12: timezone = TimeTools::timezones.time_zone_from_region("UTC+12"); break;
				default: timezone = TimeTools::timezones.time_zone_from_region("UTC"); break;
			}

			boost::gregorian::date _date(date.tm_year, date.tm_mon, date.tm_mday);
			boost::posix_time::time_duration _time(date.tm_hour,date.tm_min,date.tm_sec,0);

			boost::posix_time::ptime posixTimeWoTimezone(_date, _time);

			boost::local_time::local_date_time localDateTime(posixTimeWoTimezone, timezone);

			if (dateIsLocalTime && offsetInMinsFromUtc != 0) {
				boost::posix_time::time_duration subs(::abs(offsetInMinsFromUtc / 60), 0, 0, 0);

				if (offsetInMinsFromUtc < 0) {
					out = localDateTime + subs;
				}
				else {
					out = localDateTime - subs;
				}
			}
			else {
				out = localDateTime;
			}

		}
		else {
			throw std::runtime_error("Timezone data is not initialized! Call initBoostTimezones first! ");
		}

		return out;
	}

	TimeTools();
	virtual ~TimeTools();
};

#endif /* TIMETOOLS_H_ */
