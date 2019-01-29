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

#define DEFAULT_APRS_SERVER_TIMEOUT_SECONDS 73

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
		Telemetry::Debug = Debug;

	}
	catch (const SettingNotFoundException &ex) {
	//	return -3;
	}

	aprsConfig.RetryServerLookup = true;

	cout << "--- libconfig++: Configuration parsed successfully" << endl;

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
		asioThread = new AprsAsioThread(aprsConfig, DEFAULT_APRS_SERVER_TIMEOUT_SECONDS);

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

				if (mysqlDb.enable == true) {
					mysqlDb.InsertIntoDb(&wxTarget);
				}




			}
			else {
				if (Debug == true)
					cout << "--- This is not valid APRS packet" << endl;
			}


		}

		std::cout << "--- Connection to APRS server died. Reconnecting.." << std::endl;

		delete asioThread;
	} while (mainLoopExit);		// end of main loop

	std::cout << "--- Exiting application";

	fDebug.close();


	return 0;
}
