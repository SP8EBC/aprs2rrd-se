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

using namespace std;

/**
 * Enum which is used when to control which parameters are printed on the website.
 * If a table is configured to show two different sources it controls which parameter
 * from which source is displayed.
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

	/**
	 * Stores obtained weather data into RRD file(s)
	 */
	void FetchDataInRRD(const AprsWXData* const cInput, bool inhibitLog);

	/**
	 * Stores calulated differential measurements in RRD file(s)
	 */
	void FetchDiffInRRD(AprsWXData & data);
	void FetchBatteryVoltageInRRD(float voltage);

	/**
	 * Plots all configured graphs from RRD files
	 */
	void PlotGraphsFromRRD(void);

	/**
	 * Generates HTML file
	 */
	void GenerateWebiste(const AprsWXData & WX, const AprsWXData & secondaryWX, const Locale & locale, const char * datetimeLocale, const Telemetry & telemetry);

	/**
	 * This method has a practical usage only when displaying two sources are enabled. It select out among few inputs
	 * from different sources, basing on configuration data.
	 */
	void GetSecondarySource(const AprsWXData& aprsIS,
			const AprsWXData& serial,
			const AprsWXData& holfuy,
			const AprsWXData& zywiec,
			const AprsWXData& weatherlink,
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
