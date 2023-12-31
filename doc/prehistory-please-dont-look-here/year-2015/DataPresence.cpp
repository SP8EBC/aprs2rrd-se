#include "DataPresence.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <iostream>

#include "main.h"

DataPresence::DataPresence()
{
	this->Plot0Path.clear();
	this->Plot1Path.clear();
	this->Plot1Path.clear();
}

DataPresence::~DataPresence()
{
}

void DataPresence::FetchDataInRRD(AprsWXData* cInput) {
	char command[512];
	int currtimeint;
	time_t currtime;
	
	for (unsigned i = 0; i < this->vRRDFiles.size(); i++) {
		if (this->vRRDFiles[i].eType == TEMPERATURE) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->temperature);
			if (Debug == true)
				cout << command << endl;
		}
		else if (this->vRRDFiles[i].eType == QNH) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%d", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->pressure);	
			if (Debug == true)
				cout << command << endl;		
		}		
		else if (this->vRRDFiles[i].eType == WIND_DIR) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%d", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_direction);
			if (Debug == true)
				cout << command << endl;			
		}
		else if (this->vRRDFiles[i].eType == WIND_SPD) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_speed);	
			if (Debug == true)
				cout << command << endl;		
		}
		else if (this->vRRDFiles[i].eType == WIND_GST) {
			currtime =time(NULL);
			currtimeint = (int)currtime;
			memset(command, 0x00, sizeof(command));
			sprintf(command, "rrdtool update %s %d:%f", this->vRRDFiles[i].sPath.c_str(), currtimeint, cInput->wind_gusts);		
			if (Debug == true)
				cout << command << endl;	
		}
		system(command);
	}
}

void DataPresence::PlotGraphsFromRRD() {
	char command[1024];
	int currtimeint;
	unsigned i;
	time_t currtime;
	
	currtime =time(NULL);
	currtimeint = (int)currtime;
	
	if (Debug == true) {
		cout << "----- Liczba wykresów do wygenerowania: " <<  this->vPNGFiles.size() << endl;
	}
	
	for (i = 0; i < this->vPNGFiles.size(); i++) {
		memset(command, 0x00, sizeof(command));
		if (this->vPNGFiles[i].DoubleDS == false)
			sprintf(command, "rrdtool graph %s -w %d -h %d -t \"%s\" -v \"%s\" -l %.4e -u %.4e -r --right-axis 1:0 --right-axis-label \"%s\" --x-grid MINUTE:10:HOUR:1:HOUR:1:0:%%H:00 --start %d --end %d DEF:%s=%s:%s:AVERAGE %s:%s#%.6x", \
					this->vPNGFiles[i].sPath.c_str(), this->vPNGFiles[i].Width, this->vPNGFiles[i].Height, \
					this->vPNGFiles[i].Title.c_str(), this->vPNGFiles[i].Axis.c_str() , this->vPNGFiles[i].MinScale, \
					this->vPNGFiles[i].MaxScale, this->vPNGFiles[i].Axis.c_str(), currtimeint-36000, currtimeint, \
					this->vPNGFiles[i].sDS0Name.c_str(), this->vPNGFiles[i].sDS0Path.c_str(), this->vPNGFiles[i].sDS0Name.c_str(), \
					this->RevSwitchPlotGraphType(this->vPNGFiles[i].eDS0PlotType), \
					this->vPNGFiles[i].sDS0Name.c_str(), this->vPNGFiles[i].DS0PlotColor);
		else
			sprintf(command, "rrdtool graph %s -w %d -h %d -t \"%s\" -v \"%s\" -l %.4e -u %.4e -r --right-axis 1:0 --right-axis-label \"%s\" --x-grid MINUTE:10:HOUR:1:HOUR:1:0:%%H:00 --start %d --end %d DEF:%s=%s:%s:AVERAGE DEF:%s=%s:%s:AVERAGE %s:%s#%.6x:%s %s:%s#%.6x:%s", \
					this->vPNGFiles[i].sPath.c_str(), this->vPNGFiles[i].Width, this->vPNGFiles[i].Height, \
					this->vPNGFiles[i].Title.c_str(), this->vPNGFiles[i].Axis.c_str() , this->vPNGFiles[i].MinScale, \
					this->vPNGFiles[i].MaxScale, this->vPNGFiles[i].Axis.c_str(), currtimeint-36000, currtimeint, \
					this->vPNGFiles[i].sDS0Name.c_str(), this->vPNGFiles[i].sDS0Path.c_str(), this->vPNGFiles[i].sDS0Name.c_str(), \
					this->vPNGFiles[i].sDS1Name.c_str(), this->vPNGFiles[i].sDS1Path.c_str(), this->vPNGFiles[i].sDS1Name.c_str(), \
					this->RevSwitchPlotGraphType(this->vPNGFiles[i].eDS0PlotType), this->vPNGFiles[i].sDS0Name.c_str(), \
					this->vPNGFiles[i].DS0PlotColor, this->vPNGFiles[i].sDS0Label.c_str(), \
					this->RevSwitchPlotGraphType(this->vPNGFiles[i].eDS1PlotType), this->vPNGFiles[i].sDS1Name.c_str(), \
					this->vPNGFiles[i].DS1PlotColor, this->vPNGFiles[i].sDS1Label.c_str());
					
			
		if (Debug == true)
			cout << command << endl;
		system(command);
	}
}

void DataPresence::GenerateWebiste(AprsWXData* WX) {
	FILE* plik;
	time_t currtime;
    struct tm* local;
	currtime =time(NULL);
	local = localtime(&currtime);
	
	static const char mon_name[][13] = {
    "Stycznia", "Lutego", "Marca", "Kwietnia", "Maja", "Czerwca",
    "Lipca", "Sierpnia", "Wrzesnia", "Pazdziernika", "Listopada", "Grudnia"
    };
	
	if ((plik=fopen(this->WebsitePath.c_str(),"wt")) != NULL) {
		fprintf(plik, " <!DOCTYPE html><HTML><head>\r\n<TITLE>%s</TITLE> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">", this->WebsiteTitle.c_str());
		fprintf(plik, "<style type=\"text/css\">td.dupa {	text-align: right;} td.dupa-druga {text-align: left;} td.dupa-trzecia{text-align: center;} table.dupa-trzecia {width: 870px;} p.wagina {font-style: italic;}</style></head>");
		fprintf(plik, "<P><H2>%s</H2></P>\r\n", this->WebsiteHeadingTitle.c_str());
		fprintf(plik, "<table><tr><td class=dupa><b>Aktualna Prędkość Wiatru (średnia za 3 minuty):</b></td><td class=dupa-druga id=srednia> %.1f m/s </td></tr>\r\n", WX->wind_speed);
		fprintf(plik, "<tr><td class=dupa><b>Aktualne Porywy (maksymalna szybkość przez ostatnie 3 minuty):</b></td><td class=dupa-druga id=porywy> %.1f m/s </td></tr>", WX->wind_gusts);
		fprintf(plik, "<tr><td class=dupa><b>Aktualny Kierunek Meteo:</b></td><td class=dupa-druga id=kierunek> %d stopni ", WX->wind_direction);
		if (WX->wind_direction <= 11 && WX->wind_direction >= 349)
			fprintf(plik, "- z północy");
		else if (WX->wind_direction <= 34 && WX->wind_direction > 11)
			fprintf(plik, "- z północy-północnego wschodu");
		else if (WX->wind_direction <= 56 && WX->wind_direction > 34)
			fprintf(plik, "- z północnego wschodu");
		else if (WX->wind_direction <= 79 && WX->wind_direction > 56)
			fprintf(plik, "- ze wschodu-północnego wschodu");
		else if (WX->wind_direction <= 101 && WX->wind_direction > 79)
			fprintf(plik, "- ze wschodu");
		else if (WX->wind_direction <= 124 && WX->wind_direction > 101)
			fprintf(plik, "- ze wschodu-południowego wschodu");
		else if (WX->wind_direction <= 146 && WX->wind_direction > 124)
			fprintf(plik, "- z południowego wschodu");
		else if (WX->wind_direction <= 169 && WX->wind_direction > 146)
			fprintf(plik, "- z południa-południowego wschodu");
		else if (WX->wind_direction <= 191 && WX->wind_direction > 169)
			fprintf(plik, "- z południa");
		else if (WX->wind_direction <= 214 && WX->wind_direction > 191)
			fprintf(plik, "- z południa-południowego zachodu");
		else if (WX->wind_direction <= 236 && WX->wind_direction > 214)
			fprintf(plik, "- z południowego zachodu");
		else if (WX->wind_direction <= 259 && WX->wind_direction > 236)
			fprintf(plik, "- z zachodu-południowego zachodu");
		else if (WX->wind_direction <= 281 && WX->wind_direction > 259)
			fprintf(plik, "- z zachodu");
		else if (WX->wind_direction <= 304 && WX->wind_direction > 281)
			fprintf(plik, "- z zachodu-północnego zachodu");
		else if (WX->wind_direction <= 327 && WX->wind_direction > 304)
			fprintf(plik, "- z północnego zachodu");
		else if (WX->wind_direction <= 349 && WX->wind_direction > 327)
			fprintf(plik, "- z północy-północnego zachodu");
		else;
		fprintf(plik, "</td></tr>");
		if (this->PrintTemperature)
			fprintf(plik, "<tr><td class=dupa><b>Temperatura:</b></td><td class=dupa-druga id=temperatura> %f Stopni Celcjusza ", WX->temperature);
		if (this->PrintPressure)
			fprintf(plik, "<tr><td class=dupa><b>Ciśnienie:</b></td><td class=dupa-druga id=Ciśnienie> %d hPa ", WX->pressure);
		fprintf(plik, "</table>");
		local = localtime(&currtime);
		fprintf(plik, "<P class=wagina><b>Czas ostatniej aktualizacji: %3d %s %.2d:%.2d:%.2d</b> </P>", local->tm_mday, mon_name[local->tm_mon], local->tm_hour, local->tm_min, local->tm_sec);
		fprintf(plik, "<table class=dupa-trzecia><td class=dupa-trzecia>%s</td></table>\r\n", this->WebsiteSubHeading.c_str());
		if(this->WebsiteLinkToMoreInfo == true)
			fprintf(plik, "<table class=dupa-trzecia><td class=dupa-trzecia><a href=\"info.html\">Informacje o stacji i startowisku</a></td></table>\r\n");
		if(this->Plot0Path.size() >= 4)
			fprintf(plik, "<br><img src=\"%s\">\r\n", this->Plot0Path.c_str());
		if(this->Plot1Path.size() >= 4)
			fprintf(plik, "<br><img src=\"%s\">\r\n", this->Plot1Path.c_str());
		if(this->Plot2Path.size() >= 4)
			fprintf(plik, "<br><img src=\"%s\">\r\n", this->Plot2Path.c_str());
		fprintf(plik, "<p>Strona wygenerowna przy pomocy programu APRS2RRD-0.8.5-21072015 <a href=\"http://ebc41.elektroda.eu/aprs/aprs2rrd/changelog\">CHANGELOG</a></p>");
		fprintf(plik, "<p>%s</p>", this->WebisteFooter.c_str());
		fclose(plik);
	}
}

PlotType DataPresence::SwitchPlotType(string input) {		
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

const char* DataPresence::RevSwitchPlotType(PlotType in) {		

	if (in == WIND_SPD_GST)
		return "WIND_SPD_GST";
	else if (in == WIND_DIR)
		return "WIND_DIR";
	else if (in == N)
		return "N";
	return "unknown";
}

PlotGraphType DataPresence::SwitchPlotGraphType(string input) {
	PlotGraphType out;
	
	if (input == "AREA")
		out = AREA;
	if (input == "LINE2")
		out = LINE2;
	return out;
}

const char* DataPresence::RevSwitchPlotGraphType(PlotGraphType in) {
	
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
}
