/*
 * PlotFileDefinition.h
 *
 *  Created on: 06.05.2020
 *      Author: mateusz
 */

#ifndef PLOTFILEDEFINITION_H_
#define PLOTFILEDEFINITION_H_

#include <string>
#include "RRDFileDefinition.h"

using namespace std;

class PlotFileDefinition : public RRDFileDefinition {
public:
	virtual void Zero(void);

	PlotFileDefinition();
	virtual ~PlotFileDefinition();

	string sDS0Path;
	string sDS0Name;
	enum RRAType eDS0RRAType;
	enum PlotGraphType eDS0PlotType;
	int DS0PlotColor;
	string sDS0Label;

	string sDS1Path;
	string sDS1Name;
	enum RRAType eDS1RRAType;
	enum PlotGraphType eDS1PlotType;
	int DS1PlotColor;
	string sDS1Label;

	bool DoubleDS;

	string Title;
	int Width, Height;
	string Axis;
	float MinScale, MaxScale;
	string LongTimescaleFormat;

	int ScaleStep, LabelStep;

	int Exponent;

	int timeScaleLn;

	bool isWind; //!< It is important only if km/h should be displayed
};


#endif /* PLOTFILEDEFINITION_H_ */
