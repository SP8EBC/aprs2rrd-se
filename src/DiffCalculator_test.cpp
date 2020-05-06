/*
 * DiffCalculator_test.cpp
 *
 *  Created on: 05.05.2020
 *      Author: mateusz
 */

#include "DiffCalculator.h"
#include "AprsWXData.h"
#include "Telemetry.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DIFF
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(diff1) {
	AprsWXData is, rs, h, out;
	Telemetry t;

	DiffCalculator d;
	d.enable = true;
	d.temperatureFrom = WxDataSource::IS_PRIMARY;
	d.temperatureSubstract = WxDataSource::HOLFUY;
	d.windFrom = WxDataSource::IS_PRIMARY;
	d.windSubstract = WxDataSource::HOLFUY;

	is.is_primary = true;
	is.is_secondary = false;

	is.temperature = 3.3f;
	h.temperature = 5.6f;

	is.wind_direction = 3;
	is.wind_speed = 5.7f;
	is.wind_gusts = 9.1f;

	h.wind_direction = 350;
	h.wind_speed = 4.5f;
	h.wind_gusts = 8.0f;

	d.calculate(is, rs, h, t, out);

	BOOST_CHECK_CLOSE(out.temperature, -2.3, 1);
	BOOST_CHECK_CLOSE(out.wind_speed, 1.2, 1);
	BOOST_CHECK_EQUAL(out.wind_direction, 13);
}

BOOST_AUTO_TEST_CASE(diff11) {
	AprsWXData is, rs, h, out;
	Telemetry t;

	DiffCalculator d;
	d.enable = true;
	d.temperatureFrom = WxDataSource::IS_PRIMARY;
	d.temperatureSubstract = WxDataSource::HOLFUY;
	d.windFrom = WxDataSource::IS_PRIMARY;
	d.windSubstract = WxDataSource::HOLFUY;

	is.is_primary = true;
	is.is_secondary = false;

	is.temperature = 3.3f;
	h.temperature = 5.6f;

	is.wind_direction = 350;
	is.wind_speed = 5.7f;
	is.wind_gusts = 9.1f;

	h.wind_direction = 3;
	h.wind_speed = 4.5f;
	h.wind_gusts = 8.0f;

	d.calculate(is, rs, h, t, out);

	BOOST_CHECK_CLOSE(out.temperature, -2.3, 1);
	BOOST_CHECK_CLOSE(out.wind_speed, 1.2, 1);
	BOOST_CHECK_EQUAL(out.wind_direction, -13);
}

BOOST_AUTO_TEST_CASE(diff2) {
	AprsWXData is, rs, h, out;
	Telemetry t;

	DiffCalculator d;
	d.enable = true;
	d.temperatureFrom = WxDataSource::IS_PRIMARY;
	d.temperatureSubstract = WxDataSource::HOLFUY;
	d.windFrom = WxDataSource::IS_PRIMARY;
	d.windSubstract = WxDataSource::HOLFUY;

	is.is_primary = true;
	is.is_secondary = false;

	is.temperature = 6.3f;
	h.temperature = 5.6f;

	is.wind_direction = 300;
	is.wind_speed = 5.7f;
	is.wind_gusts = 9.1f;

	h.wind_direction = 350;
	h.wind_speed = 4.5f;
	h.wind_gusts = 8.0f;

	d.calculate(is, rs, h, t, out);

	BOOST_CHECK_CLOSE(out.temperature, 0.7, 1);
	BOOST_CHECK_CLOSE(out.wind_speed, 1.2, 1);
	BOOST_CHECK_EQUAL(out.wind_direction, -50);
}

BOOST_AUTO_TEST_CASE(diff3) {
	AprsWXData is, rs, h, out;
	Telemetry t;

	DiffCalculator d;
	d.enable = true;
	d.temperatureFrom = WxDataSource::IS_PRIMARY;
	d.temperatureSubstract = WxDataSource::HOLFUY;
	d.windFrom = WxDataSource::IS_PRIMARY;
	d.windSubstract = WxDataSource::HOLFUY;

	is.is_primary = true;
	is.is_secondary = false;

	is.temperature = 6.3f;
	h.temperature = 5.6f;

	is.wind_direction = 350;
	is.wind_speed = 5.7f;
	is.wind_gusts = 9.1f;

	h.wind_direction = 300;
	h.wind_speed = 4.5f;
	h.wind_gusts = 8.0f;

	d.calculate(is, rs, h, t, out);

	BOOST_CHECK_CLOSE(out.temperature, 0.7, 1);
	BOOST_CHECK_CLOSE(out.wind_speed, 1.2, 1);
	BOOST_CHECK_EQUAL(out.wind_direction, 50);
}
