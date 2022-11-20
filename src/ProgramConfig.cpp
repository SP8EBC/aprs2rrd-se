/*
 * ProgramConfig.cpp
 *
 *  Created on: 04.12.2018
 *      Author: mateusz
 */

#include "ProgramConfig.h"
#include "AmbigiousDataSourceConfig.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "ZywiecMeteo.h"


ProgramConfig::ProgramConfig(std::string fn) : configFilename(fn) {
	// TODO Auto-generated constructor stub

	parseParameterPrinting = [](std::string input) {

		boost::to_upper(input);

		int in = swstring(input.c_str());

		switch (in) {
			case swstring("FALSE"):
			case swstring("OFF"):
			case swstring("NONE"):
					return PRINT_OFF;

			case swstring("TRUE"):
			case swstring("ON"):
			case swstring("BOTH"):
				return PRINT_BOTH;

			case swstring("LEFT"):
			case swstring("PRIMARY"):
				return PRINT_LEFT_PRIMARY;

			case swstring("RIGHT"):
			case swstring("SECONDARY"):
				return PRINT_LEFT_PRIMARY;

		}

		return PRINT_OFF;
	};
}

ProgramConfig::~ProgramConfig() {
	// TODO Auto-generated destructor stub
}

void ProgramConfig::parseFile() {
	config.readFile(configFilename.c_str());

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
		rBaza.lookupValue("ExecBeforeInsert", db.execBeforeInsert);
		rBaza.lookupValue("ExecBeforeInsertPath", db.execBeforeInsertPath);

		// old, version 1 database schema is the default one
		try {
			rBaza.lookupValue("SchemaV1", db.schema_v1);
		}
		catch (libconfig::SettingNotFoundException &ex) {
			db.schema_v1 = true;
		}

		try {
			rBaza.lookupValue("SchemaV2", db.schema_v2);
		}
		catch (libconfig::SettingNotFoundException &ex) {
			db.schema_v2 = false;
		}

		// if the version one schema is enabled the user must provide
		// a table to store the measurements
		try {
			rBaza.lookupValue("DbTable", db.tableName);
		}
		catch (libconfig::SettingNotFoundException &ex) {
			if (db.schema_v1) {
				throw UnsufficientConfig();
			}
		}


		try {
			rBaza.lookupValue("DumpHolfuy", db.dumpHolfuy);
			rBaza.lookupValue("DumpDiff", db.dumpDiff);
			rBaza.lookupValue("DumpTelemetry", db.dumpTelemetry);
		}
		catch (libconfig::SettingNotFoundException &ex) {
			db.dumpHolfuy = false;
			db.dumpDiff = false;
			db.dumpTelemetry = false;
		}
	}
	std::cout << "---ProgramConfig::getDbConfig:86 - Configuration loaded successfully" << std::endl;
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
	std::cout << "---ProgramConfig::getAprsThreadConfig:110 - Configuration loaded successfully" << std::endl;
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

	std::cout << "---ProgramConfig::getSerialConfig:130 - Configuration loaded successfully" << std::endl;
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

	rWWW.lookupValue("colorfulResultTable", data.colorfulResultTable);

//	std::cout << "---ProgramConfig::getDataPresentationConfig:158 - result = " << result << std::endl;
//	std::cout << "---ProgramConfig::getDataPresentationConfig:159 - data.colorfulResultTable = " << data.colorfulResultTable << std::endl;

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

	temp = "";

	rWWW.lookupValue("PrintTemperature", temp);
	data.PrintTemperature = this->parseParameterPrinting(std::move(temp));

	rWWW.lookupValue("PrintPressure", temp);
	data.PrintPressure = this->parseParameterPrinting(std::move(temp));

	rWWW.lookupValue("PrintHumidity", temp);
	data.PrintHumidity = this->parseParameterPrinting(std::move(temp));

	rWWW.lookupValue("DirectionCorrection", (int32_t&)data.directionCorrection);
	try {
		rWWW.lookupValue("PrintWind", temp);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		data.PrintWind = PRINT_BOTH;
	}

	try {
		rWWW.lookupValue("AdditionalImageBetweenTableAndPlots", data.WebsiteAdditionalImage);
		rWWW.lookupValue("AdditionalImageUrl", data.WebsiteAdditionalImgeUrl);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		data.WebsiteAdditionalImage = "";
	}

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

	std::cout << "---ProgramConfig::getDataPresentationConfig:211 - plotCount: " << plotCount << std::endl;

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

		temp = std::string();
		rPlots[ii].lookupValue("LongTimescaleFormat", temp);
		if (temp.size() > 0) {
			cVectorPNGTemp.LongTimescaleFormat = temp;
		}
		else {
			cVectorPNGTemp.LongTimescaleFormat = "%a-%H:00";
		}

		if (cVectorPNGTemp.sDS1Name.length() == 0)
			cVectorPNGTemp.DoubleDS = false;
		else
			cVectorPNGTemp.DoubleDS = true;

		data.vPNGFiles.push_back(cVectorPNGTemp);
	}

	std::cout << "---ProgramConfig::getDataPresentationConfig:263 - Configuration loaded successfully" << std::endl;
}

//void ProgramConfig::getTelemetryConfig(Telemetry& data, bool& useAsTemperature) {
//
//	config.lookupValue("FifthTelemAsTemperature", useAsTemperature);
//	config.lookupValue("TelemAScaling", data.ch5a);
//	config.lookupValue("TelemBScaling", data.ch5b);
//	config.lookupValue("TelemCScaling", data.ch5c);
//}

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

bool ProgramConfig::getBatchMode() {

	bool out = false;

	config.lookupValue("BatchMode", out);

	return out;
}

bool ProgramConfig::getExitOnException() {
	bool out = true;

	config.lookupValue("ExitOnCriticalException", out);

	return out;
}

bool ProgramConfig::configureLogOutput() {
	if (this->Debug == true) {
		cout << "--- Tryb debugowania włączony" << endl;
		cout << endl;
	}

	if (DebugToFile == true && this->DebugLogFn.length() > 3) {
			cout << "--- Wyjście z konsoli przekierownane do pliku: " << this->DebugLogFn;
			//fDebug.open(LogFile.c_str());
			//cout.rdbuf(fDebug.rdbuf());
			FILE* result_out = freopen(this->DebugLogFn.c_str(), "w", stdout);
			FILE* result_err = freopen(this->DebugLogFn.c_str(), "w", stderr);

			if (result_out == NULL || result_err == NULL) {
				return false;
			}
	}

	return true;
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
		std::cout << "--- ProgramConfig::getDataSourceConfig:370 - Error in data sources configuration. Using defaul values!" << std::endl;

		config_out.globalBackup = WxDataSource::IS_PRIMARY;
		config_out.humidity = WxDataSource::IS_PRIMARY;
		config_out.pressure = WxDataSource::IS_PRIMARY;
		config_out.rain = WxDataSource::IS_PRIMARY;
		config_out.temperature = WxDataSource::IS_PRIMARY;
		config_out.wind = WxDataSource::IS_PRIMARY;
	}
	catch (AmbigiousDataSourceConfig & ex) {
		std::cout << "--- ProgramConfig::getDataSourceConfig:380 - Error in data sources configuration. Using defaul values!" << std::endl;

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
		std::cout << "--- ProgramConfig::getDataSourceConfig:399 - No Serial KISS communcation is configured." << std::endl;

	}

//	try {
//		libconfig::Setting &h = root["Holfuy"];
//
//		//h.lookupValue("StationId", config_out.holfuyNumber);
//	}
//	catch (libconfig::SettingNotFoundException &ex) {
//		std::cout << "--- ProgramConfig::getDataSourceConfig:385 - No Holfuy communcation is configured." << std::endl;
//
//	}
}

void ProgramConfig::getHolfuyConfig(HolfuyClientConfig& config_out) {
	libconfig::Setting &root = config.getRoot();

	try {
		libconfig::Setting &h = root["Holfuy"];

		h.lookupValue("StationId", config_out.stationId);
		h.lookupValue("ApiPassword", config_out.apiPassword);
		h.lookupValue("Enable", config_out.enable);
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
	case swstring("ZYWIEC"): out = WxDataSource::ZYWIEC;  break;
	default: throw AmbigiousDataSourceConfig();
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
	case swstring("ZYWIEC"): out = WxDataSource::ZYWIEC;  break;
	default: throw AmbigiousDataSourceConfig();
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
	case swstring("ZYWIEC"): out = WxDataSource::ZYWIEC;  break;
	default: throw AmbigiousDataSourceConfig();
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
	case swstring("ZYWIEC"): out = WxDataSource::ZYWIEC;  break;
	default: throw AmbigiousDataSourceConfig();
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
	case swstring("ZYWIEC"): out = WxDataSource::ZYWIEC;  break;
	default: throw AmbigiousDataSourceConfig();
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
	case swstring("ZYWIEC"): out = WxDataSource::ZYWIEC;  break;
	default: throw AmbigiousDataSourceConfig();
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

void ProgramConfig::getPressureCalcConfig(PressureCalculator& pressureCalc) {
	libconfig::Setting &root = config.getRoot();
	std::string from, to;
	int altitude;

	try {
		libconfig::Setting &d = root["Pressure"];
		d.lookupValue("StationMeasures", from);
		d.lookupValue("ConvertTo", to);
		d.lookupValue("Altitude", altitude);

		boost::to_upper(from);
		boost::to_upper(to);

		pressureCalc.setStationMeasures(PressureCalculator::fromString(from));
		pressureCalc.setConvertTo(PressureCalculator::fromString(to));
		pressureCalc.setAltitude((uint16_t)altitude);
		pressureCalc.setEnable(true);

	}
	catch (libconfig::SettingNotFoundException &ex) {
		pressureCalc.setEnable(false);
	}
}

void ProgramConfig::getSlewRateLimitConfig(SlewRateLimiter& limiter) {
	float temperatureSlew = MAX_TEMP_SLEW;
	int32_t pressureSlew = MAX_PRESSURE_SLEW;
	float speedSlew = MAX_SPEED_SLEW;
	float gustsSlew = MAX_GUSTS_SLEW;
	int32_t humiditySlew = MAX_HUMIDITY_SLEW;
	bool humidityZeroAsHundret = false;
//	int32_t directionSlew;

	bool lookupSucceded = true;

	libconfig::Setting &root = config.getRoot();
	try {
		libconfig::Setting &slew = root["SlewLimit"];

		lookupSucceded = slew.lookupValue("Temperature", temperatureSlew);
		std::cout << "--- ProgramConfig::getSlewRateLimitConfig:654 - lookupSucceded: " << lookupSucceded << ", temperatureSlew: " << temperatureSlew << std::endl;

		lookupSucceded = (slew.lookupValue("Pressure", pressureSlew) ? lookupSucceded : false);
		std::cout << "--- ProgramConfig::getSlewRateLimitConfig:657 - lookupSucceded: " << lookupSucceded << ", pressureSlew: " << pressureSlew << std::endl;

		lookupSucceded = (slew.lookupValue("WindSpeed", speedSlew) ? lookupSucceded : false);
		std::cout << "--- ProgramConfig::getSlewRateLimitConfig:660 - lookupSucceded: " << lookupSucceded << ", speedSlew: " << speedSlew << std::endl;

		lookupSucceded = (slew.lookupValue("WindGusts", gustsSlew) ? lookupSucceded : false);
		std::cout << "--- ProgramConfig::getSlewRateLimitConfig:663 - lookupSucceded: " << lookupSucceded << ", gustsSlew: " << gustsSlew << std::endl;

		lookupSucceded = (slew.lookupValue("Humidity", humiditySlew) ? lookupSucceded : false);
		std::cout << "--- ProgramConfig::getSlewRateLimitConfig:666 - lookupSucceded: " << lookupSucceded << ", humidiySlew: " << humiditySlew << std::endl;


		//if (lookupSucceded) {
			limiter.setMaxTempSlew(temperatureSlew);
			limiter.setMaxPressureSlew((int16_t)pressureSlew);
			limiter.setMaxSpeedSleew(speedSlew);
			limiter.setMaxGustsSleew(gustsSlew);
			limiter.setMaxHumiditySlew(humiditySlew);
		//}

	}
	catch (libconfig::SettingNotFoundException &ex) {
		;
	}

	try {
		libconfig::Setting &slew = root["SlewLimit"];

		slew.lookupValue("HumidityZeroAsHundret", humidityZeroAsHundret);

		limiter.setHmZeroAs100(humidityZeroAsHundret);
	}
	catch (libconfig::SettingNotFoundException &ex) {
		;
	}

}

void ProgramConfig::getLocaleStaticString(Locale &l) {

	try {
		libconfig::Setting &root = config.getRoot();
		libconfig::Setting &locale = root["LocaleStaticStrings"];

		locale.lookupValue("generatedBy", l.generatedBy);
		locale.lookupValue("generatedBy2", l.generatedBy2);
		locale.lookupValue("humidity", l.humidity);
		locale.lookupValue("lastUpdate", l.lastUpdate);
		locale.lookupValue("moreInfo", l.moreInfo);
		locale.lookupValue("pressure", l.pressure);
		locale.lookupValue("temperature", l.temperature);
		locale.lookupValue("windDirection", l.windDirection);
		locale.lookupValue("windGusts", l.windGusts);
		locale.lookupValue("windSpeed", l.windSpeed);

		std::cout << "--- ProgramConfig::getLocaleStaticString:695 - Locale text data loaded successfully." << std::endl;
	}
	catch (libconfig::SettingNotFoundException &ex) {
		std::cout << "--- ProgramConfig::getLocaleStaticString:698 - Error during loading locale! Default values will be used" << std::endl;
	}

}

void ProgramConfig::getZywiecMeteoConfig(ZywiecMeteoConfig & z) {
	libconfig::Setting &rRoot = config.getRoot();
	std::string temp;

	z.enable = true;

	try {
		libconfig::Setting &zywiec = rRoot["ZywiecMeteo"];

		if(!zywiec.lookupValue("Enabled", z.enable)) {
			z.enable = false;
		}
	
		if (zywiec.lookupValue("BaseUrl", temp)){
			z.baseUrl = temp;
		}
		else {
			z.baseUrl = "https://monitoring.zywiec.powiat.pl/";
		}

		zywiec.lookupValue("StationId", z.stationId);

	}
	catch (libconfig::SettingNotFoundException &ex) {
		z.enable = false;
	}
 
}

bool ProgramConfig::getDateTimeLocale(char *localeString, basic_string<char>::size_type ln) {

	std::string l;

	bool output = false;

	if (localeString != 0) {
		config.lookupValue("DatetimeLocale", l);

		if (l.length() > 0 && ln > l.length()) {
			strcpy(localeString, l.c_str());
			output = true;
		}
	}

	return output;

}

void ProgramConfig::printConfigInPl(
											MySqlConnInterface& mysqlDb,
											AprsThreadConfig& aprsConfig,
											DataPresentation& dataPresence,
											int& rrdCount,
											int& plotCount,
											ZywiecMeteoConfig & zywiec,
											HolfuyClientConfig& holfuy,
											DiffCalculator & calculator,
											DataSourceConfig & source,
											PressureCalculator& pressureCalc,
											SlewRateLimiter & limiter,
											Locale & locale

									) {
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
			if (mysqlDb.dumpDiff)
				cout << "--- Zapis pomiarów różnocowych włączony." << endl;
			if (mysqlDb.dumpHolfuy)
				cout << "--- Zapis pomiarów Holfuy włączony." << endl;
			if (mysqlDb.dumpTelemetry)
				cout << "--- Zapis telemetrii włączony." << endl;

			cout << endl;
		}
		cout << "--------KONFIGURACJA ŁĄCZNOŚCI Z SERWEREM APRS-----" << endl;
		cout << "--- Adres Serwera: " << aprsConfig.ServerURL << endl;
		cout << "--- Port Serwera: " << aprsConfig.ServerPort << endl;
		cout << "--- Znak pierwszej  monitorowanej stacji: " << aprsConfig.StationCall << endl;
		cout << "--- SSID pierwszej monitorowanej stacji: " << aprsConfig.StationSSID << endl;
		cout << "--- Znak drugiej monitorowanej stacji: " << aprsConfig.SecondaryCall << endl;
		cout << "--- SSID drugiej monitorowanej stacji: " << aprsConfig.SecondarySSID << endl;
		cout << "--- Własny znak: " << aprsConfig.Call << endl;
		cout << "--- Aprs Secret: " << aprsConfig.Passwd << endl;
		cout << endl;
		if (zywiec.enable) {
			cout << "--------KONFIGURACJA ŁĄCZNOŚCI Z API STAROSTWA POWIATOWEGO W ZYWCU -----" << endl;
			cout << "--- Bazowy URL do API: " << zywiec.baseUrl << endl;
			cout << "--- Numeryczny identyfikator stacji: " << boost::lexical_cast<std::string>(zywiec.stationId) << endl;
			cout << "--- Nazwa stacji: " << ZywiecMeteo::convertToName(zywiec.stationId) << endl;
			cout << endl;
		}
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
		if (source.primaryCall != aprsConfig.StationCall || source.primarySsid != aprsConfig.StationSSID) {
			cout << "--- Uwaga! Znak i SSID pierwszej monitorowanej stacji różny od konfiguracji APRS-IS" << endl;
			cout << "---  " << source.primaryCall << "-" << (int)source.primarySsid << endl;
		}
		if (source.secondaryCall != aprsConfig.SecondaryCall || source.secondarySsid != aprsConfig.SecondarySSID) {
			cout << "--- Uwaga! Znak i SSID drugiej monitorowanej stacji różny od konfiguracji APRS-IS" << endl;
			cout << "---  " << source.secondaryCall << "-" << (int)source.secondarySsid << endl;
		}
		cout << endl;
		if (limiter.isChangedFromDefault()) {
			cout << "--------KONFIGURACJA OGRANICZNIKA SZYBKOŚCI NARASTANIA--------" << endl;
			cout << "----- Jeżeli zmiana parametrów w dwóch kolejnych ramkach -----" << endl;
			cout << "----- będzie większa niż poniższa, to zmiana ta zostanie -----" << endl;
			cout << "------------------ ograniczona do poniższych -----------------" << endl;
			cout << "--- Maksymalna zmiana temperatury: " << limiter.getMaxTempSlew() << endl;
			cout << "--- Maksymalna zmiana ciśnienia: " << limiter.getMaxPressureSlew() << endl;
			cout << "--- Maksymalna zmiana prędkoci wiatru: " << limiter.getMaxSpeedSleew() << endl;
			cout << "--- Maksymalna zmiana porywów wiatru: " << limiter.getMaxGustsSleew() << endl;
			cout << "--- Maksymalna zmiana wilgotności: " << limiter.getMaxHumiditySlew() << endl;
			if (limiter.isHmZeroAs100()) {
				cout << "----- Wilgotność 0 procent będzie zamieniana na 100 ------" << endl;
			}
			cout << endl;
		}
		if (pressureCalc.isEnable()) {
			cout << "-------- KONFIGURACJA PRZELICZANIA CIŚNIENIA --------" << endl;
			cout << "--- Stacja mierzy: " << PressureCalculator::toString(pressureCalc.getStationMeasures()) << endl;
			cout << "--- Przelicz na: " << PressureCalculator::toString(pressureCalc.getConvertTo()) << endl;
			cout << "--- Wysokość zainstalowania stacji: " << ::to_string(pressureCalc.getAltitude()) << endl;
			cout << endl;
		}
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
		if (dataPresence.colorfulResultTable) {
			cout << "--- Style CSS będą ładowane w zależności od temperatury, blue-style.css dla ujemnych i green-style.css dla dodatnich" << endl;
		}
		else {
			;
		}
		if (dataPresence.WebsiteLinkToMoreInfo == true) {
			cout << "--- Link do dodatkowych info zostanie wygenerowany" << endl;
		}
		cout << "--- Wyświetlanie wiatru: " << DataPresentation::ParametersPrintEnumToStr(dataPresence.PrintWind) << endl;
		cout << "--- Wyświetlanie ciśnienia: " << DataPresentation::ParametersPrintEnumToStr(dataPresence.PrintPressure) << endl;
		cout << "--- Wyświetlanie temperatury: " << DataPresentation::ParametersPrintEnumToStr(dataPresence.PrintTemperature) << endl;
        if (dataPresence.directionCorrection != 0) {
			cout << "--- Korekcja kierunku wiatru" << endl;
        }
        if (dataPresence.PrintTwoSourcesInTable) {
        	cout << "--- Tabela będzie zawierała dane z dwóch różnych źródeł" << endl;
        	cout << "-- Etykieta Pierwszego: " << dataPresence.PrimaryLabel << endl;
        	cout << "-- Etykieta Drugiego: " << dataPresence.SecondaryLabel << endl;
        	cout << "-- Drugie źrodło danych: " << wxDataSourceToStr(dataPresence.SecondarySource) << endl;
        }
        if (dataPresence.WebsiteAdditionalImage.size() > 2) {
        	cout << "--- Dodatkowa grafika nad wykresami: " << dataPresence.WebsiteAdditionalImage << endl;
        	if (dataPresence.WebsiteAdditionalImgeUrl.size() > 2) {
            	cout << "--- Link ustawiony na dodatkowej grafice: " << dataPresence.WebsiteAdditionalImgeUrl << endl;

        	}
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
		cout << endl;
		cout << "--------ZWROTY UŻYWANE DO GENEROWANIA STRONY WWW (LOKALIZACJA)-----" << endl;
		cout << "--- " << locale.generatedBy << endl;
		cout << "--- " << locale.generatedBy2 << endl;
		cout << "--- " << locale.humidity << endl;
		cout << "--- " << locale.lastUpdate << endl;
		cout << "--- " << locale.moreInfo << endl;
		cout << "--- " << locale.pressure << endl;
		cout << "--- " << locale.temperature << endl;
		cout << "--- " << locale.windDirection << endl;
		cout << "--- " << locale.windGusts << endl;
		cout << "--- " << locale.windSpeed << endl;

		cout << endl;

//	}

}
