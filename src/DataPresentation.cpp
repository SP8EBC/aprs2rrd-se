#include "DataPresentation.h"

#include "SOFTWARE_VERSION.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <boost/date_time/local_time/local_time.hpp>

#include <fstream>
//#include "main.h"

DataPresentation::DataPresentation() : 	WebsitePath(""),
										PrintTemperature(PRINT_OFF),
										PrintPressure (PRINT_OFF),
										PrintHumidity (PRINT_OFF),
										PrintWind(PRINT_OFF),
										WebsiteTitle(""),
										WebsiteHeadingTitle(""),
										WebsiteSubHeading(""),
										WebsiteLinkToMoreInfo(""),
										WebisteFooter(""),
										PrintTwoSourcesInTable(false),
										SecondaryLabel (""),
										PrimaryLabel (""),
										directionCorrection (0),
										colorfulResultTable(false),
										DebugOutput(false)
{
	this->Plot0Path.clear();
	this->Plot1Path.clear();
	this->Plot2Path.clear();
	this->Plot3Path.clear();

	this->SecondarySource = WxDataSource::UNKNOWN;
}

DataPresentation::~DataPresentation()
{
}

void DataPresentation::FetchDiffInRRD(AprsWXData& data) {

	std::stringstream command;
	RRDFileDefinition diff_temperature, diff_windspd, diff_winddir;
	boost::posix_time::ptime current = boost::posix_time::second_clock::universal_time();
	boost::posix_time::ptime epoch (boost::gregorian::date(1970, 1, 1));

	time_t seconds = (current - epoch).total_seconds();

	if (!data.useTemperature && !data.useWind)
		return;

	std::cout << "--- DataPresentation::FetchDiffInRRD:58 - Inserting diffs into RRD files" << std::endl;

	auto diff_temperature_it = std::find(this->vRRDFiles.begin(), this->vRRDFiles.end(), RRDFileDefinition(PlotType::DIFF_TEMPERATURE));
	auto diff_winddir_it = std::find(this->vRRDFiles.begin(), this->vRRDFiles.end(), RRDFileDefinition(PlotType::DIFF_WIND_DIR));
	auto diff_windspd_it = std::find(this->vRRDFiles.begin(), this->vRRDFiles.end(), RRDFileDefinition(PlotType::DIFF_WIND_SPD));
	auto diff_windgst_it = std::find(this->vRRDFiles.begin(), this->vRRDFiles.end(), RRDFileDefinition(PlotType::DIFF_WIND_GST));

	if (diff_temperature_it != this->vRRDFiles.end()) {
		command << "rrdtool update " << diff_temperature_it->sPath << " " << seconds << ":" << data.temperature;
		::system(command.str().c_str());
		if (this->DebugOutput == true)
			cout << command.str() << endl;
	}

	if (diff_winddir_it != this->vRRDFiles.end()) {
		command.str("");
		command << "rrdtool update " << diff_winddir_it->sPath << " " << seconds << ":" << data.wind_direction;
		::system(command.str().c_str());
		if (this->DebugOutput == true)
			cout << command.str() << endl;
	}

	if (diff_windspd_it != this->vRRDFiles.end()) {
		command.str("");
		command << "rrdtool update " << diff_windspd_it->sPath << " " << seconds << ":" << data.wind_speed;
		::system(command.str().c_str());
		if (this->DebugOutput == true)
			cout << command.str() << endl;
	}

	if (diff_windgst_it != this->vRRDFiles.end()) {
		command.str("");
		command << "rrdtool update " << diff_windgst_it->sPath << " " << seconds << ":" << data.wind_gusts;
		::system(command.str().c_str());
		if (this->DebugOutput == true)
			cout << command.str() << endl;
	}
}

void DataPresentation::FetchDataInRRD(const AprsWXData* const cInput, bool inhibitLog) {
	char command[512];
	int currtimeint;
	time_t currtime;

	int sys_retval = 0;

	if (cInput == nullptr)
		return;

	if (!cInput->valid) {
		//cout << "--- DataPresentation::FetchDataInRRD:108 - Input data is invalid and cannot be used" << endl;
		return;
	}

	for (unsigned i = 0; i < this->vRRDFiles.size(); i++) {
		if (this->vRRDFiles[i].eType == PlotType::TEMPERATURE && cInput->useTemperature == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->temperature);
			if (this->DebugOutput == true && inhibitLog == false)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == PlotType::QNH && cInput->usePressure == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%d", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->pressure);
			if (this->DebugOutput == true && inhibitLog == false)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == PlotType::WIND_DIR && cInput->useWind == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%d", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_direction);
			if (this->DebugOutput == true && inhibitLog == false)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == PlotType::WIND_SPD && cInput->useWind == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_speed);
			if (this->DebugOutput == true && inhibitLog == false)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == PlotType::WIND_GST && cInput->useWind == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_gusts);
			if (this->DebugOutput == true && inhibitLog == false)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == PlotType::HUMIDITY && cInput->useHumidity == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, (float)cInput->humidity);
			if (this->DebugOutput == true && inhibitLog == false)
				cout << command << endl;
		}
		else continue;

		sys_retval = system(command);

		if (sys_retval != 0) {
			;
		}

		memset(command, 0x00, sizeof(command));
		sprintf(command, "rrdtool dump %s > %s.dmp", this->vRRDFiles[i].sPath.c_str(), this->vRRDFiles[i].sPath.c_str());
		sys_retval = system(command);

		if (sys_retval != 0) {
			;
		}
	}
}

void DataPresentation::FetchBatteryVoltageInRRD(float voltage) {
	char command[512];
	int currtimeint;
	time_t currtime;

	int sys_retval = 0;

	cout << "--- DataPresentation::FetchBatteryVoltageInRRD:186 - voltage: " << voltage << endl;
	for (unsigned i = 0; i < this->vRRDFiles.size(); i++) {
		if (this->vRRDFiles[i].eType == PlotType::VOLTAGE) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, voltage);
			if (this->DebugOutput == true)
				cout << command << endl;

			sys_retval = system(command);

			if (sys_retval != 0) {
				;
			}
		}
	}
}

void DataPresentation::PlotGraphsFromRRD() {
	char command[1024];
	int currtimeint;
	unsigned i;
	time_t currtime;

	int sys_retval = 0;

	currtime =time(NULL);
	currtimeint = (int)currtime;

	stringstream xgrid;
	stringstream scalestep;
	stringstream exp;

	std::string rraType;
	std::string graphType;

	std::string rra2Type;
	std::string graph2Type;

	if (this->DebugOutput == true) {
		cout << "--- DataPresentation::PlotGraphsFromRRD:227 -  Count of plots to be generated: " <<  this->vPNGFiles.size() << endl;
	}

	for (i = 0; i < this->vPNGFiles.size(); i++) {
        xgrid.str("");
	scalestep.str("");
	exp.str("");

        if (this->vPNGFiles[i].timeScaleLn <= 840 /* 14 godzin */)
            xgrid << "MINUTE:10:HOUR:1:HOUR:1:0:%H:00";
        else if (this->vPNGFiles[i].timeScaleLn > 840 && this->vPNGFiles[i].timeScaleLn <= 1680 /* 28 godziny*/)
            xgrid << "MINUTE:20:HOUR:2:HOUR:2:0:%H:00";
        else if (this->vPNGFiles[i].timeScaleLn > 1680 && this->vPNGFiles[i].timeScaleLn <= 2520 /* 42 godziny*/)
             xgrid << "MINUTE:30:HOUR:4:HOUR:4:0:" << this->vPNGFiles[i].LongTimescaleFormat;
        else
             xgrid << "MINUTE:60:HOUR:6:HOUR:6:0:"  << this->vPNGFiles[i].LongTimescaleFormat;

	if (this->vPNGFiles[i].ScaleStep > 0 && this->vPNGFiles[i].LabelStep > 0) {
		scalestep << " --y-grid " << this->vPNGFiles[i].ScaleStep << ":" << this->vPNGFiles[i].LabelStep << " ";
	}
	else scalestep << " ";

	if (this->vPNGFiles[i].Exponent != -1)
		exp << " -X " << this->vPNGFiles[i].Exponent;
	else {
		exp << " ";
	}

		memset(command, 0x00, sizeof(command));
		if (this->vPNGFiles[i].DoubleDS == false) {
			rraType = this->RevSwitchRRAType(this->vPNGFiles[i].eDS0RRAType);
			graphType = this->RevSwitchPlotGraphType(this->vPNGFiles[i].eDS0PlotType);

			sprintf(command, "rrdtool graph %s -w %d -h %d -t \"%s\" -v \"%s\" -l %.4e -u %.4e -r %s %s --right-axis 1:0 --right-axis-label \"%s\" --x-grid %s --start %d --end %d DEF:%s=%s:%s:%s %s:%s#%.6x", \
					this->vPNGFiles[i].sPath.c_str(), this->vPNGFiles[i].Width, this->vPNGFiles[i].Height, \
					this->vPNGFiles[i].Title.c_str(), this->vPNGFiles[i].Axis.c_str() , this->vPNGFiles[i].MinScale, \
					this->vPNGFiles[i].MaxScale, scalestep.str().c_str(), exp.str().c_str(), 
					this->vPNGFiles[i].Axis.c_str(), xgrid.str().c_str(), currtimeint- (this->vPNGFiles[i].timeScaleLn * 60), currtimeint, \
					this->vPNGFiles[i].sDS0Name.c_str(), this->vPNGFiles[i].sDS0Path.c_str(), this->vPNGFiles[i].sDS0Name.c_str(), \
					rraType.c_str(), \
					graphType.c_str(), \
					this->vPNGFiles[i].sDS0Name.c_str(), this->vPNGFiles[i].DS0PlotColor);
		}
		else {
			rraType = this->RevSwitchRRAType(this->vPNGFiles[i].eDS0RRAType);
			graphType = this->RevSwitchPlotGraphType(this->vPNGFiles[i].eDS0PlotType);

			rra2Type = this->RevSwitchRRAType(this->vPNGFiles[i].eDS1RRAType);
			graph2Type = this->RevSwitchPlotGraphType(this->vPNGFiles[i].eDS1PlotType);

			sprintf(command, "rrdtool graph %s -w %d -h %d -t \"%s\" -v \"%s\" -l %.4e -u %.4e -r %s %s --right-axis 1:0 --right-axis-label \"%s\" --x-grid %s --start %d --end %d DEF:%s=%s:%s:%s DEF:%s=%s:%s:%s %s:%s#%.6x:%s %s:%s#%.6x:%s", \
					this->vPNGFiles[i].sPath.c_str(), this->vPNGFiles[i].Width, this->vPNGFiles[i].Height, \
					this->vPNGFiles[i].Title.c_str(), this->vPNGFiles[i].Axis.c_str() , this->vPNGFiles[i].MinScale, \
					this->vPNGFiles[i].MaxScale, scalestep.str().c_str(), exp.str().c_str(), 
					this->vPNGFiles[i].Axis.c_str(), xgrid.str().c_str(), currtimeint- (this->vPNGFiles[i].timeScaleLn * 60), currtimeint, \
					this->vPNGFiles[i].sDS0Name.c_str(), this->vPNGFiles[i].sDS0Path.c_str(), this->vPNGFiles[i].sDS0Name.c_str(), \
										rraType.c_str(), \
					this->vPNGFiles[i].sDS1Name.c_str(), this->vPNGFiles[i].sDS1Path.c_str(), this->vPNGFiles[i].sDS1Name.c_str(), \
										rra2Type.c_str(), \
					graphType.c_str(), this->vPNGFiles[i].sDS0Name.c_str(), \
					this->vPNGFiles[i].DS0PlotColor, this->vPNGFiles[i].sDS0Label.c_str(), \
					graph2Type.c_str(), this->vPNGFiles[i].sDS1Name.c_str(), \
					this->vPNGFiles[i].DS1PlotColor, this->vPNGFiles[i].sDS1Label.c_str());
		}


		if (this->DebugOutput == true)
			cout << command << endl;
		sys_retval = system(command);

		if (sys_retval != 0) {
			;
		}
	}
}

void DataPresentation::GenerateWebiste(const AprsWXData & WX, const AprsWXData & secondaryWX, const Locale & locale, const char * datetimeLocale, const Telemetry & telemetry) {

	uint8_t temperaturePrecision = 3;

	boost::posix_time::ptime localtime(boost::date_time::second_clock<boost::posix_time::ptime>::local_time());

	boost::posix_time::time_facet * formatter = new boost::posix_time::time_facet();
	formatter->format("%d %B %H:%M");

	//boost::filesystem::path html{this->WebsitePath.c_str()};
	std::ofstream html;

	float batteryVoltage = telemetry.getBatteryVoltage();
	int rawMeasurement = (int)telemetry.getRawMeasurement();

	html.open(this->WebsitePath.c_str(), ios::out | ios::trunc);

	if (!html.is_open()) {
		std::cout << "--- DataPresentation::GenerateWebiste:321 - Html file cannot by opened because of unknown reason" << std::endl;
		return;
	}

	if (!html.good()) {
		std::cout << "--- DataPresentation::GenerateWebiste:326 - Something is wrong with the html file!" << std::endl;
		return;
	}


	//static const char mon_name[][13] = {
    //"Stycznia", "Lutego", "Marca", "Kwietnia", "Maja", "Czerwca",
    //"Lipca", "Sierpnia", "Wrzesnia", "Pazdziernika", "Listopada", "Grudnia"
    //};

	//html.precision(3);

	std::cout << "--- DataPresentation::GenerateWebiste:338 - Html file opened" << std::endl;

	std::cout << "--- DataPresentation::GenerateWebiste:340 - batteryVoltage: " << batteryVoltage << ", rawMeasurement: " << rawMeasurement << std::endl;

	try {
		html << " <!DOCTYPE html>" << std::endl;
		html <<	"<HTML><head>" << std::endl;
		html << "<TITLE>" << this->WebsiteTitle << "</TITLE>" << std::endl;
		html << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		if (colorfulResultTable) {
			if (WX.temperature > 0.0f) {
				html << "<link rel=\"stylesheet\" type=\"text/css\" href=\"green-style.css\"></head>" << std::endl;
			}
			else {
				html << "<link rel=\"stylesheet\" type=\"text/css\" href=\"blue-style.css\"></head>" << std::endl;
			}
		}
		else {
			html << "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></head>" << std::endl;
		}
		html << "<P><H2>" << this->WebsiteHeadingTitle << "</H2></P>" << std::endl;

		html << "<table class=\"data\"><tbody>" << std::endl;

		if (this->PrintWind != PRINT_OFF) {
			html << "<tr>" << std::endl;
			html << "<td class=table_caption>" << locale.windSpeed << ":</td>" << std::endl;
			html << "<td class=table_value id=average> "<< WX.wind_speed << " m/s </td>" << std::endl;
			html << "</tr>" << std::endl;
			html << "<tr>" << std::endl;
			html << "<td class=table_caption>" << locale.windGusts << ":</td>" << std::endl;
			html << "<td class=table_value id=gusts> " << WX.wind_gusts << " m/s </td>" << std::endl;
			html << "</tr>" << std::endl;
			html << "<tr>" << std::endl;
			html << "<td class=table_caption>" << locale.windDirection <<":</td><td class=table_value id=kierunek> " << WX.wind_direction << " stopni ";


			if (WX.wind_direction <= 11 && WX.wind_direction >= 349)
				html << "- z północy";
			else if (WX.wind_direction <= 34 && WX.wind_direction > 11)
				html << "- z północy-północnego wschodu";
			else if (WX.wind_direction <= 56 && WX.wind_direction > 34)
				html << "- z północnego wschodu";
			else if (WX.wind_direction <= 79 && WX.wind_direction > 56)
				html << "- ze wschodu-północnego wschodu";
			else if (WX.wind_direction <= 101 && WX.wind_direction > 79)
				html << "- ze wschodu";
			else if (WX.wind_direction <= 124 && WX.wind_direction > 101)
				html << "- ze wschodu-południowego wschodu";
			else if (WX.wind_direction <= 146 && WX.wind_direction > 124)
				html << "- z południowego wschodu";
			else if (WX.wind_direction <= 169 && WX.wind_direction > 146)
				html << "- z południa-południowego wschodu";
			else if (WX.wind_direction <= 191 && WX.wind_direction > 169)
				html << "- z południa";
			else if (WX.wind_direction <= 214 && WX.wind_direction > 191)
				html << "- z południa-południowego zachodu";
			else if (WX.wind_direction <= 236 && WX.wind_direction > 214)
				html << "- z południowego zachodu";
			else if (WX.wind_direction <= 259 && WX.wind_direction > 236)
				html <<"- z zachodu-południowego zachodu";
			else if (WX.wind_direction <= 281 && WX.wind_direction > 259)
				html << "- z zachodu";
			else if (WX.wind_direction <= 304 && WX.wind_direction > 281)
				html << "- z zachodu-północnego zachodu";
			else if (WX.wind_direction <= 327 && WX.wind_direction > 304)
				html << "- z północnego zachodu";
			else if (WX.wind_direction <= 349 && WX.wind_direction > 327)
				html << "- z północy-północnego zachodu";
			else;

			html << "</td></tr>";
		}

		if (this->PrintTemperature != PRINT_OFF) {
			html << "<tr><td class=table_caption>" << locale.temperature <<":</td><td class=table_value id=temperature> " << std::setprecision(temperaturePrecision) << WX.temperature << " ⁰C ";
		}
		if (this->PrintPressure != PRINT_OFF) {
			html << "<tr><td class=table_caption>" << locale.pressure << ":</td><td class=table_value id=pressure> " << WX.pressure << " hPa ";
		}
		if (this->PrintHumidity != PRINT_OFF) {
			html << "<tr><td class=table_caption>" << locale.humidity << ":</td><td class=table_value id=humidity> " << WX.humidity << " % ";
		}
		if (SpecialTelemetry) {
			html << "<tr><td class=table_caption>Napięcie akumulatora:</td><td class=table_value id=vbat> " << std::setprecision(temperaturePrecision) << batteryVoltage << " V ";
			html << "<tr><td class=table_caption>Surowy odczyt </td><td class=table_value id=raw> 0x" << std::hex << rawMeasurement <<  std::dec << " ";
		}

		html << "</tbody></table>";

		if (SpecialTelemetry) {
			html << "  <p></p><table class=sub_heading><tbody><tr><td class=sub_heading>Status sterownika</td></tr></tbody></table>" << std::endl;
			html << "<table class=data><tbody><tr>" << std::endl;
			html << "<td class=table_value>LSERDY</td>" << std::endl;
			html << "<td class=table_value>LSERDY</td>" << std::endl;
			html << "<td class=table_value>MAX_OK</td>" << std::endl;
			html << "<td class=table_value>SLEEP</td>" << std::endl;
			html << "<td class=table_value>SPI_ER</td>" << std::endl;
			html << "<td class=table_value>SPI_OK</td>" << std::endl;
			html << "</tr></tbody></table>" << std::endl;
		}

		if (datetimeLocale == NULL) {
			html.imbue(std::locale(std::locale::classic(), formatter));
		}
		else {
			html.imbue(std::locale(std::locale(datetimeLocale), formatter));
		}
		html << "<P class=last_update>" << locale.lastUpdate << ": " << localtime << " </P>";
		html << "<table class=sub_heading><tbody><tr><td class=sub_heading>" << this->WebsiteSubHeading << "</td></tr>";

		if (this->WebsiteAdditionalImage.size() > 1) {
			if (this->WebsiteAdditionalImgeUrl.size() > 1) {
				html << "<tr><td><a href = \"" << this->WebsiteAdditionalImgeUrl <<  "\"><img class = \"additional\"  src=\"" << this->WebsiteAdditionalImage << "\"></a><td></tr>";
			}
			else {
				html << "<tr><td><img class = \"additional\"  src=\"" << this->WebsiteAdditionalImage << "\"><td></tr>";
			}
		}

		html << "</tbody></table><br>\r\n";

		if(this->WebsiteLinkToMoreInfo == true)
			html << "<table class=sub_heading><td class=sub_heading><a href=\"info.html\">Informacje o stacji i startowisku</a></td></table>\r\n";
		if(this->Plot0Path.size() >= 4)
			html << "<br><img src=\"" << this->Plot0Path << "\">\r\n";
		if(this->Plot1Path.size() >= 4)
			html << "<br><img src=\"" << this->Plot1Path << "\">\r\n";
		if(this->Plot2Path.size() >= 4)
			html << "<br><img src=\"" << this->Plot2Path << "\">\r\n";
		if (this->Plot3Path.size() >= 5)
			html << "<br><img src=\"" << this->Plot3Path << "\">\r\n";
		if (this->Plot4Path.size() >= 5)
			html << "<br><img src=\"" << this->Plot4Path << "\">\r\n";
		html << "<p>Strona wygenerowana przy pomocy programu APRS2RRD w wersji " << SW_VER << " <a href=\"https://github.com/SP8EBC/aprs2rrd-se\">STRONA PROJEKTU</a> </p>";
		html << "<p>" << this->WebisteFooter << "</p>";
	}
	catch (...) {
		std::cout << "--- Unknown exception thrown during generating html page!" << std::endl;
	}

	std::cout << "--- Closing html file" << std::endl;
	html.close();

}

PlotType DataPresentation::SwitchPlotType(string input) {
	PlotType out = PlotType::N;

	if (input == "WIND_SPD_GST")
		out = PlotType::WIND_SPD_GST;
	else if (input == "WIND_DIR")
		out = PlotType::WIND_DIR;
	else if (input == "WIND_SPD")
		out = PlotType::WIND_SPD;
	else if (input == "WIND_GST")
		out = PlotType::WIND_GST;
	else if (input == "TEMPERATURE")
		out = PlotType::TEMPERATURE;
	else if (input == "QNH")
		out = PlotType::QNH;
	else if (input == "DIFF_TEMPERATURE")
		out = PlotType::DIFF_TEMPERATURE;
	else if (input == "DIFF_WIND_SPD")
		out = PlotType::DIFF_WIND_SPD;
	else if (input == "DIFF_WIND_GST")
		out = PlotType::DIFF_WIND_GST;
	else if (input == "DIFF_WIND_DIR")
		out = PlotType::DIFF_WIND_DIR;
	else if (input == "HUMIDITY")
		out = PlotType::HUMIDITY;
	else if (input == "VOLTAGE")
		out = PlotType::VOLTAGE;
	return out;

}

const std::string DataPresentation::RevSwitchPlotType(PlotType in) {

	if (in == PlotType::WIND_SPD_GST)
		return "WIND_SPD_GST";
	else if (in == PlotType::WIND_DIR)
		return "WIND_DIR";
	else if (in == PlotType::DIFF_TEMPERATURE)
		return "DIFF_TEMPERATURE";
	else if (in == PlotType::DIFF_WIND_DIR)
		return "DIFF_WIND_DIR";
	else if (in == PlotType::DIFF_WIND_SPD)
		return "DIFF_WIND_SPD";
	else if (in == PlotType::DIFF_WIND_GST)
		return "DIFF_WIND_GST";
	else if (in == PlotType::HUMIDITY)
		return "HUMIDITY";
	else if (in == PlotType::QNH)
		return "QNH";
	else if (in == PlotType::TEMPERATURE)
		return "TEMPERATURE";
	else if (in == PlotType::WIND_DIR)
		return "WIND_DIR";
	else if (in == PlotType::WIND_GST)
		return "WIND_GST";
	else if (in == PlotType::WIND_SPD)
		return "WIND_SPD";
	else if (in == PlotType::VOLTAGE)
		return "VOLTAGE";
	else if (in == PlotType::N)
		return "N";
	return "unknown";
}

PlotGraphType DataPresentation::SwitchPlotGraphType(string input) {
	PlotGraphType out;

	if (input == "AREA")
		out = PlotGraphType::AREA;
	if (input == "LINE2")
		out = PlotGraphType::LINE2;
	return out;
}

RRAType DataPresentation::SwitchRRAType(string input) {
	RRAType out;

	if (input == "AVERAGE")
		out = RRAType::AVERAGE;
	if (input == "LAST")
		out = RRAType::LAST;
	return out;
}

void DataPresentation::GetSecondarySource(const AprsWXData& aprsIS,
		const AprsWXData& serial, const AprsWXData& holfuy, const AprsWXData& zywiec, AprsWXData& out)
{
	switch(this->SecondarySource) {
	case WxDataSource::IS_PRIMARY: {
		if (aprsIS.is_primary)
			out = const_cast<AprsWXData&>(aprsIS);
		else
			return;
		break;
	}
	case WxDataSource::IS_SECONDARY: {
		if (aprsIS.is_secondary)
			out = const_cast<AprsWXData&>(aprsIS);
		else
			return;
		break;
	}
	case WxDataSource::SERIAL: out = const_cast<AprsWXData&>(serial); break;
	case WxDataSource::TELEMETRY: return; break;
	case WxDataSource::HOLFUY: out = const_cast<AprsWXData&>(holfuy); break;
	case WxDataSource::ZYWIEC: out = const_cast<AprsWXData&>(zywiec); break;
	case WxDataSource::UNKNOWN: break;
	}
}

const std::string DataPresentation::RevSwitchRRAType(RRAType in) {

	if (in == RRAType::AVERAGE)
		return "AVERAGE";
	else if (in == RRAType::LAST)
		return "LAST";
	return "unknown";
}

const std::string DataPresentation::RevSwitchPlotGraphType(PlotGraphType in) {

	if (in == PlotGraphType::AREA)
		return "AREA";
	else if (in == PlotGraphType::LINE2)
		return "LINE2";
	return "unknown";
}
