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


using namespace libconfig;
using namespace std;

bool Debug = false;
bool DebugToFile = false;
ofstream fDebug;

bool doZeroCorrection = false;
int correction = 0;

int main(int argc, char **argv)
{

	Config cLibConfig;
	AprsThreadConfig cAprsConfig;
	AprsThread cAprs;
	MySqlConnInterface cDB;
	DataPresence cPresence;

	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	AprsWXData* cWXtemp;
	AprsPacket* cPKTtemp;

	queue <AprsPacket> qPackets;
	queue <AprsWXData> qMeteo;

	string LogFile;

	bool booltemp = false;
	int PlotsCount = 0;
	int RRDCount = 0;

	try {
		cLibConfig.readFile("config.conf");
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

	Setting &rRoot = cLibConfig.getRoot();

	cLibConfig.lookupValue("Debug", Debug);
	cLibConfig.lookupValue("DebugToFile", DebugToFile);
	cLibConfig.lookupValue("DebugLogFile", LogFile);

	try {
		Setting &rBaza = rRoot["MySQL"];

		rBaza.lookupValue("Enable", cDB.enable);
		if (cDB.enable) {
			rBaza.lookupValue("IpAddress", cDB.IP);
			rBaza.lookupValue("Port", cDB.port);
			rBaza.lookupValue("DbName", cDB.dbName);
			rBaza.lookupValue("DbUser", cDB.Username);
			rBaza.lookupValue("DbPassword", cDB.Password);
			rBaza.lookupValue("DbTable", cDB.tableName);
			rBaza.lookupValue("ExecBeforeInsert", cDB.execBeforeInsert);
			rBaza.lookupValue("ExecBeforeInsertPath", cDB.execBeforeInsertPath);
		}

		Setting &rAprsIS = rRoot["AprsIS"];

		rAprsIS.lookupValue("StationCall", cAprsConfig.StationCall);
		rAprsIS.lookupValue("StationSSID", cAprsConfig.StationSSID);
		rAprsIS.lookupValue("ServerAddr", cAprsConfig.ServerURL);
		rAprsIS.lookupValue("ServerPort", cAprsConfig.ServerPort);
		rAprsIS.lookupValue("MyCALL", cAprsConfig.Call);
		rAprsIS.lookupValue("MyPasswd", cAprsConfig.Passwd);

		Setting &rRRD = rRoot["RRD"];
		RRDCount = rRRD.getLength();

		for (int ii = 0; ii < RRDCount; ii++) {
			sVectorRRDTemp.Zero();
			string temp;

			rRRD[ii].lookupValue("Type", temp);
			sVectorRRDTemp.eType = cPresence.SwitchPlotType(temp);
			rRRD[ii].lookupValue("Path", sVectorRRDTemp.sPath);
			cPresence.vRRDFiles.push_back(sVectorRRDTemp);

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

		rWWW.lookupValue("IndexHtml", cPresence.WebsitePath);
		rWWW.lookupValue("Title", cPresence.WebsiteTitle);
		rWWW.lookupValue("HeadingTitle", cPresence.WebsiteHeadingTitle);
		rWWW.lookupValue("SubHeading", cPresence.WebsiteSubHeading);
		rWWW.lookupValue("LinkToMoreInfo", cPresence.WebsiteLinkToMoreInfo);
		rWWW.lookupValue("Footer", cPresence.WebisteFooter);
		rWWW.lookupValue("Plot0", cPresence.Plot0Path);
		rWWW.lookupValue("Plot1", cPresence.Plot1Path);
		rWWW.lookupValue("Plot2", cPresence.Plot2Path);
		rWWW.lookupValue("Plot3", cPresence.Plot3Path);
		rWWW.lookupValue("PrintTemperature", cPresence.PrintTemperature);
		rWWW.lookupValue("PrintPressure", cPresence.PrintPressure);
		rWWW.lookupValue("PrintHumidity", cPresence.PrintHumidity);
		rWWW.lookupValue("TemperatureCorrection", doZeroCorrection);
        rWWW.lookupValue("DirectionCorrection", correction);

		Setting &rPlots = rRoot["Plots"];
		PlotsCount = rPlots.getLength();
		for (int ii = 0; ii < PlotsCount; ii++) {
			string temp;

			cVectorPNGTemp.Zero();

			rPlots[ii].lookupValue("Type", temp);
			cVectorPNGTemp.eType = cPresence.SwitchPlotType(temp);
			rPlots[ii].lookupValue("Path", cVectorPNGTemp.sPath);
			rPlots[ii].lookupValue("DS0", cVectorPNGTemp.sDS0Path);
			rPlots[ii].lookupValue("DS0Name", cVectorPNGTemp.sDS0Name);
			rPlots[ii].lookupValue("DS0RRAType", temp);
			cVectorPNGTemp.eDS0RRAType = cPresence.SwitchRRAType(temp);
			rPlots[ii].lookupValue("DS0PlotType", temp);
			cVectorPNGTemp.eDS0PlotType = cPresence.SwitchPlotGraphType(temp);
			rPlots[ii].lookupValue("DS0PlotColor", cVectorPNGTemp.DS0PlotColor);
			rPlots[ii].lookupValue("DS0Label", cVectorPNGTemp.sDS0Label);

			rPlots[ii].lookupValue("DS1", cVectorPNGTemp.sDS1Path);
			rPlots[ii].lookupValue("DS1Name", cVectorPNGTemp.sDS1Name);
			rPlots[ii].lookupValue("DS1RRAType", temp);
			cVectorPNGTemp.eDS1RRAType = cPresence.SwitchRRAType(temp);
			rPlots[ii].lookupValue("DS1PlotType", temp);
			cVectorPNGTemp.eDS1PlotType = cPresence.SwitchPlotGraphType(temp);
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

			cPresence.vPNGFiles.push_back(cVectorPNGTemp);
		}

	}
	catch (const SettingNotFoundException &ex) {
	//	return -3;
	}

	cAprsConfig.RetryServerLookup = true;

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

		cout << "--------KONFIGURACJA BAZY DANYCH-----" << endl;
		cout << "--- Adres Serwera: " << cDB.IP << endl;
		cout << "--- Port: " << cDB.port << endl;
		cout << "--- Użytkownik: " << cDB.Username << endl;
		cout << "--- Nazwa bazy: " << cDB.dbName << endl;
		cout << "--- Nazwa tabeli: " << cDB.tableName << endl;
		cout << "--- Hasło nie jest wyświetlane" << endl;
		if (cDB.execBeforeInsert == true)
			cout << "--- rc.preinsert zostanie wykonany" << endl;
		cout << endl;
		cout << "--------KONFIGURACJA ŁĄCZNOŚCI Z SERWEREM APRS-----" << endl;
		cout << "--- Adres Serwera: " << cAprsConfig.ServerURL << endl;
		cout << "--- Port Serwera: " << cAprsConfig.ServerPort << endl;
		cout << "--- Znak monitorowanej stacji: " << cAprsConfig.StationCall << endl;
		cout << "--- SSID monitorowanej stacji: " << cAprsConfig.StationSSID << endl;
		cout << "--- Własny znak: " << cAprsConfig.Call << endl;
		cout << "--- Aprs Secret: " << cAprsConfig.Passwd << endl;
		cout << endl;
		cout << "--------KONFIGURACJA PLIKÓW RRD-----" << endl;
		for (unsigned i = 0; i < cPresence.vRRDFiles.size(); i++) {
			cout << "--- Ścieżka: " << cPresence.vRRDFiles[i].sPath << endl;
			cout << "--- Typ: " << cPresence.vRRDFiles[i].eType << endl;
		}
		cout << endl;
		cout << "--------KONFIGURACJA GENEROWANEJ STRONY-----" << endl;
		cout << "--- Ścieżka zapisu pliku html: " << cPresence.WebsitePath << endl;
		cout << "--- Tytuł generowanej strony: " << cPresence.WebsiteTitle << endl;
		cout << "--- Nagłówek: " << cPresence.WebsiteHeadingTitle << endl;
		cout << "--- Wiersz pomimędzy danymi num. a wykresami: " << cPresence.WebsiteSubHeading << endl;
		cout << "--- Stopka: " << cPresence.WebisteFooter << endl;
		if (cPresence.WebsiteLinkToMoreInfo == true)
			cout << "--- Link do dodatkowych info zostanie wygenerowany" << endl;
		if (cPresence.PrintPressure == true)
			cout << "--- Wyświetlanie ciśnienia włączone" << endl;
		if (cPresence.PrintTemperature == true)
			cout << "--- Wyświetlanie temperatury włączone" << endl;
		if (doZeroCorrection == true)
			cout << "--- Korekcja przekłamań temperatury włączona" << endl;
        if (correction != 0)
			cout << "--- Korekcja kierunku wiatru" << endl;
		cout << endl;
		cout << "--------KONFIGURACJA WYKRESÓW-----" << endl;
		cout << "--- Ilość wykresów do wygenerowania: " << PlotsCount << endl;
		for (unsigned ii = 0; ii < cPresence.vPNGFiles.size(); ii++) {
			cout << "-----------------------" << endl;
			cout << "--- WYKRES NUMER: " << ii << endl;
			cout << "--- Tytuł wykresu: " << cPresence.vPNGFiles[ii].Title << endl;
			cout << "--- Opis osi Y: " << cPresence.vPNGFiles[ii].Axis << endl;
			cout << "--- Krok Osi i etykiet osi Y: " << cPresence.vPNGFiles[ii].ScaleStep << ":" << cPresence.vPNGFiles[ii].LabelStep << endl;
			cout << "--- Ścieżka do zapisu pliku PNG: " << cPresence.vPNGFiles[ii].sPath << endl;
			cout << "--- Typ wykresu: " << cPresence.vPNGFiles[ii].eType << " / " << cPresence.RevSwitchPlotType(cPresence.vPNGFiles[ii].eType) <<  endl;
			cout << "--- Ścieżka do PIERWSZEGO DS: " << cPresence.vPNGFiles[ii].sDS0Path << endl;
			cout << "--- Opis pierwszego DS: " << cPresence.vPNGFiles[ii].sDS0Name << endl;
			cout << "--- Typ RRA w pierwszym DS: " << cPresence.RevSwitchRRAType(cPresence.vPNGFiles[ii].eDS0RRAType) << endl; ;
			cout << "--- Rodzaj kreślenia pierwszego DS: " << cPresence.vPNGFiles[ii].eDS0PlotType << " / " << cPresence.RevSwitchPlotGraphType(cPresence.vPNGFiles[ii].eDS0PlotType) << endl;
			cout << "--- Kolor kreślenia pierwszego DS: " <<  hex << cPresence.vPNGFiles[ii].DS0PlotColor << endl;
			if (cPresence.vPNGFiles[ii].DoubleDS == false)
				continue;
			cout << "--- Ścieżka do DRUGIEGO DS: " << cPresence.vPNGFiles[ii].sDS1Path << endl;
			cout << "--- Opis drugiego DS: " << cPresence.vPNGFiles[ii].sDS1Name << endl;
			cout << "--- Typ RRA w drugim DS: " << cPresence.RevSwitchRRAType(cPresence.vPNGFiles[ii].eDS1RRAType) << endl; ;
			cout << "--- Rodzaj kreślenia drugiego DS: " << cPresence.vPNGFiles[ii].eDS1PlotType << " / " << cPresence.RevSwitchPlotGraphType(cPresence.vPNGFiles[ii].eDS1PlotType) << endl;
			cout << "--- Kolor kreślenia drugiego DS: " <<  hex << cPresence.vPNGFiles[ii].DS1PlotColor << endl;
			cout << "--- Wysokość wykresu: " << dec << cPresence.vPNGFiles[ii].Height << endl;
			cout << "--- Szerokość wykresu: " << dec << cPresence.vPNGFiles[ii].Width << endl;

		}
	}

	if (cDB.enable == true) {
		try {
			cDB.OpenDBConnection();
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

	try {
		cAprs.AprsISConnect(&cAprsConfig);
	}
	catch (AprsServerNotFound &e) {
		cout << e.what() << endl;
		if (cAprsConfig.ServerPort == false)
			return -3;
		else
			booltemp = true;
	}
	catch (AprsServerConnected &e) {
		if (Debug == true)
			cout << "--- Połączono i zalogowano" << endl;
	}
	if (booltemp == true) {
		for (;;) {
			try {
				cAprs.AprsISConnect(&cAprsConfig);
			}
			catch (AprsServerConnected &e) {
				if (Debug == true)
					cout << "--- Połączono i zalogowano" << endl;
				break;
			}
			catch (AprsServerNotFound &e) {
				cout << e.what() << endl;
				sleep(10);
			}
		}
	}

	for(;;) {
		try {
			cAprs.AprsISThread(true);
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
			cAprs.zero();
			//continue;
			cAprs.BufferLen = 0;
		}
		catch (ReadRetZero &e) {
			cout << e.what() << endl;
			e.error();
			cAprs.AprsISDisconnect();
			try {
				cAprs.AprsISConnect();
			}
			catch (AprsServerConnected &e) {
				if (Debug == true)
					cout << "--- Połączono i zalogowano" << endl;
				sleep(2);
			}
			//continue;
			cAprs.BufferLen = 0;
		}
		catch (...) {
			cout << "--- Nieznany wyjątek rzucony z cAprs.AprsISThread(true);" << endl;
		}
		cPKTtemp = new AprsPacket;
		if (cPKTtemp != NULL) {
			if (cDB.enable == true) {
				cDB.Keepalive();
			}
			try {
				cPKTtemp->ParseAPRSISData(cAprs.Buffer, cAprs.BufferLen, cPKTtemp);
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

			cWXtemp = new AprsWXData;
			if (cWXtemp != NULL) {
				try {
					cout << "--- Przetwarzanie danych pogodowych..." << endl;
					cWXtemp->ParseData(cPKTtemp);
				}
				catch(NotValidWXData &e) {
					cout << "--- To nie jest poprawny pakiet pogodowy" << endl;
				}
				catch(WXDataOK &e) {
					if (Debug == true && doZeroCorrection == true)
						cout << "--- ZeroCorrection" << endl;
					if (doZeroCorrection == true)
						cWXtemp->ZeroCorrection(qMeteo);
                    if ((short)correction != 0)
                        cWXtemp->DirectionCorrection((short)correction);
//					if (Debug == true)
//						cout << "--- QnhQfeCorrection" << endl;
//					cWXtemp->pressure = cWXtemp->QnhQfeCorrection(cWXtemp->pressure, 960);
					//cout << cWXtemp->QnhQfeCorrection(cWXtemp->pressure, 960) << endl;
					if (Debug == true)
						cout << "--- FetchDataInRRD" << endl;
					cPresence.FetchDataInRRD(cWXtemp);
					if (Debug == true)
						cout << "--- PlotGraphs" << endl;
					cPresence.PlotGraphsFromRRD();
					if (Debug == true)
						cout << "--- GenerateWebsite" << endl;
					cPresence.GenerateWebiste(cWXtemp);
					if (cDB.enable == true) {
						if (Debug == true)
							cout << "--- InsertIntoDb" << endl;
						cDB.InsertIntoDb(cWXtemp);
					}
					qMeteo.push(*cWXtemp);
					if (qMeteo.size() >= 4)
						qMeteo.pop();
					if (Debug == true) {
						cout << "--- Liczba obiektów w kolejce qMeteo: " << qMeteo.size() << endl;
						cout << "--- Liczba obiektów w kolejce qPackets: " << qPackets.size() << endl;
					}
					cWXtemp->PrintData();
				}

				delete cWXtemp;

			}
			else
				cout << "--- Błąd alokacji przy:: cWXtemp = new AprsWXData;";


			delete cPKTtemp;
			}
		else
			cout << "--- Błąd alokacji przy:: cPKTtemp = new AprsPacket;";
	}
	cout << "--- ZAMYKANIE";
	cDB.CloseDBConnection();
	fDebug.close();
	return 0;
}
