/*
 * DiffCalculator.h
 *
 *  Created on: 05.05.2020
 *      Author: mateusz
 */

#ifndef DIFFCALCULATOR_H_
#define DIFFCALCULATOR_H_

#include "DataSourceConfig.h"
#include "AprsWXData.h"
#include "Telemetry.h"

class DiffCalculator {
public:
	bool enable;

	WxDataSource temperatureFrom;
	WxDataSource temperatureSubstract;

	WxDataSource windFrom;
	WxDataSource windSubstract;

	void calculate(	const AprsWXData& aprsIS,
					const AprsWXData& serial,
					const AprsWXData& holfuy,
					const AprsWXData& zywiec,
					const AprsWXData& weatherlink,
					const Telemetry& telemetry,
					AprsWXData& out);

	DiffCalculator();
	virtual ~DiffCalculator();
};

#endif /* DIFFCALCULATOR_H_ */
