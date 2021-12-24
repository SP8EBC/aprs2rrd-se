/*
 * ZywiecMeteo.h
 *
 *  Created on: Dec 19, 2021
 *      Author: mateusz
 */

#ifndef ZYWIECMETEO_H_
#define ZYWIECMETEO_H_

#include <string>
#include "AprsWXData.h"

#include <boost/date_time.hpp>

class ZywiecMeteo {

	/**
	 * http://monitoring.zywiec.powiat.pl/
	 */
	std::string baseUrl;

	static size_t static_write_callback(char *get_data, size_t always_one, size_t get_data_size, void *userdata);

	void write_callback(char* data, size_t data_size);

	ZywiecMeteo * ptr;

	std::string responseBuffer;

	boost::posix_time::time_duration hour;

	/**
	 * https://stackoverflow.com/questions/17779660/who-is-responsible-for-deleting-the-facet
	 *
     * Once a facet reference is obtained from a locale object by calling use_facet<>,
     * that reference remains usable, and the results from member functions of it may
     * be cached and re-used, as long as some locale object refers to that facet.
	 *
	 */

	boost::posix_time::time_facet * formatter;

	/**
	 * Set to true to use temp2m (2 meters abouve ground???), keep set to false to use temp ('normal' temperature??)
	 */
	bool temperatureSwitch;


public:
	ZywiecMeteo(std::string _base_url, bool _temperature_switch);
	virtual ~ZywiecMeteo();
	ZywiecMeteo(const ZywiecMeteo &other);
	ZywiecMeteo(ZywiecMeteo &&other);
	ZywiecMeteo& operator=(const ZywiecMeteo &other);
	ZywiecMeteo& operator=(ZywiecMeteo &&other);

	bool downloadLastMeasureForStation(int stationId, std::string & response);
	bool downloadMeasuresFromRangeForStation(int stationId, std::string & respons);

	void parseJson(std::string & in, AprsWXData & out);

};

/**
 * Station mapping:
 *
 * 505 - Jezioro Żywieckie, u Rumcajsa na półwyspie
 * 509 - GSS Międzybrodzie, hangar szybowcowy
 * 511 - Żywiec centrala
 * 507 - Jeleśnia Urząd Gminy
 * 508 - Ochodzita
 *
 */

/**
 * Example response
 *
 * http://monitoring.zywiec.powiat.pl/server.php?action=getLastMeasure&deviceID=505
 *
 * {
	"result": [
		{
			"datetime": "2021-12-19 21:00:19",
			"rainfall_type": "mieszany",
			"temp": "3.30000",
			"humid": "84.00000",
			"wind_direct": "290.00",
			"wind_speed": "6.31",
			"rainfall_intensity": "3.00000",
			"pressure": "101300.00",
			"prtrend": "1"
		}
	]
}
 */

#endif /* ZYWIECMETEO_H_ */

/**
 * Example response
 *
 * http://monitoring.zywiec.powiat.pl/server.php?action=getMeasuresFromRange&deviceID=509&fromDate=2021-12-19%2018%3A00%3A00&toDate=2021-12-19%2023%3A59%3A59&timeStep=10minutowy
 *
 *
 *
 * {
   "result":[
      {
         "datetime":"2021-12-19 18:00",
         "temp":"4.30",
         "temp2m":"4.20",
         "tempmin2m":null,
         "tempmax2m":null,
         "tempmin":null,
         "tempmax":null,
         "dew_point":"1.80",
         "humid":"84.00",
         "humid2m":"97.20",
         "humidmax":"0",
         "humidmax2m":"0",
         "humidmin":"0",
         "humidmin2m":"0",
         "wind_min":"0",
         "wind_direct":"271.22",
         "wind_speed":"6.35",
         "wind_max":"13.37",
         "rainfall_type":"deszcz",
         "rainfall":"0.1",
         "rainfall_intensity":"0.00",
         "pressure":"101370.00"
      },
      {
         "datetime":"2021-12-19 18:10",
         "temp":"4.20",
         "temp2m":"3.70",
         "tempmin2m":null,
         "tempmax2m":null,
         "tempmin":null,
         "tempmax":null,
         "dew_point":"2.04",
         "humid":"86.00",
         "humid2m":"97.60",
         "humidmax":"0",
         "humidmax2m":"0",
         "humidmin":"0",
         "humidmin2m":"0",
         "wind_min":"0",
         "wind_direct":"269.29",
         "wind_speed":"6.39",
         "wind_max":"15.40",
         "rainfall_type":"deszcz",
         "rainfall":"0",
         "rainfall_intensity":"0.60",
         "pressure":"101360.00"
      },
 *
 */
