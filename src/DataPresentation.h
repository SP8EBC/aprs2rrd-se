#ifndef DATAPRESENCE_H
#define DATAPRESENCE_H

#include <string>
#include <vector>
#include <exception>

#include "AprsWXData.h"

using namespace std;

enum class PlotType {
	TEMPERATURE,
	QNH,
	WIND_DIR,
	WIND_SPD,
	WIND_GST,
	WIND_SPD_GST,
	HUMIDITY,
	DIFF_TEMPERATURE,
	DIFF_WIND,
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

	enum PlotType eType;
	string sPath;
};

class PlotFileDefinition : public RRDFileDefinition {
public:
	virtual void Zero(void);

	PlotFileDefinition();

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
	string Axis, AxisR;
	float MinScale, MaxScale;
	float MinScaleR, MaxScaleR;

	int ScaleStep, LabelStep;

	int Exponent;

	int timeScaleLn;
};


class DataPresentation
{
public:
	DataPresentation();
	~DataPresentation();

	void FetchDataInRRD(AprsWXData* cInput);
	void PlotGraphsFromRRD(void);
	void GenerateWebiste(AprsWXData* WX);
	PlotType SwitchPlotType(string input);
	PlotGraphType SwitchPlotGraphType(string input);
	RRAType SwitchRRAType(string input);

	const std::string RevSwitchPlotType(PlotType in);
	const std::string RevSwitchPlotGraphType(PlotGraphType in);
	const std::string RevSwitchRRAType(RRAType in);

	vector <RRDFileDefinition> vRRDFiles;
	vector <PlotFileDefinition> vPNGFiles;

	int8_t directionCorrection;

	string WebsitePath;
	string Plot0Path, Plot1Path, Plot2Path, Plot3Path;

	bool PrintTemperature, PrintPressure, PrintHumidity;

	string WebsiteTitle;
	string WebsiteHeadingTitle;
	string WebsiteSubHeading;
	bool WebsiteLinkToMoreInfo;
	string WebisteFooter;

	bool DebugOutput;

};



#endif // DATAPRESENCE_H
