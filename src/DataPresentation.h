#ifndef DATAPRESENCE_H
#define DATAPRESENCE_H

#include <string>
#include <vector>
#include <exception>
#include <memory>

#include "AprsWXData.h"
#include "PlotFileDefinition.h"
#include "DataSourceConfig.h"
#include "Locale.h"
#include "Telemetry.h"

using namespace std;

/**
 *
 */
enum DataPresentationParametersPrint {
	PRINT_OFF,
	PRINT_BOTH,
	PRINT_LEFT_PRIMARY,
	PRINT_RIGHT_SECONDARY
};

class DataPresentation
{
public:
	DataPresentation();
	~DataPresentation();

	void FetchDataInRRD(const AprsWXData* const cInput, bool inhibitLog);
	void FetchDiffInRRD(AprsWXData & data);
	void FetchBatteryVoltageInRRD(float voltage);
	void PlotGraphsFromRRD(void);
	void GenerateWebiste(const AprsWXData & WX, const AprsWXData & secondaryWX, const Locale & locale, const char * datetimeLocale, const Telemetry & telemetry);
	void GetSecondarySource(const AprsWXData& aprsIS,
			const AprsWXData& serial,
			const AprsWXData& holfuy,
			const AprsWXData& zywiec,
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

	DataPresentationParametersPrint PrintTemperature, PrintPressure, PrintHumidity, PrintWind;

	bool SpecialTelemetry;

	string WebsiteTitle;
	string WebsiteHeadingTitle;
	string WebsiteSubHeading;
	bool WebsiteLinkToMoreInfo;
	string WebisteFooter;

	bool PrintTwoSourcesInTable;
	string SecondaryLabel;
	string PrimaryLabel;
	WxDataSource SecondarySource;

	string WebsiteAdditionalImage;
	string WebsiteAdditionalImgeUrl;

	int32_t directionCorrection;

	bool colorfulResultTable;

	bool DebugOutput;

	static std::string ParametersPrintEnumToStr(DataPresentationParametersPrint e) {
		switch (e) {
		case PRINT_OFF: return "Print disable";
		case PRINT_BOTH: return "Print enable for both";
		case PRINT_LEFT_PRIMARY: return "Enable only left (primary)";
		case PRINT_RIGHT_SECONDARY: return "Enable only right (secondary)";
		}

		return "";
	}

};



#endif // DATAPRESENCE_H
