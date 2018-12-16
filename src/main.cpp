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
#include <unistd.h>
#include <libconfig.h++>

#include "MySqlConnInterface.h"
#include "AprsPacket.h"
#include "AprsThreadConfig.h"
#include "AprsThread.h"
#include "AprsWXData.h"
#include "Telemetry.h"
#include "AprsAsioThread.h"
#include "SlewRateLimiter.h"
#include "ProgramConfig.h"

#include "ConnectionTimeoutEx.h"
#include "DataPresentation.h"


using namespace libconfig;
using namespace std;

bool Debug = false;
bool DebugToFile = false;
ofstream fDebug;

bool doZeroCorrection = false;
int correction = 0;

int main(int argc, char **argv){

	Config config;
	ProgramConfig programConfig("config.conf");

	AprsThreadConfig aprsConfig;
	AprsThread aprsThread;
	MySqlConnInterface mysqlDb;
	DataPresentation dataPresence;
	Telemetry telemetry;
	AprsAsioThread * asioThread;
	SlewRateLimiter limiter;

	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	AprsWXData wxTemp, wxTelemetry;
	AprsWXData wxTarget; // target wx data to be inserted into RRD DB & printed onto website
	AprsWXData wxLastTarget;
	AprsPacket* cPKTtemp;

	queue <AprsPacket> qPackets;
	queue <AprsWXData> qMeteo;

	string LogFile;

	bool mainLoopExit = true;
	bool isConnectionAlive = false;

	int PlotsCount = 0;
	int RRDCount = 0;

	bool useFifthTelemAsTemperature = false;
	float telemA = 0.0f;
	float telemB = 0.0f;
	float telemC = 0.0f;

	try {
		programConfig.parseFile();
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

	Debug = programConfig.getDebug();
	DebugToFile = programConfig.getDebugToFile();
	LogFile = programConfig.getDebugLogFn();

	try {
		programConfig.getTelemetryConfig(telemetry, useFifthTelemAsTemperature);
		programConfig.getDbConfig(mysqlDb);
		programConfig.getAprsThreadConfig(aprsConfig);
		programConfig.getDataPresentationConfig(dataPresence, RRDCount, PlotsCount);

		dataPresence.DebugOutput = Debug;
		mysqlDb.Debug = Debug;
		AprsWXData::DebugOutput = Debug;

	}
	catch (const SettingNotFoundException &ex) {
	//	return -3;
	}

	aprsConfig.RetryServerLookup = true;

	cout << "--- libconfig++: Konfiguracja odczytana" << endl;

	programConfig.configureLogOutput();

	ProgramConfig::printConfigInPl(mysqlDb, aprsConfig, dataPresence, RRDCount, PlotsCount, telemetry, useFifthTelemAsTemperature);

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

		// setting a logging level
		asioThread->DebugOutput = Debug;

		// initializing connection
		asioThread->connect();

		// a timeout handling shall be included here. Now it is only waiting
		while (!asioThread->isConnected()) { sleep(1);};

		// here the connection shall be full OK
		isConnectionAlive = true;

		while (isConnectionAlive) {
			try {
				// waiting for new frame to be reveived
				asioThread->receive();
			}
			catch (ConnectionTimeoutEx &e) {
				// if connection is timed out break internal loop to allow reconnecting
				isConnectionAlive = false;
				break;
			}

			// checkig if correct data has been received
			if (asioThread->isPacketValid()) {
				// Parsing wheater data. If this is not correct APRS wx packet the method will set
				// the internal validity flag to false
				AprsWXData::ParseData(asioThread->getPacket(), &wxTemp);

				// Parsing telemetry data
				Telemetry::ParseData(asioThread->getPacket(), &telemetry);

				// if this is data from WX Packet
				if (wxTemp.valid) {
					wxTarget.copy(wxTemp, useFifthTelemAsTemperature, false);

				}

				if (telemetry.valid) {
					wxTarget.copy(telemetry.getCh5(), useFifthTelemAsTemperature);
				}

				// each method below checks if passed WX packet is valid and if no they will
				// exit immediately witoud performing any changes

				wxTarget.PrintData();

				// applying wind direction correction if it was enabled by user
				AprsWXData::DirectionCorrection(wxTarget, (int16_t)dataPresence.directionCorrection);

				// limiting slew rates for measurements
				limiter.limitFromSingleFrame(wxLastTarget, wxTarget);

				// inserting the data inside a RRD file
				dataPresence.FetchDataInRRD(&wxTarget);

				// replotting the graphs set
				dataPresence.PlotGraphsFromRRD();

				// generating the website
				dataPresence.GenerateWebiste(&wxTarget);

				// storing values for slew rate corrections
				wxLastTarget = wxTarget;




			}
			else {
				if (Debug == true)
					cout << "--- To nie jest poprawna ramka APRS" << endl;
			}


		}

		delete asioThread;
	} while (mainLoopExit);		// end of main loop

	/////
	/////
	///// OLD GARBAGE TO BE REFACTORED
	/////
	/////
	/*
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
						//cWXtemp.ParseData(cPKTtemp);
					}
					catch(NotValidWXData &e) {
						cout << "--- To nie jest poprawny pakiet pogodowy" << endl;
					}
					catch(WXDataOK &e) {
						if (useFifthTelemAsTemperature == true)
							wxTemp.useTemperature = false;
						else
							wxTemp.useTemperature = true;

						wxTemp.useHumidity = true;
						wxTemp.usePressure = true;
						wxTemp.useWind = true;

						if (Debug == true && doZeroCorrection == true)
							cout << "--- ZeroCorrection" << endl;
						if (doZeroCorrection == true)
							wxTemp.ZeroCorrection(qMeteo);
						if ((short)correction != 0)
							wxTemp.DirectionCorrection((short)correction);
						if (Debug == true)
							cout << "--- FetchDataInRRD" << endl;
						dataPresence.FetchDataInRRD(&wxTemp);
						if (Debug == true)
							cout << "--- PlotGraphs" << endl;
						dataPresence.PlotGraphsFromRRD();
						if (Debug == true)
							cout << "--- GenerateWebsite" << endl;

						if (useFifthTelemAsTemperature == true) {
							wxTemp.temperature = wxTelemetry.temperature;
						}

						dataPresence.GenerateWebiste(&wxTemp);
						if (mysqlDb.enable == true) {
							if (Debug == true)
								cout << "--- InsertIntoDb" << endl;
							mysqlDb.InsertIntoDb(&wxTemp);
						}
						qMeteo.push(wxTemp);
						if (qMeteo.size() >= 4)
							qMeteo.pop();
						if (Debug == true) {
							cout << "--- Liczba obiektów w kolejce qMeteo: " << qMeteo.size() << endl;
							cout << "--- Liczba obiektów w kolejce qPackets: " << qPackets.size() << endl;
						}
						wxTemp.PrintData();
					}

					//char result = telemetry.ParseData(cPKTtemp);
/*
					if (result == 0 && useFifthTelemAsTemperature == true) {

						wxTelemetry.useTemperature = true;
						wxTelemetry.useHumidity = false;
						wxTelemetry.usePressure = false;
						wxTelemetry.useWind = false;

						if (Debug == true)
							cout << "--- Przetworzono temperature z telemetrii: " << telemetry.getCh5() << endl;
						wxTelemetry.temperature = telemetry.getCh5();

						if (Debug == true)
							cout << "--- FetchDataInRRD" << endl;
						dataPresence.FetchDataInRRD(&wxTelemetry);
						if (Debug == true)
							cout << "--- PlotGraphs" << endl;
						dataPresence.PlotGraphsFromRRD();
						if (Debug == true)
							cout << "--- GenerateWebsite" << endl;

						wxTelemetry.humidity 		= wxTemp.humidity;
						wxTelemetry.pressure 		= wxTemp.pressure;
						wxTelemetry.rain24 		= wxTemp.rain24;
						wxTelemetry.rain60 		= wxTemp.rain60;
						wxTelemetry.rain_day 		= wxTemp.rain_day;
						wxTelemetry.wind_direction = wxTemp.wind_direction;
						wxTelemetry.wind_gusts 	= wxTemp.wind_gusts;
						wxTelemetry.wind_speed 	= wxTemp.wind_speed;

						dataPresence.GenerateWebiste(&wxTelemetry);
					}

				//delete cWXtemp;

			}
			else
				cout << "--- Błąd alokacji przy:: cWXtemp = new AprsWXData;";


			delete cPKTtemp;

	}
	cout << "--- ZAMYKANIE";
	mysqlDb.CloseDBConnection();
	*/

	fDebug.close();


	return 0;
}
