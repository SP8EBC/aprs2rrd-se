/*
 * TimeTools.cpp
 *
 *  Created on: Apr 8, 2023
 *      Author: mateusz
 */

#include "TimeTools.h"

boost::local_time::tz_database TimeTools::timezones;

bool TimeTools::timezonesInit = false;

TimeTools::TimeTools() {
	// TODO Auto-generated constructor stub

}

TimeTools::~TimeTools() {
	// TODO Auto-generated destructor stub
}

