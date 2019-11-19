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
#include "SerialAsioThread.h"
#include "SerialConfig.h"

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
	AprsThread aprsThread;
	MySqlConnInterface mysqlDb;
	DataPresentation dataPresence;
	Telemetry telemetry;
	AprsAsioThread * asioThread;
	SlewRateLimiter limiter;
	SerialConfig serialConfig;

	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	AprsWXData wxTemp, wxTelemetry;
	AprsWXData wxTarget; // target wx data to be inserted into RRD DB & printed onto website
	AprsWXData wxLastTarget;
	AprsPacket rxPacket;

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
		programConfig.getSerialConfig(serialConfig);

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

	serialThread = new SerialAsioThread(syncCondition, syncLock, serialConfig.serialPort, serialConfig.baudrate);

	// configuring serial thread
//	serialThread.configure(serialConfig.serialPort, serialConfig.baudrate, boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none),
//			boost::asio::serial_port_base::character_size(),
//			boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none),
//			boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

	// if an user want to use serial port it needs to be opened and configured
	if (serialConfig.enable) {
		serialThread->openPort();
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

				wait_for_data();

				// checkig if correct data has been received
				if (asioThread->isPacketValid() || serialThread->isPacketValid()) {

					// checking from what input data has been received
					if (asioThread->isPacketValid())
						rxPacket = asioThread->getPacket();
					else if (serialThread->isPacketValid()) {
						rxPacket = serialThread->getPacket();

						// check the KISS frame against the user configuration configuration
						if (!serialConfig.validateAprsPacket(rxPacket)) {

							continue;
						}
					}
					else;

					// Parsing wheater data. If this is not correct APRS wx packet the method will set
					// the internal validity flag to false
					AprsWXData::ParseData(rxPacket, &wxTemp);

					// Parsing telemetry data
					Telemetry::ParseData(rxPacket, &telemetry);

					// if this is data from WX Packet
					if (wxTemp.valid) {

						// applying wind direction correction if it was enabled by user
						AprsWXData::DirectionCorrection(wxTemp, (int16_t)dataPresence.directionCorrection);

						wxTarget.copy(wxTemp, useFifthTelemAsTemperature, false);

					}

					if (telemetry.valid) {
						wxTarget.copy(telemetry.getCh5(), useFifthTelemAsTemperature);
					}

					// each method below checks if passed WX packet is valid and if no they will
					// exit immediately witoud performing any changes

					wxTarget.PrintData();

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
						try {
							mysqlDb.OpenDBConnection();

							mysqlDb.InsertIntoDb(&wxTarget);

							mysqlDb.CloseDBConnection();
						}
						catch(UnsufficientConfig &e) {

						}
						catch(BadSrvAddr &e) {

						}
						catch(ConnError &e) {
							cout << e.what();
						}
						catch (...) {

						}
					}

				}
				else {
					if (Debug == true)
						cout << "--- This is not valid APRS packet" << endl;
				}
			}
			catch (ConnectionTimeoutEx &e) {
				// if connection is timed out break internal loop to allow reconnecting
				isConnectionAlive = false;
				break;
			}
			catch (std::exception &e) {
				cout << "std::exception " << e.what() << std::endl;
			}
			catch (...) {
				cout << "Unknown exception thrown during processing!" << std::endl;
			}

		}

		std::cout << "--- Connection to APRS server died. Reconnecting.." << std::endl;

	} while (mainLoopExit);		// end of main loop

	std::cout << "--- Exiting application";

	fDebug.close();


	return 0;
}
