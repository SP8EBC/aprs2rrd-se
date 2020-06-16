/*
 * PressureCalculator_test.cpp
 *
 *  Created on: 15.06.2020
 *      Author: mateusz
 */

#include "PressureCalculator.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE PRESSURE
#include <boost/test/unit_test.hpp>

PressureCalculator calc;

struct fixture_qnh {
	fixture_qnh() {
		calc.setAltitude(1000);
		calc.setConvertTo(PressureCalculatorType::TO_QNH);
		calc.setEnable(true);

	}
};

struct fixture_qfe {
	fixture_qfe() {
		calc.setAltitude(620);	// bezmiechowa
		calc.setConvertTo(PressureCalculatorType::TO_QFE);
		calc.setEnable(true);

	}
};


BOOST_FIXTURE_TEST_CASE(to_qnh, fixture_qnh) {

	// ciśnienie mierzone przez stacje -> 845hPa
	float output = calc.convertPressure(845.0f, 0);

	BOOST_CHECK_CLOSE(954.0f, output, 0.5f);
}

BOOST_FIXTURE_TEST_CASE(to_qnh_2, fixture_qnh) {

	// bezmeichowa
	calc.setAltitude(620);

	// ciśnienie mierzone przez stacje -> 845hPa
	float output = calc.convertPressure(940.0f, 0);

	BOOST_CHECK_CLOSE(1012.0f, output, 0.5f);

}

BOOST_FIXTURE_TEST_CASE(to_qnh_3, fixture_qnh) {

	// bezmeichowa
	calc.setAltitude(620);

	// ciśnienie mierzone przez stacje -> 845hPa
	float output = calc.convertPressure(940.0f, 30);

	BOOST_CHECK_CLOSE(1012.0f, output, 0.5f);

}

BOOST_FIXTURE_TEST_CASE(to_qfe_1, fixture_qfe) {
	float output = calc.convertPressure(1012.0, 15);

	BOOST_CHECK_CLOSE(940.0f, output, 0.5f);

}
