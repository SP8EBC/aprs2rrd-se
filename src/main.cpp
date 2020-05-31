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
#include "AprsWXData.h"
#include "Telemetry.h"
#include "AprsAsioThread.h"
#include "SlewRateLimiter.h"
#include "ProgramConfig.h"
#include "SerialAsioThread.h"
#include "SerialConfig.h"
#include "HolfuyClientConfig.h"
#include "HolfuyClient.h"
#include "DiffCalculator.h"

#include "ConnectionTimeoutEx.h"
#include "DataPresentation.h"


using namespace libconfig;
using namespace std;

bool Debug = false;
bool DebugToFile = false;
ofstream fDebug;

bool doZeroCorrection = false;
int correction = 0;

std::shared_ptr<std::condition_variable> syncCondition;
std::shared_ptr<std::mutex> syncLock;

#define DEFAULT_APRS_SERVER_TIMEOUT_SECONDS 73

void wait_for_data() {
	std::unique_lock<std::mutex> lock(*syncLock);

	// waiting for receive
	auto result = syncCondition->wait_for(lock, std::chrono::seconds(DEFAULT_APRS_SERVER_TIMEOUT_SECONDS));

	// there is no need to encapsulate 'wait_for()' method inside while(work_is_done) loop which protect against
	// locking a thread before processing in another one even starts. Here receiving is triggered just before waiting
	// and app architecture doesn't provide next receiving before previous frame is fully processed

	if (result == cv_status::timeout) {
		lock.unlock();
		throw ConnectionTimeoutEx();
	}
	else {
		lock.unlock();
		return;
	}
}

int main(int argc, char **argv){

	Config config;
	ProgramConfig programConfig("config.conf");

	SerialAsioThread * serialThread;
	AprsThreadConfig aprsConfig;
	MySqlConnInterface mysqlDb;
	DataPresentation dataPresence;
	Telemetry telemetry;
	AprsAsioThread * asioThread;
	SlewRateLimiter limiter;
	SerialConfig serialConfig;
	DataSourceConfig sourceConfig;
	HolfuyClientConfig holfuyConfig;
	std::unique_ptr<HolfuyClient> holfuyClient;
	DiffCalculator diffCalculator;


	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	AprsWXData wxIsTemp, wxSerialTemp, wxTelemetry, wxHolfuy;
	AprsWXData wxTarget; // target wx data to be inserted into RRD DB & printed onto website
	AprsWXData wxLastTarget;
	AprsWXData wxDifference;
	AprsWXData wxSecondarySrcForPage;
	AprsPacket isRxPacket;
	AprsPacket serialRxPacket;

	queue <AprsPacket> qPackets;
	queue <AprsWXData> qMeteo;

	syncCondition.reset(new std::condition_variable());
	syncLock.reset(new std::mutex());

	string LogFile;

	bool mainLoopExit = true;
	bool isConnectionAlive = false;

	int PlotsCount = 0;
	int RRDCount = 0;

	bool useFifthTelemAsTemperature = false;
	//float telemA = 0.0f;
	//float telemB = 0.0f;
	//float telemC = 0.0f;

	try {
		programConfig.parseFile();
		cout << "--- main:119 - Opening configuration file" << endl;
	}

	catch(const FileIOException &ex)
	{
		printf("--- main:124 - The configuration file cannot be opened.\r\n");
		return -1;
	}
	catch(const ParseException &ex) {
		printf("--- main:128 - Error during parsing a content of configuration file near line %d", ex.getLine());
		return -2;
	}

	Debug = programConfig.getDebug();
	DebugToFile = programConfig.getDebugToFile();
	LogFile = programConfig.getDebugLogFn();


	try {
		programConfig.getDataSourceConfig(sourceConfig);
		programConfig.getTelemetryConfig(telemetry, useFifthTelemAsTemperature);
		programConfig.getDbConfig(mysqlDb);
		programConfig.getAprsThreadConfig(aprsConfig);
		programConfig.getDataPresentationConfig(dataPresence, RRDCount, PlotsCount);
		programConfig.getSerialConfig(serialConfig);
		programConfig.getHolfuyConfig(holfuyConfig);
		programConfig.getDiffConfiguration(diffCalculator);
		programConfig.getStationName();

		dataPresence.DebugOutput = Debug;
		mysqlDb.Debug = Debug;
		AprsWXData::DebugOutput = Debug;
		Telemetry::Debug = Debug;

		if (useFifthTelemAsTemperature) {
			sourceConfig.temperature = WxDataSource::TELEMETRY;
		}

	}
	catch (const SettingNotFoundException &ex) {
		cout << "--- main:160 - Unrecoverable error during configuration file loading!" << endl;
		return -3;
	}

	aprsConfig.RetryServerLookup = true;

	cout << "--- main:166 - Configuration parsed successfully" << endl;

	programConfig.configureLogOutput();

	ProgramConfig::printConfigInPl(mysqlDb, aprsConfig, dataPresence, RRDCount, PlotsCount, telemetry, useFifthTelemAsTemperature, holfuyConfig, diffCalculator, sourceConfig);

	serialThread = new SerialAsioThread(syncCondition, syncLock, serialConfig.serialPort, serialConfig.baudrate);

	// if an user want to use serial port it needs to be opened and configured
	if (serialConfig.enable) {
		serialThread->openPort();
	}

	if (holfuyConfig.enable) {
		holfuyClient.reset(new HolfuyClient(holfuyConfig.stationId, holfuyConfig.apiPassword));
	}

	// creating a new copy of ASIO thread
	asioThread = new AprsAsioThread(aprsConfig, DEFAULT_APRS_SERVER_TIMEOUT_SECONDS, syncCondition, syncLock);

	// setting a logging level
	asioThread->DebugOutput = Debug;
	serialThread->debug = Debug;

	// main loop
	do {
		// initializing connection
		asioThread->connect();

		// a timeout handling shall be included here. Now it is only waiting
		while (!asioThread->isConnected()) { sleep(1);};

		// here the connection shall be full OK
		isConnectionAlive = true;

		while (isConnectionAlive) {
			try {
				// waiting for new frame to be reveived
				asioThread->receive(false);

				// starting serial receive
				serialThread->receive(false);

				// waiting for new data
				wait_for_data();

				// checkig if correct data has been received
				if (asioThread->isPacketValid() || serialThread->isPacketValid()) {

					// checking from what input data has been received
					if (asioThread->isPacketValid()) {
						isRxPacket = asioThread->getPacket();

						// Parsing weather data. If this is not correct APRS wx packet the method will set
						// the internal validity flag to false
						AprsWXData::ParseData(isRxPacket, &wxIsTemp);

						// Parsing telemetry data
						Telemetry::ParseData(isRxPacket, &telemetry);

						// marking if this packet was received from primary or secondary callsign (or none of them)
						AprsWXData::checkIsPrimaryCall(wxIsTemp, sourceConfig);

						// storing the telemetry values in db. This metod checks internally if that function is enabled
						// and telemetry data are valid.
						mysqlDb.InsertTelmetry(telemetry);

						// wait for another packet if not WX data has been received. Protect against
						// flooding with a lot of data from Holfuy after each heartbeat message from APRS-IS
						if (!wxIsTemp.valid) {
							continue;
						}

						// setting the source of this wx packet
						wxIsTemp.dataSource = WXDataSource::APRSIS;
					}
					else if (serialThread->isPacketValid()) {
						serialRxPacket = serialThread->getPacket();

						// check the KISS frame against the user configuration configuration
						if (!serialConfig.validateAprsPacket(serialRxPacket)) {

							continue;
						}

						// Parsing data from serial KISS modem
						AprsWXData::ParseData(serialRxPacket, &wxSerialTemp);

						// setting the source of this wx packet
						wxSerialTemp.dataSource = WXDataSource::SERIAL;
					}
					else;

					// downloading Holfuy data if it is enabled
					if (holfuyConfig.enable) {
						holfuyClient->download();

						holfuyClient->parse();

						holfuyClient->getWxData(wxHolfuy);

						std::cout << "--- main.cpp:267 - Printing data downloaded & parsed from Holfuy API. Ignore 'use' flags" << std::endl;

						wxHolfuy.PrintData();
					}

					// zeroing the usage flags in target object
					AprsWXData::zeroUse(wxTarget);

					// if this is data from WX Packet
					if (wxIsTemp.valid) {

						// applying wind direction correction if it was enabled by an user
						AprsWXData::DirectionCorrection(wxIsTemp, (int16_t)dataPresence.directionCorrection);

						wxTarget.copy(wxIsTemp, sourceConfig);

						// invalidate packet from IS to prevent reusing in next loop iteration even
						// if data was sent by another source
						wxIsTemp.valid = false;

					}

					if (wxSerialTemp.valid) {

						// applying wind direction correction if it was enabled by user
						AprsWXData::DirectionCorrection(wxSerialTemp, (int16_t)dataPresence.directionCorrection);

						wxTarget.copy(wxSerialTemp, sourceConfig);

						wxSerialTemp.valid = false;
					}

					if (telemetry.valid) {
						wxTarget.copy(telemetry, sourceConfig);
					}

					if (wxHolfuy.valid) {
						wxTarget.copy(wxHolfuy, sourceConfig);
					}

					// calculating the difference between sources according to user configuration
					// if this feature is disabled completely the function will do nothing and return immediately
					diffCalculator.calculate(wxIsTemp, wxSerialTemp, wxHolfuy, telemetry, wxDifference);

					// each method below checks if passed WX packet is valid and if no they will
					// exit immediately witout performing any changes

					// printing target data
					std::cout << "--- main.c:315 - Printing target WX data which will be used for further processing." << std::endl;
					wxTarget.PrintData();

					// limiting slew rates for measurements
					limiter.limitFromSingleFrame(wxLastTarget, wxTarget);

					// geting the data for second source
					dataPresence.GetSecondarySource(wxIsTemp, wxSerialTemp, wxHolfuy, wxSecondarySrcForPage);

					// inserting the data inside a RRD file
					dataPresence.FetchDataInRRD(&wxTarget);

					// insertind diff-data inside RRD files
					dataPresence.FetchDiffInRRD(wxDifference);

					// replotting the graphs set
					dataPresence.PlotGraphsFromRRD();

					// generating the website
					dataPresence.GenerateWebiste(wxTarget, wxSecondarySrcForPage);

					// storing values for slew rate corrections
					wxLastTarget = wxTarget;

					if (mysqlDb.enable == true) {
						try {
							mysqlDb.OpenDBConnection();

							mysqlDb.InsertIntoDb(&wxTarget);

							mysqlDb.InsertIntoDbSchema2(wxTarget, sourceConfig, programConfig.getStationName());

							mysqlDb.CloseDBConnection();
						}
						catch(UnsufficientConfig &e) {

						}
						catch(BadSrvAddr &e) {

						}
						catch(ConnError &e) {
							cout << e.what();
						}
						catch (AlreadyDisconnected &e) {
							cout << e.what();
						}
						catch (...) {

						}
					}

				}
				else {
					if (Debug == true)
						cout << "--- main.cpp:366 - This is not valid APRS packet" << endl;
				}
			}
			catch (ConnectionTimeoutEx &e) {
				// if connection is timed out break internal loop to allow reconnecting
				isConnectionAlive = false;
				break;
			}
			catch (std::exception &e) {
				cout << "--- main:375 - std::exception " << e.what() << std::endl;
			}
			catch (...) {
				cout << "--- main:378 - Unknown exception thrown during processing!" << std::endl;
			}

		}

		std::cout << "--- main:383 - Connection to APRS server died. Reconnecting.." << std::endl;

	} while (mainLoopExit);		// end of main loop

	std::cout << "--- Exiting application";

	fDebug.close();


	return 0;
}
