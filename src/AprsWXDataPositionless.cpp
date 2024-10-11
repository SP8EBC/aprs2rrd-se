#include "AprsWXDataPositionless.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <ctime>

#include "TimeTools.h"

int AprsWXDataPositionless::ParseData(const std::string & in, AprsWXData * output) {

    // _10101355c...s...g...t079[0_1258_277][1_629_270][2_0_265][3_2516_274]
    
    tm tm_timestamp = {0u};
    tm * tm_current_utc = {0u};
    time_t utc_raw_time;

    // if string doesn't contain correct prefix or it is too short
    if (in.length() < 25) {
        return -1;
    }

    if (in.at(0) != '_' || in.at(9) != 'c' || in.at(13) != 's' || in.at(17) != 'g' || in.at(21) != 't') {
        return -2;
    }

    time(&utc_raw_time);
    tm_current_utc = gmtime(&utc_raw_time);

    // check if standard positionless aprs packet contains specific history data
    const size_t history_pos = in.find_first_of('[', 0);

    // timestamp
    const std::string datetime_str = in.substr(1, 8);

    // wind direction
    const std::string winddir_str = in.substr(10, 3);

    const std::string windspd_str = in.substr(14, 3);

    const std::string windgust_str = in.substr(18, 3);

    const std::string temperature_str = in.substr(22, 3);

    // convert timestamp to output structure
    const int month = boost::lexical_cast<int>(datetime_str.substr(0, 2));
    const int day = boost::lexical_cast<int>(datetime_str.substr(2, 2));
    const int hour = boost::lexical_cast<int>(datetime_str.substr(4, 2));
    const int minutes = boost::lexical_cast<int>(datetime_str.substr(6, 2));

    // set a tm structure with decoded month, day, hour, minute & current year
    tm_timestamp.tm_year = tm_current_utc->tm_year;
    tm_timestamp.tm_mon = month;
    tm_timestamp.tm_mday = day;

    tm_timestamp.tm_hour = hour;
    tm_timestamp.tm_min = minutes;    

    // store conversion output in 
    output->packetLocalTimestmp = boost::posix_time::ptime_from_tm(tm_timestamp);
    output->packetUtcTimestamp = TimeTools::getEpochFromPtime(output->packetLocalTimestmp, true);

    try {
        // convert winddirection to output structure
        output->wind_direction = boost::lexical_cast<int>(windspd_str);
    }
    catch (const boost::bad_lexical_cast& ex) {
        output->wind_direction = 0;
        std::cout << ex.what();
    }
    catch (const std::bad_cast& ex) {
        output->wind_direction = 0;
        std::cout << ex.what();
    }

    try {
        // convert windspeed and windgusts to output structure
        output->wind_speed = (float)boost::lexical_cast<int>(windspd_str) * 0.44f;
        output->wind_gusts = (float)boost::lexical_cast<int>(windgust_str) * 0.44f;
    }
    catch (const boost::bad_lexical_cast& ex) {
        output->wind_speed = 0.0f;
        output->wind_gusts = 0.0f;
    }
    catch (const std::bad_cast& ex) {
        output->wind_direction = 0;
        std::cout << ex.what();
    }

    // convert temperature to output structure
    output->temperature = ((float)boost::lexical_cast<int>(temperature_str) - 32.0f) / 9.0f * 5.0f;

    // if positionless packet contains optional, custom historic data
    if (history_pos != std::string::npos) {
        std::cout << "AprsWXDataPositionless::ParseData - historical data detected" << std::endl;

        std::vector<std::string> splitPerSqBracket;

        // create a substring with custom historic entries
        const std::string & historyStr = in.substr(history_pos);

        // split sections per '[' character
        // the 'split' method doesn't copy a character which was used as split point!
        boost::split(splitPerSqBracket, historyStr, boost::is_any_of("["));

        // go through all historic entries
        for (const std::string & elem : splitPerSqBracket) {
            std::vector<std::string> splitUnderscore;

            // split element of single entry
            boost::split(splitUnderscore, elem, boost::is_any_of("_]"));

            if (splitUnderscore.size() > 2) {
                // 0 - index
                // 1 - age in seconds
                // 2 - value
                if (splitUnderscore.at(1).find_first_not_of("0123456789") == std::string::npos) {
                    if (splitUnderscore.at(2).find_first_not_of("0123456789") == std::string::npos) {
                        // calculate epoch timestamp
                        const uint64_t entryEpoch = TimeTools::getEpoch() - boost::lexical_cast<int>(splitUnderscore.at(1));
                        const float temperature = boost::lexical_cast<float>(splitUnderscore.at(2)) / 10.0f;

                        output->additionalTemperature.emplace_back(entryEpoch, temperature);
                    }
                }
            }
        }
    }
    return 0;

}
