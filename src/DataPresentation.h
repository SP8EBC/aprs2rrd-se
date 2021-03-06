#ifndef DATAPRESENCE_H
#define DATAPRESENCE_H

#include <string>
#include <vector>
#include <exception>
#include <memory>

#include "AprsWXData.h"
#include "PlotFileDefinition.h"
#include "DataSourceConfig.h"

using namespace std;


class DataPresentation
{
public:
	DataPresentation();
	~DataPresentation();

	void FetchDataInRRD(const AprsWXData* const cInput);
	void FetchDiffInRRD(AprsWXData & data);
	void PlotGraphsFromRRD(void);
	void GenerateWebiste(const AprsWXData & WX, const AprsWXData & secondaryWX);
	void GetSecondarySource(const AprsWXData& aprsIS,
			const AprsWXData& serial,
			const AprsWXData& holfuy,
			AprsWXData& out);
	PlotType SwitchPlotType(string input);
	PlotGraphType SwitchPlotGraphType(string input);
	RRAType SwitchRRAType(string input);

	const std::string RevSwitchPlotType(PlotType in);
	const std::string RevSwitchPlotGraphType(PlotGraphType in);
	const std::string RevSwitchRRAType(RRAType in);

	vector <RRDFileDefinition> vRRDFiles;
	vector <PlotFileDefinition> vPNGFiles;

	string WebsitePath;
	string Plot0Path, Plot1Path, Plot2Path, Plot3Path, Plot4Path;

	bool PrintTemperature, PrintPressure, PrintHumidity;

	string WebsiteTitle;
	string WebsiteHeadingTitle;
	string WebsiteSubHeading;
	bool WebsiteLinkToMoreInfo;
	string WebisteFooter;

	bool PrintTwoSourcesInTable;
	string SecondaryLabel;
	string PrimaryLabel;
	WxDataSource SecondarySource;

	int8_t directionCorrection;

	bool DebugOutput;

};



#endif // DATAPRESENCE_H
