#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <queue>
#include <libconfig.h++>

#include "MySqlConnInterface.h"
#include "AprsPacket.h"
#include "AprsThreadConfig.h"
#include "AprsThread.h"
#include "AprsWXData.h"
#include "DataPresence.h"
#include "Telemetry.h"
#include "AprsAsioThread.h"


using namespace libconfig;
using namespace std;

bool Debug = false;
bool DebugToFile = false;
ofstream fDebug;

bool doZeroCorrection = false;
int correction = 0;

int main(int argc, char **argv)
{

	Config config;
	AprsThreadConfig aprsConfig;
	AprsThread aprsThread;
	MySqlConnInterface mysqlDb;
	DataPresence dataPresence;
	Telemetry telemetry;
	AprsAsioThread * asioThread;


	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	AprsWXData cWXtemp, cWXtelemetry;
	AprsPacket* cPKTtemp;

	queue <AprsPacket> qPackets;
	queue <AprsWXData> qMeteo;

	string LogFile;

	bool mainLoopExit = true;
	bool isConnectionAlive = false;

	bool booltemp = false;
	int PlotsCount = 0;
	int RRDCount = 0;

	bool useFifthTelemAsTemperature = false;
	float telemA = 0.0f;
	float telemB = 0.0f;
	float telemC = 0.0f;

	try {
		config.readFile("config.conf");
		cout << "--- libconfig++: Otwieram plik konfiguracyjny..." << endl;
	}

	catch(const FileIOException &ex)
	{
		printf("--- libconfig++: Nie mozna otworzyc pliku!!!\r\n");
		return -1;
	}
	catch(const ParseException &ex) {
		printf("--- libconfig++: Blad parsowania pliku wejsciowego!!!");
		return -2;
	}

	Setting &rRoot = config.getRoot();

	config.lookupValue("Debug", Debug);
	config.lookupValue("DebugToFile", DebugToFile);
	config.lookupValue("DebugLogFile", LogFile);

	config.lookupValue("FifthTelemAsTemperature", useFifthTelemAsTemperature);
	config.lookupValue("TelemAScaling", telemA);
	config.lookupValue("TelemBScaling", telemB);
	config.lookupValue("TelemCScaling", telemC);

	telemetry.ch5a = telemA;
	telemetry.ch5b = telemB;
	telemetry.ch5c = telemC;


	try {
		Setting &rBaza = rRoot["MySQL"];

		rBaza.lookupValue("Enable", mysqlDb.enable);
		if (mysqlDb.enable) {
			rBaza.lookupValue("IpAddress", mysqlDb.IP);
			rBaza.lookupValue("Port", mysqlDb.port);
			rBaza.lookupValue("DbName", mysqlDb.dbName);
			rBaza.lookupValue("DbUser", mysqlDb.Username);
			rBaza.lookupValue("DbPassword", mysqlDb.Password);
			rBaza.lookupValue("DbTable", mysqlDb.tableName);
			rBaza.lookupValue("ExecBeforeInsert", mysqlDb.execBeforeInsert);
			rBaza.lookupValue("ExecBeforeInsertPath", mysqlDb.execBeforeInsertPath);
		}

		Setting &rAprsIS = rRoot["AprsIS"];

		rAprsIS.lookupValue("StationCall", aprsConfig.StationCall);
		rAprsIS.lookupValue("StationSSID", aprsConfig.StationSSID);
		rAprsIS.lookupValue("ServerAddr", aprsConfig.ServerURL);
		rAprsIS.lookupValue("ServerPort", aprsConfig.ServerPort);
		rAprsIS.lookupValue("MyCALL", aprsConfig.Call);
		rAprsIS.lookupValue("MyPasswd", aprsConfig.Passwd);

		Setting &rRRD = rRoot["RRD"];
		RRDCount = rRRD.getLength();

		for (int ii = 0; ii < RRDCount; ii++) {
			sVectorRRDTemp.Zero();
			string temp;

			rRRD[ii].lookupValue("Type", temp);
			sVectorRRDTemp.eType = dataPresence.SwitchPlotType(temp);
			rRRD[ii].lookupValue("Path", sVectorRRDTemp.sPath);
			dataPresence.vRRDFiles.push_back(sVectorRRDTemp);

			}

/*
		rRRD.lookupValue("WindSpeedRRD", sVectorRRDTemp.sPath);
		sVectorRRDTemp.eType = WIND_SPD;
		cPresence.vRRDFiles.push_back(sVectorRRDTemp);

		rRRD.lookupValue("WindGustsRRD", sVectorRRDTemp.sPath);
		sVectorRRDTemp.eType = WIND_GST;
		cPresence.vRRDFiles.push_back(sVectorRRDTemp);

		rRRD.lookupValue("WindDirRRD", sVectorRRDTemp.sPath);
		sVectorRRDTemp.eType = WIND_DIR;
		cPresence.vRRDFiles.push_back(sVectorRRDTemp);
*/
		Setting &rWWW = rRoot["Website"];

		rWWW.lookupValue("IndexHtml", dataPresence.WebsitePath);
		rWWW.lookupValue("Title", dataPresence.WebsiteTitle);
		rWWW.lookupValue("HeadingTitle", dataPresence.WebsiteHeadingTitle);
		rWWW.lookupValue("SubHeading", dataPresence.WebsiteSubHeading);
		rWWW.lookupValue("LinkToMoreInfo", dataPresence.WebsiteLinkToMoreInfo);
		rWWW.lookupValue("Footer", dataPresence.WebisteFooter);
		rWWW.lookupValue("Plot0", dataPresence.Plot0Path);
		rWWW.lookupValue("Plot1", dataPresence.Plot1Path);
		rWWW.lookupValue("Plot2", dataPresence.Plot2Path);
		rWWW.lookupValue("Plot3", dataPresence.Plot3Path);
		rWWW.lookupValue("PrintTemperature", dataPresence.PrintTemperature);
		rWWW.lookupValue("PrintPressure", dataPresence.PrintPressure);
		rWWW.lookupValue("PrintHumidity", dataPresence.PrintHumidity);
		rWWW.lookupValue("TemperatureCorrection", doZeroCorrection);
        rWWW.lookupValue("DirectionCorrection", correction);

		Setting &rPlots = rRoot["Plots"];
		PlotsCount = rPlots.getLength();
		for (int ii = 0; ii < PlotsCount; ii++) {
			string temp;

			cVectorPNGTemp.Zero();

			rPlots[ii].lookupValue("Type", temp);
			cVectorPNGTemp.eType = dataPresence.SwitchPlotType(temp);
			rPlots[ii].lookupValue("Path", cVectorPNGTemp.sPath);
			rPlots[ii].lookupValue("DS0", cVectorPNGTemp.sDS0Path);
			rPlots[ii].lookupValue("DS0Name", cVectorPNGTemp.sDS0Name);
			rPlots[ii].lookupValue("DS0RRAType", temp);
			cVectorPNGTemp.eDS0RRAType = dataPresence.SwitchRRAType(temp);
			rPlots[ii].lookupValue("DS0PlotType", temp);
			cVectorPNGTemp.eDS0PlotType = dataPresence.SwitchPlotGraphType(temp);
			rPlots[ii].lookupValue("DS0PlotColor", cVectorPNGTemp.DS0PlotColor);
			rPlots[ii].lookupValue("DS0Label", cVectorPNGTemp.sDS0Label);

			rPlots[ii].lookupValue("DS1", cVectorPNGTemp.sDS1Path);
			rPlots[ii].lookupValue("DS1Name", cVectorPNGTemp.sDS1Name);
			rPlots[ii].lookupValue("DS1RRAType", temp);
			cVectorPNGTemp.eDS1RRAType = dataPresence.SwitchRRAType(temp);
			rPlots[ii].lookupValue("DS1PlotType", temp);
			cVectorPNGTemp.eDS1PlotType = dataPresence.SwitchPlotGraphType(temp);
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

			dataPresence.vPNGFiles.push_back(cVectorPNGTemp);
		}

	}
	catch (const SettingNotFoundException &ex) {
	//	return -3;
	}

	aprsConfig.RetryServerLookup = true;

	cout << "--- libconfig++: Konfiguracja odczytana" << endl;

	if (Debug == true) {
		cout << "--- Tryb debugowania włączony" << endl;
		cout << "--- Wyjście z konsoli przekierownane do pliku: " << LogFile;
		cout << endl;

		if (DebugToFile == true) {
			//fDebug.open(LogFile.c_str());
			//cout.rdbuf(fDebug.rdbuf());
			freopen(LogFile.c_str(), "w", stdout);
		}

		cout << "--------KONFIGURACJA TELEMETRII------" << endl;
		cout << "--- FifthTelemAsTemperature: " << useFifthTelemAsTemperature << endl;
		cout << "--- scalingA: " << telemA << endl;
		cout << "--- scalingB: " << telemB << endl;
		cout << "--- scalingC: " << telemC << endl;
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
		cout << "--------KONFIGURACJA ŁĄCZNOŚCI Z SERWEREM APRS-----" << endl;
		cout << "--- Adres Serwera: " << aprsConfig.ServerURL << endl;
		cout << "--- Port Serwera: " << aprsConfig.ServerPort << endl;
		cout << "--- Znak monitorowanej stacji: " << aprsConfig.StationCall << endl;
		cout << "--- SSID monitorowanej stacji: " << aprsConfig.StationSSID << endl;
		cout << "--- Własny znak: " << aprsConfig.Call << endl;
		cout << "--- Aprs Secret: " << aprsConfig.Passwd << endl;
		cout << endl;
		cout << "--------KONFIGURACJA PLIKÓW RRD-----" << endl;
		for (unsigned i = 0; i < dataPresence.vRRDFiles.size(); i++) {
			cout << "--- Ścieżka: " << dataPresence.vRRDFiles[i].sPath << endl;
			cout << "--- Typ: " << dataPresence.vRRDFiles[i].eType << endl;
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
		if (doZeroCorrection == true)
			cout << "--- Korekcja przekłamań temperatury włączona" << endl;
        if (correction != 0)
			cout << "--- Korekcja kierunku wiatru" << endl;
		cout << endl;
		cout << "--------KONFIGURACJA WYKRESÓW-----" << endl;
		cout << "--- Ilość wykresów do wygenerowania: " << PlotsCount << endl;
		for (unsigned ii = 0; ii < dataPresence.vPNGFiles.size(); ii++) {
			cout << "-----------------------" << endl;
			cout << "--- WYKRES NUMER: " << ii << endl;
			cout << "--- Tytuł wykresu: " << dataPresence.vPNGFiles[ii].Title << endl;
			cout << "--- Opis osi Y: " << dataPresence.vPNGFiles[ii].Axis << endl;
			cout << "--- Krok Osi i etykiet osi Y: " << dataPresence.vPNGFiles[ii].ScaleStep << ":" << dataPresence.vPNGFiles[ii].LabelStep << endl;
			cout << "--- Ścieżka do zapisu pliku PNG: " << dataPresence.vPNGFiles[ii].sPath << endl;
			cout << "--- Typ wykresu: " << dataPresence.vPNGFiles[ii].eType << " / " << dataPresence.RevSwitchPlotType(dataPresence.vPNGFiles[ii].eType) <<  endl;
			cout << "--- Ścieżka do PIERWSZEGO DS: " << dataPresence.vPNGFiles[ii].sDS0Path << endl;
			cout << "--- Opis pierwszego DS: " << dataPresence.vPNGFiles[ii].sDS0Name << endl;
			cout << "--- Typ RRA w pierwszym DS: " << dataPresence.RevSwitchRRAType(dataPresence.vPNGFiles[ii].eDS0RRAType) << endl; ;
			cout << "--- Rodzaj kreślenia pierwszego DS: " << dataPresence.vPNGFiles[ii].eDS0PlotType << " / " << dataPresence.RevSwitchPlotGraphType(dataPresence.vPNGFiles[ii].eDS0PlotType) << endl;
			cout << "--- Kolor kreślenia pierwszego DS: " <<  hex << dataPresence.vPNGFiles[ii].DS0PlotColor << endl;
			if (dataPresence.vPNGFiles[ii].DoubleDS == false)
				continue;
			cout << "--- Ścieżka do DRUGIEGO DS: " << dataPresence.vPNGFiles[ii].sDS1Path << endl;
			cout << "--- Opis drugiego DS: " << dataPresence.vPNGFiles[ii].sDS1Name << endl;
			cout << "--- Typ RRA w drugim DS: " << dataPresence.RevSwitchRRAType(dataPresence.vPNGFiles[ii].eDS1RRAType) << endl; ;
			cout << "--- Rodzaj kreślenia drugiego DS: " << dataPresence.vPNGFiles[ii].eDS1PlotType << " / " << dataPresence.RevSwitchPlotGraphType(dataPresence.vPNGFiles[ii].eDS1PlotType) << endl;
			cout << "--- Kolor kreślenia drugiego DS: " <<  hex << dataPresence.vPNGFiles[ii].DS1PlotColor << endl;
			cout << "--- Wysokość wykresu: " << dec << dataPresence.vPNGFiles[ii].Height << endl;
			cout << "--- Szerokość wykresu: " << dec << dataPresence.vPNGFiles[ii].Width << endl;

		}
	}

	if (mysqlDb.enable == true) {
		try {
			mysqlDb.OpenDBConnection();
		}
		catch(UnsufficientConfig &e) {

		}
		catch(BadSrvAddr &e) {

		}
		catch(ConnError &e) {
			cout << e.what();
		}
		catch(OK &e) {
			cout << e.what();
		}
		catch (...) {

		}
	}

	// main loop
	do {
		// creating a new copy of ASIO thread
		asioThread = new AprsAsioThread(aprsConfig, 99);

		// initializing connection
		asioThread->connect();

		// a timeout handling shall be included here
		while (!asioThread->isConnected());

		// here the connection shall be full OK
		isConnectionAlive = true;

		while (isConnectionAlive) {

		}

		delete asioThread;
	} while (mainLoopExit);

	for(;;) {
		try {
			aprsThread.AprsISThread(true);
		}
		catch (AprsConnectionFrozen &e) {
			cout << e.what() << endl;
			for (;;) {
				try {
					e.Reconnect();
				}
				catch (AprsServerNotFound &e) {
					cout << e.what() << endl;
					sleep(10);
				}
				catch (AprsServerConnected &e) {
					if (Debug == true)
						cout << "--- Połączono i zalogowano" << endl;
					break;
				}
			}
		}
		catch (BufferOverflow &e) {
			cout << e.what() << endl;
			e.error();
			aprsThread.zero();
			//continue;
			aprsThread.BufferLen = 0;
		}
		catch (ReadRetZero &e) {
			cout << e.what() << endl;
			e.error();
			aprsThread.AprsISDisconnect();
			try {
				aprsThread.AprsISConnect();
			}
			catch (AprsServerConnected &e) {
				if (Debug == true)
					cout << "--- Połączono i zalogowano" << endl;
				sleep(2);
			}
			//continue;
			aprsThread.BufferLen = 0;
		}
		catch (...) {
			cout << "--- Nieznany wyjątek rzucony z cAprs.AprsISThread(true);" << endl;
		}
		cPKTtemp = new AprsPacket;
		if (cPKTtemp != NULL) {
			if (mysqlDb.enable == true) {
				mysqlDb.Keepalive();
			}
			try {
				cPKTtemp->ParseAPRSISData(aprsThread.Buffer, aprsThread.BufferLen, cPKTtemp);
			}
			catch (NotValidAprsPacket &e) {
				if (Debug == true)
					cout << "--- Odebrano dane nie będące pakietem APRS" << endl;
			}
			catch (PacketParsedOK &e) {
				cPKTtemp->PrintPacketData();
				qPackets.push(*cPKTtemp);
				if (qPackets.size() >= 4)
					qPackets.pop();
			}

			//cWXtemp = new AprsWXData;
					try {
						cout << "--- Przetwarzanie danych pogodowych..." << endl;
						cWXtemp.ParseData(cPKTtemp);
					}
					catch(NotValidWXData &e) {
						cout << "--- To nie jest poprawny pakiet pogodowy" << endl;
					}
					catch(WXDataOK &e) {
						if (useFifthTelemAsTemperature == true)
							cWXtemp.useTemperature = false;
						else
							cWXtemp.useTemperature = true;

						cWXtemp.useHumidity = true;
						cWXtemp.usePressure = true;
						cWXtemp.useWind = true;

						if (Debug == true && doZeroCorrection == true)
							cout << "--- ZeroCorrection" << endl;
						if (doZeroCorrection == true)
							cWXtemp.ZeroCorrection(qMeteo);
						if ((short)correction != 0)
							cWXtemp.DirectionCorrection((short)correction);
						if (Debug == true)
							cout << "--- FetchDataInRRD" << endl;
						dataPresence.FetchDataInRRD(&cWXtemp);
						if (Debug == true)
							cout << "--- PlotGraphs" << endl;
						dataPresence.PlotGraphsFromRRD();
						if (Debug == true)
							cout << "--- GenerateWebsite" << endl;

						if (useFifthTelemAsTemperature == true) {
							cWXtemp.temperature = cWXtelemetry.temperature;
						}

						dataPresence.GenerateWebiste(&cWXtemp);
						if (mysqlDb.enable == true) {
							if (Debug == true)
								cout << "--- InsertIntoDb" << endl;
							mysqlDb.InsertIntoDb(&cWXtemp);
						}
						qMeteo.push(cWXtemp);
						if (qMeteo.size() >= 4)
							qMeteo.pop();
						if (Debug == true) {
							cout << "--- Liczba obiektów w kolejce qMeteo: " << qMeteo.size() << endl;
							cout << "--- Liczba obiektów w kolejce qPackets: " << qPackets.size() << endl;
						}
						cWXtemp.PrintData();
					}

					char result = telemetry.ParseData(cPKTtemp);

					if (result == 0 && useFifthTelemAsTemperature == true) {

						cWXtelemetry.useTemperature = true;
						cWXtelemetry.useHumidity = false;
						cWXtelemetry.usePressure = false;
						cWXtelemetry.useWind = false;

						if (Debug == true)
							cout << "--- Przetworzono temperature z telemetrii: " << telemetry.getCh5() << endl;
						cWXtelemetry.temperature = telemetry.getCh5();

						if (Debug == true)
							cout << "--- FetchDataInRRD" << endl;
						dataPresence.FetchDataInRRD(&cWXtelemetry);
						if (Debug == true)
							cout << "--- PlotGraphs" << endl;
						dataPresence.PlotGraphsFromRRD();
						if (Debug == true)
							cout << "--- GenerateWebsite" << endl;

						cWXtelemetry.humidity 		= cWXtemp.humidity;
						cWXtelemetry.pressure 		= cWXtemp.pressure;
						cWXtelemetry.rain24 		= cWXtemp.rain24;
						cWXtelemetry.rain60 		= cWXtemp.rain60;
						cWXtelemetry.rain_day 		= cWXtemp.rain_day;
						cWXtelemetry.wind_direction = cWXtemp.wind_direction;
						cWXtelemetry.wind_gusts 	= cWXtemp.wind_gusts;
						cWXtelemetry.wind_speed 	= cWXtemp.wind_speed;

						dataPresence.GenerateWebiste(&cWXtelemetry);
					}

				//delete cWXtemp;

			}
			else
				cout << "--- Błąd alokacji przy:: cWXtemp = new AprsWXData;";


			delete cPKTtemp;

	}
	cout << "--- ZAMYKANIE";
	mysqlDb.CloseDBConnection();
	fDebug.close();
	return 0;
}
