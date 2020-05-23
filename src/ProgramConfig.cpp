/*
 * ProgramConfig.cpp
 *
 *  Created on: 04.12.2018
 *      Author: mateusz
 */

#include "ProgramConfig.h"

#include <boost/algorithm/string.hpp>


ProgramConfig::ProgramConfig(std::string fn) : configFilename(fn) {
	// TODO Auto-generated constructor stub

}

ProgramConfig::~ProgramConfig() {
	// TODO Auto-generated destructor stub
}

void ProgramConfig::parseFile() {
	config.readFile("config.conf");

//	root = config.getRoot();
}

void ProgramConfig::getDbConfig(MySqlConnInterface& db) {

	libconfig::Setting &rRoot = config.getRoot();

	libconfig::Setting &rBaza = rRoot["MySQL"];

	rBaza.lookupValue("Enable", db.enable);
	if (db.enable) {
		rBaza.lookupValue("IpAddress", db.IP);
		rBaza.lookupValue("Port", db.port);
		rBaza.lookupValue("DbName", db.dbName);
		rBaza.lookupValue("DbUser", db.Username);
		rBaza.lookupValue("DbPassword", db.Password);
		rBaza.lookupValue("DbTable", db.tableName);
		rBaza.lookupValue("ExecBeforeInsert", db.execBeforeInsert);
		rBaza.lookupValue("ExecBeforeInsertPath", db.execBeforeInsertPath);

		try {
			rBaza.lookupValue("SchemaV1", db.schema_v1);
			rBaza.lookupValue("SchemaV2", db.schema_v2);

		}
		catch (libconfig::SettingNotFoundException &ex) {
			db.schema_v1 = true;
			db.schema_v2 = false;
		}
	}

}

void ProgramConfig::getAprsThreadConfig(AprsThreadConfig& aprs) {
	libconfig::Setting &rRoot = config.getRoot();

	libconfig::Setting &rAprsIS = rRoot["AprsIS"];

	rAprsIS.lookupValue("Enable", aprs.enable);
	rAprsIS.lookupValue("StationCall", aprs.StationCall);
	rAprsIS.lookupValue("StationSSID", aprs.StationSSID);
	rAprsIS.lookupValue("ServerAddr", aprs.ServerURL);
	rAprsIS.lookupValue("ServerPort", aprs.ServerPort);
	rAprsIS.lookupValue("MyCALL", aprs.Call);
	rAprsIS.lookupValue("MyPasswd", aprs.Passwd);

	try {
		rAprsIS.lookupValue("SecondaryCall", aprs.SecondaryCall);
		rAprsIS.lookupValue("SecondarySSID", aprs.SecondarySSID);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		aprs.SecondaryCall = "";
		aprs.SecondarySSID = 0xFF;
	}
}

void ProgramConfig::getSerialConfig(SerialConfig& serial) {
	libconfig::Setting &rRoot = config.getRoot();

	try {
		libconfig::Setting &rAprsIS = rRoot["Serial"];

		rAprsIS.lookupValue("Enable", serial.enable);
		rAprsIS.lookupValue("StationCall", serial.call);
		rAprsIS.lookupValue("StationSSID", serial.ssid);
		rAprsIS.lookupValue("Baudrate", serial.baudrate);
		rAprsIS.lookupValue("Port", serial.serialPort);
		rAprsIS.lookupValue("CaptureAll", serial.captureAll);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		serial.enable = false;
		serial.captureAll = false;
	}


}

void ProgramConfig::getDataPresentationConfig(DataPresentation& data, int& rrdCount, int& plotCount) {
	RRDFileDefinition sVectorRRDTemp;
	libconfig::Setting &rRoot = config.getRoot();
	std::string temp;
	//cVectorPNGTemp

	libconfig::Setting &rRRD = rRoot["RRD"];
	rrdCount = rRRD.getLength();

	for (int ii = 0; ii < rrdCount; ii++) {
		sVectorRRDTemp.Zero();
		std::string temp;

		rRRD[ii].lookupValue("Type", temp);
		sVectorRRDTemp.eType = data.SwitchPlotType(temp);
		rRRD[ii].lookupValue("Path", sVectorRRDTemp.sPath);
		data.vRRDFiles.push_back(sVectorRRDTemp);

		}

	libconfig::Setting &rWWW = rRoot["Website"];

	rWWW.lookupValue("IndexHtml", data.WebsitePath);
	rWWW.lookupValue("Title", data.WebsiteTitle);
	rWWW.lookupValue("HeadingTitle", data.WebsiteHeadingTitle);
	rWWW.lookupValue("SubHeading", data.WebsiteSubHeading);
	rWWW.lookupValue("LinkToMoreInfo", data.WebsiteLinkToMoreInfo);
	rWWW.lookupValue("Footer", data.WebisteFooter);
	rWWW.lookupValue("Plot0", data.Plot0Path);
	rWWW.lookupValue("Plot1", data.Plot1Path);
	rWWW.lookupValue("Plot2", data.Plot2Path);
	try {
		rWWW.lookupValue("Plot3", data.Plot3Path);
		rWWW.lookupValue("Plot4", data.Plot4Path);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		data.Plot3Path = "";
		data.Plot4Path = "";
	}
	rWWW.lookupValue("PrintTemperature", data.PrintTemperature);
	rWWW.lookupValue("PrintPressure", data.PrintPressure);
	rWWW.lookupValue("PrintHumidity", data.PrintHumidity);
	rWWW.lookupValue("DirectionCorrection", (int32_t&)data.directionCorrection);

	try {
		rWWW.lookupValue("PrintTwoSourcesInTable", data.PrintTwoSourcesInTable);
		rWWW.lookupValue("SecondaryLabel", data.SecondaryLabel);
		rWWW.lookupValue("PrimaryLabel", data.PrimaryLabel);
		rWWW.lookupValue("SecondarySource", temp);
		data.SecondarySource = ProgramConfig::wxDataSourceFromStr(temp);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		data.PrintTwoSourcesInTable = false;
	}

	libconfig::Setting &rPlots = rRoot["Plots"];
	plotCount = rPlots.getLength();
	for (int ii = 0; ii < plotCount; ii++) {
		string temp;

		cVectorPNGTemp.Zero();

		rPlots[ii].lookupValue("Type", temp);
		cVectorPNGTemp.eType = data.SwitchPlotType(temp);
		rPlots[ii].lookupValue("Path", cVectorPNGTemp.sPath);
		rPlots[ii].lookupValue("DS0", cVectorPNGTemp.sDS0Path);
		rPlots[ii].lookupValue("DS0Name", cVectorPNGTemp.sDS0Name);
		rPlots[ii].lookupValue("DS0RRAType", temp);
		cVectorPNGTemp.eDS0RRAType = data.SwitchRRAType(temp);
		rPlots[ii].lookupValue("DS0PlotType", temp);
		cVectorPNGTemp.eDS0PlotType = data.SwitchPlotGraphType(temp);
		rPlots[ii].lookupValue("DS0PlotColor", cVectorPNGTemp.DS0PlotColor);
		rPlots[ii].lookupValue("DS0Label", cVectorPNGTemp.sDS0Label);

		rPlots[ii].lookupValue("DS1", cVectorPNGTemp.sDS1Path);
		rPlots[ii].lookupValue("DS1Name", cVectorPNGTemp.sDS1Name);
		rPlots[ii].lookupValue("DS1RRAType", temp);
		cVectorPNGTemp.eDS1RRAType = data.SwitchRRAType(temp);
		rPlots[ii].lookupValue("DS1PlotType", temp);
		cVectorPNGTemp.eDS1PlotType = data.SwitchPlotGraphType(temp);
		rPlots[ii].lookupValue("DS1PlotColor", cVectorPNGTemp.DS1PlotColor);
		rPlots[ii].lookupValue("DS1Label", cVectorPNGTemp.sDS1Label);

		rPlots[ii].lookupValue("Title", cVectorPNGTemp.Title);
		rPlots[ii].lookupValue("Width", cVectorPNGTemp.Width);
		rPlots[ii].lookupValue("Height", cVectorPNGTemp.Height);
		rPlots[ii].lookupValue("Axis", cVectorPNGTemp.Axis);

		rPlots[ii].lookupValue("MinScale", cVectorPNGTemp.MinScale);
		rPlots[ii].lookupValue("MaxScale", cVectorPNGTemp.MaxScale);

        cVectorPNGTemp.timeScaleLn = 600;
		rPlots[ii].lookupValue("TimeScaleLen", cVectorPNGTemp.timeScaleLn);

		rPlots[ii].lookupValue("ScaleStep", cVectorPNGTemp.ScaleStep);
		rPlots[ii].lookupValue("LabelStep", cVectorPNGTemp.LabelStep);

		rPlots[ii].lookupValue("Exponent", cVectorPNGTemp.Exponent);

		if (cVectorPNGTemp.sDS1Name.length() == 0)
			cVectorPNGTemp.DoubleDS = false;
		else
			cVectorPNGTemp.DoubleDS = true;

		data.vPNGFiles.push_back(cVectorPNGTemp);
	}
}

void ProgramConfig::getTelemetryConfig(Telemetry& data, bool& useAsTemperature) {

	config.lookupValue("FifthTelemAsTemperature", useAsTemperature);
	config.lookupValue("TelemAScaling", data.ch5a);
	config.lookupValue("TelemBScaling", data.ch5b);
	config.lookupValue("TelemCScaling", data.ch5c);
}

std::string ProgramConfig::getStationName() {

	std::string out;

	if (this->stationName.length() < 3) {
		try {
			config.lookupValue("StationName", this->stationName);
		}
		catch (libconfig::SettingNotFoundException &ex) {
			out = "";
		}
	}
	else {
		;
	}

	out = this->stationName;
	return out;
}

bool ProgramConfig::getDebug() {
	bool out = false;
	config.lookupValue("Debug", out);
	this->Debug = out;

	return out;
}

bool ProgramConfig::getDebugToFile() {
	bool out = false;
	config.lookupValue("DebugToFile", out);
	this->DebugToFile = out;

	return out;
}

std::string ProgramConfig::getDebugLogFn() {
	std::string out;

	config.lookupValue("DebugLogFile", out);
	this->DebugLogFn = out;

	return out;

}

void ProgramConfig::configureLogOutput() {
	if (this->Debug == true) {
		cout << "--- Tryb debugowania włączony" << endl;
		cout << endl;

		if (DebugToFile == true && this->DebugLogFn.length() > 3) {
				cout << "--- Wyjście z konsoli przekierownane do pliku: " << this->DebugLogFn;
				//fDebug.open(LogFile.c_str());
				//cout.rdbuf(fDebug.rdbuf());
				freopen(this->DebugLogFn.c_str(), "w", stdout);
				freopen(this->DebugLogFn.c_str(), "w", stderr);
		}
	}
}

void ProgramConfig::getDataSourceConfig(DataSourceConfig& config_out) {
	libconfig::Setting &root = config.getRoot();

	try {
		libconfig::Setting &aprsIS = root["AprsIS"];

		aprsIS.lookupValue("StationCall", config_out.primaryCall);
		aprsIS.lookupValue("StationSSID", config_out.primarySsid);

		aprsIS.lookupValue("SecondaryCall", config_out.secondaryCall);
		aprsIS.lookupValue("SecondarySSID", config_out.secondarySsid);

		config_out.globalBackup = this->getGlobalBackup();
		config_out.humidity = this->getHumiditySource();
		config_out.pressure = this->getPressureSource();
		config_out.rain = this->getRainSource();
		config_out.temperature = this->getTemperatureSource();
		config_out.wind = this->getWindSource();
	}
	catch (libconfig::SettingNotFoundException &ex) {
		std::cout << "--- ProgramConfig::getDataSourceConfig:294 - Error in data sources configuration. Using defaul values!" << std::endl;

		config_out.globalBackup = WxDataSource::IS_PRIMARY;
		config_out.humidity = WxDataSource::IS_PRIMARY;
		config_out.pressure = WxDataSource::IS_PRIMARY;
		config_out.rain = WxDataSource::IS_PRIMARY;
		config_out.temperature = WxDataSource::IS_PRIMARY;
		config_out.wind = WxDataSource::IS_PRIMARY;
	}

	try {
		libconfig::Setting &serial = root["Serial"];

		serial.lookupValue("StationCall", config_out.serialCall);
		serial.lookupValue("StationSsid", config_out.secondarySsid);
		serial.lookupValue("CaptureAll", config_out.serialCaptureAll);

	}
	catch (libconfig::SettingNotFoundException &ex) {
		std::cout << "--- ProgramConfig::getDataSourceConfig:313 - No Serial KISS communcation is configured." << std::endl;

	}

	try {
		libconfig::Setting &h = root["Holfuy"];

		h.lookupValue("StationId", config_out.holfuyNumber);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		std::cout << "--- ProgramConfig::getDataSourceConfig:323 - No Holfuy communcation is configured." << std::endl;

	}
}

void ProgramConfig::getHolfuyConfig(HolfuyClientConfig& config_out) {
	libconfig::Setting &root = config.getRoot();

	try {
		libconfig::Setting &h = root["Holfuy"];

		h.lookupValue("StationId", config_out.stationId);
		h.lookupValue("ApiPassword", config_out.apiPassword);
		h.lookupValue("Enable", config_out.enable);
		h.lookupValue("DumpIntoMysql", config_out.dumpIntoMysql);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		config_out.enable = false;
	}
}

WxDataSource ProgramConfig::getTemperatureSource() {
	WxDataSource out = WxDataSource::UNKNOWN;

	libconfig::Setting &root = config.getRoot();
	libconfig::Setting &sources = root["Sources"];

	std::string value = "";

	sources.lookupValue("Temperature", value);

	boost::to_upper(value);

	switch (swstring(value.c_str())) {
	case swstring("APRSIS"): out = WxDataSource::IS_PRIMARY; break;
	case swstring("SECONDARYAPRSIS"): out = WxDataSource::IS_SECONDARY;  break;
	case swstring("TELEMETRY"): out = WxDataSource::TELEMETRY;  break;
	case swstring("SERIAL"): out = WxDataSource::SERIAL;  break;
	case swstring("HOLFUY"): out = WxDataSource::HOLFUY;  break;
	default: break;
	}

	return out;
}

WxDataSource ProgramConfig::getPressureSource() {
	WxDataSource out = WxDataSource::UNKNOWN;

	libconfig::Setting &root = config.getRoot();
	libconfig::Setting &sources = root["Sources"];

	std::string value = "";

	sources.lookupValue("Pressure", value);

	boost::to_upper(value);

	switch (swstring(value.c_str())) {
	case swstring("APRSIS"): out = WxDataSource::IS_PRIMARY; break;
	case swstring("SECONDARYAPRSIS"): out = WxDataSource::IS_SECONDARY;  break;
	case swstring("TELEMETRY"): out = WxDataSource::TELEMETRY;  break;
	case swstring("SERIAL"): out = WxDataSource::SERIAL;  break;
	case swstring("HOLFUY"): out = WxDataSource::HOLFUY;  break;
	default: break;
	}

	return out;
}

WxDataSource ProgramConfig::getWindSource() {
	WxDataSource out = WxDataSource::UNKNOWN;

	libconfig::Setting &root = config.getRoot();
	libconfig::Setting &sources = root["Sources"];

	std::string value = "";

	sources.lookupValue("Wind", value);

	boost::to_upper(value);

	switch (swstring(value.c_str())) {
	case swstring("APRSIS"): out = WxDataSource::IS_PRIMARY; break;
	case swstring("SECONDARYAPRSIS"): out = WxDataSource::IS_SECONDARY;  break;
	case swstring("TELEMETRY"): out = WxDataSource::TELEMETRY;  break;
	case swstring("SERIAL"): out = WxDataSource::SERIAL;  break;
	case swstring("HOLFUY"): out = WxDataSource::HOLFUY;  break;
	default: break;
	}

	return out;
}

WxDataSource ProgramConfig::getRainSource() {
	WxDataSource out = WxDataSource::UNKNOWN;

	libconfig::Setting &root = config.getRoot();
	libconfig::Setting &sources = root["Sources"];

	std::string value = "";

	sources.lookupValue("Rain", value);

	boost::to_upper(value);

	switch (swstring(value.c_str())) {
	case swstring("APRSIS"): out = WxDataSource::IS_PRIMARY; break;
	case swstring("SECONDARYAPRSIS"): out = WxDataSource::IS_SECONDARY;  break;
	case swstring("TELEMETRY"): out = WxDataSource::TELEMETRY;  break;
	case swstring("SERIAL"): out = WxDataSource::SERIAL;  break;
	case swstring("HOLFUY"): out = WxDataSource::HOLFUY;  break;
	default: break;
	}

	return out;
}

WxDataSource ProgramConfig::getHumiditySource() {
	WxDataSource out = WxDataSource::UNKNOWN;

	libconfig::Setting &root = config.getRoot();
	libconfig::Setting &sources = root["Sources"];

	std::string value = "";

	sources.lookupValue("Humidity", value);

	boost::to_upper(value);

	switch (swstring(value.c_str())) {
	case swstring("APRSIS"): out = WxDataSource::IS_PRIMARY; break;
	case swstring("SECONDARYAPRSIS"): out = WxDataSource::IS_SECONDARY;  break;
	case swstring("TELEMETRY"): out = WxDataSource::TELEMETRY;  break;
	case swstring("SERIAL"): out = WxDataSource::SERIAL;  break;
	case swstring("HOLFUY"): out = WxDataSource::HOLFUY;  break;
	default: break;
	}

	return out;
}

WxDataSource ProgramConfig::getGlobalBackup() {
	WxDataSource out = WxDataSource::UNKNOWN;

	libconfig::Setting &root = config.getRoot();
	libconfig::Setting &sources = root["Sources"];

	std::string value = "";

	sources.lookupValue("BackupForEverything", value);

	boost::to_upper(value);

	switch (swstring(value.c_str())) {
	case swstring("APRSIS"): out = WxDataSource::IS_PRIMARY; break;
	case swstring("SECONDARYAPRSIS"): out = WxDataSource::IS_SECONDARY;  break;
	case swstring("TELEMETRY"): out = WxDataSource::TELEMETRY;  break;
	case swstring("SERIAL"): out = WxDataSource::SERIAL;  break;
	case swstring("HOLFUY"): out = WxDataSource::HOLFUY;  break;
	default: break;
	}

	return out;
}

void ProgramConfig::getDiffConfiguration(DiffCalculator& calculator) {
	libconfig::Setting &root = config.getRoot();
	std::string temperature_from, temperature_subtract;
	std::string wind_from, wind_subtract;


	try {
		libconfig::Setting &d = root["Diffs"];
		d.lookupValue("TemperatureFrom", temperature_from);
		d.lookupValue("TemperatureSubtract", temperature_subtract);
		d.lookupValue("WindFrom", wind_from);
		d.lookupValue("WindSubtract", wind_subtract);

		calculator.temperatureFrom = 		ProgramConfig::wxDataSourceFromStr(temperature_from);
		calculator.temperatureSubstract = 	ProgramConfig::wxDataSourceFromStr(temperature_subtract);
		calculator.windFrom =				ProgramConfig::wxDataSourceFromStr(wind_from);
		calculator.windSubstract = 			ProgramConfig::wxDataSourceFromStr(wind_subtract);

		d.lookupValue("Enable", calculator.enable);

	}
	catch (libconfig::SettingNotFoundException &ex) {
		calculator.enable = false;
	}
}



void ProgramConfig::printConfigInPl(
											MySqlConnInterface& mysqlDb,
											AprsThreadConfig& aprsConfig,
											DataPresentation& dataPresence,
											int& rrdCount,
											int& plotCount,
											Telemetry& data,
											bool& useAsTemperature,
											HolfuyClientConfig& holfuy,
											DiffCalculator & calculator,
											DataSourceConfig & source

									) {

		cout << "--------KONFIGURACJA TELEMETRII------" << endl;
		cout << "--- FifthTelemAsTemperature: " << useAsTemperature << endl;
		cout << "--- scalingA: " << data.ch5a << endl;
		cout << "--- scalingB: " << data.ch5b << endl;
		cout << "--- scalingC: " << data.ch5c << endl;
		if (mysqlDb.enable) {
			cout << "--------KONFIGURACJA BAZY DANYCH-----" << endl;
			cout << "--- Adres Serwera: " << mysqlDb.IP << endl;
			cout << "--- Port: " << mysqlDb.port << endl;
			cout << "--- Użytkownik: " << mysqlDb.Username << endl;
			cout << "--- Nazwa bazy: " << mysqlDb.dbName << endl;
			cout << "--- Nazwa tabeli: " << mysqlDb.tableName << endl;
			cout << "--- Hasło nie jest wyświetlane" << endl;
			if (mysqlDb.execBeforeInsert == true)
				cout << "--- rc.preinsert zostanie wykonany" << endl;
			cout << endl;
		}
		cout << "--------KONFIGURACJA ŁĄCZNOŚCI Z SERWEREM APRS-----" << endl;
		cout << "--- Adres Serwera: " << aprsConfig.ServerURL << endl;
		cout << "--- Port Serwera: " << aprsConfig.ServerPort << endl;
		cout << "--- Znak monitorowanej stacji: " << aprsConfig.StationCall << endl;
		cout << "--- SSID monitorowanej stacji: " << aprsConfig.StationSSID << endl;
		cout << "--- Własny znak: " << aprsConfig.Call << endl;
		cout << "--- Aprs Secret: " << aprsConfig.Passwd << endl;
		cout << endl;
		if (holfuy.enable) {
			cout << "--------KONFIGURACJA ŁĄCZNOŚCI Z API HOLFUY -----" << endl;
			cout << "--- Nr stacji pogodowej: " << holfuy.stationId << endl;
			cout << "--- Hasło dostępowe do API: " << holfuy.apiPassword << endl;
			cout << endl;
		}
		if (calculator.enable) {
			cout << "-------- KONFIGURACJA POMIARÓW RÓŻNICOWYCH --------" << endl;
			cout << "--- Temperatura: od " << ::wxDataSourceToStr(calculator.temperatureFrom) << " odejmij " << ::wxDataSourceToStr(calculator.temperatureSubstract) << endl;
			cout << "--- Wiatr: od " << ::wxDataSourceToStr(calculator.windFrom) << " odejmij " << ::wxDataSourceToStr(calculator.windSubstract) << endl;
			cout << endl;
		}
		cout << "--------KONFIGURACJA ŹRÓDEŁ DANYCH-----" << endl;
		cout << "--- Temperatura: " << wxDataSourceToStr(source.temperature) << endl;
		cout << "--- Cisnienie: " << wxDataSourceToStr(source.pressure) << endl;
		cout << "--- Wiatr: " << wxDataSourceToStr(source.wind) << endl;
		cout << "--- Wilgotnosc: " << wxDataSourceToStr(source.humidity) << endl;
		cout << endl;
		cout << "--------KONFIGURACJA PLIKÓW RRD-----" << endl;
		for (unsigned i = 0; i < dataPresence.vRRDFiles.size(); i++) {
			cout << "--- Typ: " << dataPresence.RevSwitchPlotType(dataPresence.vRRDFiles[i].eType) << " - Ścieżka: " << dataPresence.vRRDFiles[i].sPath <<endl;
		}
		cout << endl;
		cout << "--------KONFIGURACJA GENEROWANEJ STRONY-----" << endl;
		cout << "--- Ścieżka zapisu pliku html: " << dataPresence.WebsitePath << endl;
		cout << "--- Tytuł generowanej strony: " << dataPresence.WebsiteTitle << endl;
		cout << "--- Nagłówek: " << dataPresence.WebsiteHeadingTitle << endl;
		cout << "--- Wiersz pomimędzy danymi num. a wykresami: " << dataPresence.WebsiteSubHeading << endl;
		cout << "--- Stopka: " << dataPresence.WebisteFooter << endl;
		if (dataPresence.WebsiteLinkToMoreInfo == true)
			cout << "--- Link do dodatkowych info zostanie wygenerowany" << endl;
		if (dataPresence.PrintPressure == true)
			cout << "--- Wyświetlanie ciśnienia włączone" << endl;
		if (dataPresence.PrintTemperature == true)
			cout << "--- Wyświetlanie temperatury włączone" << endl;
        if (dataPresence.directionCorrection != 0)
			cout << "--- Korekcja kierunku wiatru" << endl;
        if (dataPresence.PrintTwoSourcesInTable) {
        	cout << "--- Tabela będzie zawierała dane z dwóch różnych źródeł" << endl;
        	cout << "-- Etykieta Pierwszego: " << dataPresence.PrimaryLabel << endl;
        	cout << "-- Etykieta Drugiego: " << dataPresence.SecondaryLabel << endl;
        	cout << "-- Drugie źrodło danych: " << wxDataSourceToStr(dataPresence.SecondarySource) << endl;
        }
		cout << endl;
		cout << "--------KONFIGURACJA WYKRESÓW-----" << endl;
		cout << "--- Ilość wykresów do wygenerowania: " << dataPresence.vPNGFiles.size() << endl;
		for (unsigned ii = 0; ii < dataPresence.vPNGFiles.size(); ii++) {
			cout << "-----------------------" << endl;
			cout << "--- WYKRES NUMER: " << ii << endl;
			cout << "--- Tytuł wykresu: " << dataPresence.vPNGFiles[ii].Title << endl;
			cout << "--- Opis osi Y: " << dataPresence.vPNGFiles[ii].Axis << endl;
			cout << "--- Krok Osi i etykiet osi Y: " << dataPresence.vPNGFiles[ii].ScaleStep << ":" << dataPresence.vPNGFiles[ii].LabelStep << endl;
			cout << "--- Ścieżka do zapisu pliku PNG: " << dataPresence.vPNGFiles[ii].sPath << endl;
			cout << "--- Typ wykresu: " << dataPresence.RevSwitchPlotType(dataPresence.vPNGFiles[ii].eType) << " / " << dataPresence.RevSwitchPlotType(dataPresence.vPNGFiles[ii].eType) <<  endl;
			cout << "--- Ścieżka do PIERWSZEGO DS: " << dataPresence.vPNGFiles[ii].sDS0Path << endl;
			cout << "--- Opis pierwszego DS: " << dataPresence.vPNGFiles[ii].sDS0Name << endl;
			cout << "--- Typ RRA w pierwszym DS: " << dataPresence.RevSwitchRRAType(dataPresence.vPNGFiles[ii].eDS0RRAType) << endl; ;
			cout << "--- Rodzaj kreślenia pierwszego DS: " << dataPresence.RevSwitchPlotGraphType(dataPresence.vPNGFiles[ii].eDS0PlotType) << " / " << dataPresence.RevSwitchPlotGraphType(dataPresence.vPNGFiles[ii].eDS0PlotType) << endl;
			cout << "--- Kolor kreślenia pierwszego DS: " <<  hex << dataPresence.vPNGFiles[ii].DS0PlotColor << endl;
			if (dataPresence.vPNGFiles[ii].DoubleDS == false)
				continue;
			cout << "--- Ścieżka do DRUGIEGO DS: " << dataPresence.vPNGFiles[ii].sDS1Path << endl;
			cout << "--- Opis drugiego DS: " << dataPresence.vPNGFiles[ii].sDS1Name << endl;
			cout << "--- Typ RRA w drugim DS: " << dataPresence.RevSwitchRRAType(dataPresence.vPNGFiles[ii].eDS1RRAType) << endl; ;
			cout << "--- Rodzaj kreślenia drugiego DS: " << dataPresence.RevSwitchPlotGraphType(dataPresence.vPNGFiles[ii].eDS1PlotType) << " / " << dataPresence.RevSwitchPlotGraphType(dataPresence.vPNGFiles[ii].eDS1PlotType) << endl;
			cout << "--- Kolor kreślenia drugiego DS: " <<  hex << dataPresence.vPNGFiles[ii].DS1PlotColor << endl;
			cout << "--- Wysokość wykresu: " << dec << dataPresence.vPNGFiles[ii].Height << endl;
			cout << "--- Szerokość wykresu: " << dec << dataPresence.vPNGFiles[ii].Width << endl;

		}
//	}

}
