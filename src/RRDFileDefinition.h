/*
 * RRDFileDefinition.h
 *
 *  Created on: 06.05.2020
 *      Author: mateusz
 */

#ifndef RRDFILEDEFINITION_H_
#define RRDFILEDEFINITION_H_

#include <string>

enum class PlotType {
	TEMPERATURE,
	QNH,
	WIND_DIR,
	WIND_SPD,
	WIND_GST,
	WIND_SPD_GST,
	HUMIDITY,
	DIFF_TEMPERATURE,
	DIFF_WIND_SPD,
	DIFF_WIND_GST,
	DIFF_WIND_DIR,
	VOLTAGE,
	N
};

enum class PlotGraphType {
	AREA,
	LINE2,
	NN
};

enum class RRAType {
	AVERAGE,
	LAST,
	NNN
};

class RRDFileDefinition {
public:
	virtual void Zero(void);

	RRDFileDefinition();
	RRDFileDefinition(PlotType t);
	virtual ~RRDFileDefinition();

	bool operator ==(const RRDFileDefinition& _in);

	enum PlotType eType;
	std::string sPath;
};



#endif /* RRDFILEDEFINITION_H_ */
