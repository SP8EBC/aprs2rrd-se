#include "DataPresentation.h"

#include "SOFTWARE_VERSION.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

#include <boost/date_time/local_time/local_time.hpp>

#include <fstream>
//#include "main.h"

PlotFileDefinition::PlotFileDefinition() {
	this->ScaleStep = 0;
	this->LabelStep = 0;
	this->Exponent = -1;
}

DataPresentation::DataPresentation()
{
	this->Plot0Path.clear();
	this->Plot1Path.clear();
	this->Plot2Path.clear();
	this->Plot3Path.clear();
}

DataPresentation::~DataPresentation()
{
}

void DataPresentation::FetchDataInRRD(AprsWXData* cInput) {
	char command[512];
	int currtimeint;
	time_t currtime;

	if (cInput == nullptr)
		return;

	if (!cInput->valid)
		return;

	for (unsigned i = 0; i < this->vRRDFiles.size(); i++) {
		if (this->vRRDFiles[i].eType == TEMPERATURE && cInput->useTemperature == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->temperature);
			if (this->DebugOutput == true)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == QNH && cInput->usePressure == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%d", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->pressure);
			if (this->DebugOutput == true)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == WIND_DIR && cInput->useWind == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%d", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_direction);
			if (this->DebugOutput == true)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == WIND_SPD && cInput->useWind == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_speed);
			if (this->DebugOutput == true)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == WIND_GST && cInput->useWind == true) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_gusts);
			if (this->DebugOutput == true)
				cout << command << endl;
		}
		else continue;

		system(command);
		memset(command, 0x00, sizeof(command));
		sprintf(command, "rrdtool dump %s > %s.dmp", this->vRRDFiles[i].sPath.c_str(), this->vRRDFiles[i].sPath.c_str());
		system(command);
	}
}

void DataPresentation::PlotGraphsFromRRD() {
	char command[1024];
	int currtimeint;
	unsigned i;
	time_t currtime;

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
		cout << "--- DataPresentation::PlotGraphsFromRRD:115 -  Count of plots to be generated: " <<  this->vPNGFiles.size() << endl;
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
             xgrid << "MINUTE:30:HOUR:4:HOUR:4:0:%a-%H:00";
        else
             xgrid << "MINUTE:60:HOUR:6:HOUR:6:0:%a-%H:00";

	if (this->vPNGFiles[i].ScaleStep > 0 && this->vPNGFiles[i].LabelStep > 0) {
		scalestep << " --y-grid " << this->vPNGFiles[i].ScaleStep << ":" << this->vPNGFiles[i].LabelStep << " ";
	}
	else scalestep << " ";

	if (this->vPNGFiles[i].Exponent != -1)
		exp << " -X " << this->vPNGFiles[i].Exponent;
	else
		exp << " ";

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
		system(command);
	}
}

void DataPresentation::GenerateWebiste(AprsWXData* WX) {

	uint8_t windspeedPrecision = 3;
	uint8_t windgustsPrecision = 3;
	uint8_t temperaturePrecision = 3;

	boost::posix_time::ptime localtime(boost::date_time::second_clock<boost::posix_time::ptime>::local_time());

	boost::posix_time::time_facet * formatter = new boost::posix_time::time_facet();
	formatter->format("%d %B %H:%M");

	//boost::filesystem::path html{this->WebsitePath.c_str()};
	std::ofstream html;

	html.open(this->WebsitePath.c_str(), ios::out | ios::trunc);

	if (!html.is_open()) {
		std::cout << "--- DataPresentation::GenerateWebiste:203 - Html file cannot by opend because of unknown reason" << std::endl;
		return;
	}

	if (!html.good()) {
		std::cout << "--- DataPresentation::GenerateWebiste:208 - Something is wrong with the html file!" << std::endl;
		return;
	}


	//static const char mon_name[][13] = {
    //"Stycznia", "Lutego", "Marca", "Kwietnia", "Maja", "Czerwca",
    //"Lipca", "Sierpnia", "Wrzesnia", "Pazdziernika", "Listopada", "Grudnia"
    //};

	//html.precision(3);

	std::cout << "--- DataPresentation::GenerateWebiste:220 - Html file opened" << std::endl;

	try {
		html << " <!DOCTYPE html><HTML><head>\r\n<TITLE>" << this->WebsiteTitle << "</TITLE> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
		html << "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></head>";
		html << "<P><H2>" << this->WebsiteHeadingTitle << "</H2></P>\r\n";
		html << "<table><tr><td class=table_caption><b>Aktualna Prędkość Wiatru (średnia za 3 minuty):</b></td><td class=table_value id=srednia> "<< std::setprecision(windspeedPrecision) << WX->wind_speed << " m/s </td></tr>\r\n";
		html << "<tr><td class=table_caption><b>Aktualne Porywy (maksymalna szybkość przez ostatnie 3 minuty):</b></td><td class=table_value id=porywy> " << std::setprecision(windgustsPrecision) <<   WX->wind_gusts << " m/s </td></tr>";
		html << "<tr><td class=table_caption><b>Meteorologiczny Kierunek Wiatru:</b></td><td class=table_value id=kierunek> " << WX->wind_direction << " stopni ";

		if (WX->wind_direction <= 11 && WX->wind_direction >= 349)
			html << "- z północy";
		else if (WX->wind_direction <= 34 && WX->wind_direction > 11)
			html << "- z północy-północnego wschodu";
		else if (WX->wind_direction <= 56 && WX->wind_direction > 34)
			html << "- z północnego wschodu";
		else if (WX->wind_direction <= 79 && WX->wind_direction > 56)
			html << "- ze wschodu-północnego wschodu";
		else if (WX->wind_direction <= 101 && WX->wind_direction > 79)
			html << "- ze wschodu";
		else if (WX->wind_direction <= 124 && WX->wind_direction > 101)
			html << "- ze wschodu-południowego wschodu";
		else if (WX->wind_direction <= 146 && WX->wind_direction > 124)
			html << "- z południowego wschodu";
		else if (WX->wind_direction <= 169 && WX->wind_direction > 146)
			html << "- z południa-południowego wschodu";
		else if (WX->wind_direction <= 191 && WX->wind_direction > 169)
			html << "- z południa";
		else if (WX->wind_direction <= 214 && WX->wind_direction > 191)
			html << "- z południa-południowego zachodu";
		else if (WX->wind_direction <= 236 && WX->wind_direction > 214)
			html << "- z południowego zachodu";
		else if (WX->wind_direction <= 259 && WX->wind_direction > 236)
			html <<"- z zachodu-południowego zachodu";
		else if (WX->wind_direction <= 281 && WX->wind_direction > 259)
			html << "- z zachodu";
		else if (WX->wind_direction <= 304 && WX->wind_direction > 281)
			html << "- z zachodu-północnego zachodu";
		else if (WX->wind_direction <= 327 && WX->wind_direction > 304)
			html << "- z północnego zachodu";
		else if (WX->wind_direction <= 349 && WX->wind_direction > 327)
			html << "- z północy-północnego zachodu";
		else;

		html << "</td></tr>";

		if (this->PrintTemperature)
			html << "<tr><td class=table_caption><b>Temperatura:</b></td><td class=table_value id=temperatura> " << std::setprecision(temperaturePrecision) << WX->temperature << " Stopni Celcjusza ";
		if (this->PrintPressure)
			html << "<tr><td class=table_caption><b>Ciśnienie:</b></td><td class=table_value id=Ciśnienie> " << WX->pressure << " hPa ";
		if (this->PrintHumidity)
			html << "<tr><td class=table_caption><b>Wilgotność:</b></td><td class=table_value id=wilgotnosc> " << WX->humidity << " %% ";

		html << "</table>";

		html.imbue(std::locale(std::locale::classic(), formatter));
		html << "<P class=last_update><b>Czas ostatniej aktualizacji: " << localtime << "</b> </P>";
		html << "<table class=sub_heading><td class=sub_heading>" << this->WebsiteSubHeading << "</td></table>\r\n";

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
	PlotType out;

	if (input == "WIND_SPD_GST")
		out = WIND_SPD_GST;
	if (input == "WIND_DIR")
		out = WIND_DIR;
	if (input == "WIND_SPD")
		out = WIND_SPD;
	if (input == "WIND_GST")
		out = WIND_GST;
	if (input == "TEMPERATURE")
		out = TEMPERATURE;
	if (input == "QNH")
		out = QNH;
	return out;

}

const std::string DataPresentation::RevSwitchPlotType(PlotType in) {

	if (in == WIND_SPD_GST)
		return "WIND_SPD_GST";
	else if (in == WIND_DIR)
		return "WIND_DIR";
	else if (in == N)
		return "N";
	return "unknown";
}

PlotGraphType DataPresentation::SwitchPlotGraphType(string input) {
	PlotGraphType out;

	if (input == "AREA")
		out = AREA;
	if (input == "LINE2")
		out = LINE2;
	return out;
}

RRAType DataPresentation::SwitchRRAType(string input) {
	RRAType out;

	if (input == "AVERAGE")
		out = AVERAGE;
	if (input == "LAST")
		out = LAST;
	return out;
}

const std::string DataPresentation::RevSwitchRRAType(RRAType in) {

	if (in == AVERAGE)
		return "AVERAGE";
	else if (in == LAST)
		return "LAST";
	return "unknown";
}

const std::string DataPresentation::RevSwitchPlotGraphType(PlotGraphType in) {

	if (in == AREA)
		return "AREA";
	else if (in == LINE2)
		return "LINE2";
	return "unknown";
}

void RRDFileDefinition::Zero(void) {
	this->eType = N;
	this->sPath.clear();
}

void PlotFileDefinition::Zero(void) {
	this->eType = N;
	this->sPath.clear();
	this->DS0PlotColor = 0;
	this->DS1PlotColor = 0;
	this->Height = 0;
	this->Width = 0;
	this->Title.clear();
	this->eDS0PlotType = NN;
	this->eDS1PlotType = NN;
	this->sDS0Name.clear();
	this->sDS1Name.clear();
	this->sDS0Path.clear();
	this->sDS1Path.clear();
	this->ScaleStep = 0;
	this->LabelStep = 0;
	this->Exponent = -1;
}
