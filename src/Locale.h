/*
 * Locale.h
 *
 *  Created on: Sep 23, 2021
 *      Author: mateusz
 */

#ifndef LOCALE_H_
#define LOCALE_H_

#include <string>

class Locale {
public:
	Locale();
	virtual ~Locale();

	std::string windSpeed;
	std::string windGusts;
	std::string windDirection;
	std::string temperature;
	std::string pressure;
	std::string humidity;
	std::string lastUpdate;
	std::string moreInfo;
	std::string generatedBy;
	std::string generatedBy2;

};

#endif /* LOCALE_H_ */
