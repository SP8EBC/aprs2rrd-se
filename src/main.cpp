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
#include <cmath>
#include <libconfig.h++>
#include <locale.h>

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
#include "PressureCalculator.h"
#include "ZywiecMeteo.h"
#include "ZywiecMeteoConfig.h"

#include "ConnectionTimeoutEx.h"
#include "DataPresentation.h"

#include "SOFTWARE_VERSION.h"


using namespace libconfig;
using namespace std;

bool Debug = false;
bool DebugToFile = false;
ofstream fDebug;

bool doZeroCorrection = false;
int correction = 0;

bool batchMode = false;

bool exitOnException = true;

std::shared_ptr<std::condition_variable> syncCondition;
std::shared_ptr<std::mutex> syncLock;

#define DEFAULT_APRS_SERVER_TIMEOUT_SECONDS 73

#define set_locale(); 		if (*datetimeLocale != 0x00) {	\
								(void)setlocale(LC_TIME, datetimeLocale);	\
							}		\

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

	std::string configFn;

	if (argc > 1) {
		configFn = std::string(argv[1]);
	}
	else {
		configFn = "config.conf";
	}

	std::cout << "--- main:86 - Using configuration from file: " << configFn << std::endl;

	Config config;
	ProgramConfig programConfig(configFn);

	SerialAsioThread * serialThread;
	AprsThreadConfig aprsConfig;
	MySqlConnInterface mysqlDb;
	DataPresentation dataPresence;
	AprsAsioThread * asioThread;
	SlewRateLimiter limiter;
	SerialConfig serialConfig;
	DataSourceConfig sourceConfig;
	HolfuyClientConfig holfuyConfig;
	std::unique_ptr<HolfuyClient> holfuyClient;
	ZywiecMeteoConfig zywiecMeteoConfig;
	std::unique_ptr<ZywiecMeteo> zywiecMeteo;
	DiffCalculator diffCalculator;
	PressureCalculator pressureCalculator;
	Locale locale;
	char datetimeLocale[16];
	char * currrentLocale;

	Telemetry telemetry;

	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	AprsWXData wxIsTemp, wxSerialTemp, wxTelemetry, wxHolfuy, wxZywiec;
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

	string response;	// for zywiec

	string LogFile;

	bool mainLoopExit = true;
	bool isConnectionAlive = false;

	int PlotsCount = 0;
	int RRDCount = 0;

	bool tcpOrSerialPacketGood = false;

	memset(datetimeLocale, 0, 16);

	try {
		programConfig.parseFile();
		cout << "--- main:142 - Opening configuration file" << endl;
	}

	catch(const FileIOException &ex)
	{
		printf("--- main:147 - The configuration file cannot be opened.\r\n");
		return -1;
	}
	catch(const ParseException &ex) {
		printf("--- main:151 - Error during parsing a content of configuration file near line %d \r\n", ex.getLine());
		return -2;
	}

	Debug = programConfig.getDebug();
	DebugToFile = programConfig.getDebugToFile();
	LogFile = programConfig.getDebugLogFn();
	exitOnException = programConfig.getExitOnException();

	try {
		programConfig.getDataSourceConfig(sourceConfig);
		programConfig.getDbConfig(mysqlDb);
		programConfig.getAprsThreadConfig(aprsConfig);
		programConfig.getDataPresentationConfig(dataPresence, RRDCount, PlotsCount);
		programConfig.getSerialConfig(serialConfig);
		programConfig.getHolfuyConfig(holfuyConfig);
		programConfig.getDiffConfiguration(diffCalculator);
		programConfig.getStationName();
		programConfig.getPressureCalcConfig(pressureCalculator);
		programConfig.getSlewRateLimitConfig(limiter);
		programConfig.getLocaleStaticString(locale);
		programConfig.getZywiecMeteoConfig(zywiecMeteoConfig);
		programConfig.getDateTimeLocale(datetimeLocale, 16);

		dataPresence.DebugOutput = Debug;
		mysqlDb.Debug = Debug;
		AprsWXData::DebugOutput = Debug;
		Telemetry::Debug = Debug;

		sourceConfig.holfuyNumber = holfuyConfig.stationId;
		sourceConfig.zywiecNumber = zywiecMeteoConfig.stationId;

		set_locale();

		(void)setlocale(LC_NUMERIC, "en_US.UTF-8");

	}
	catch (const SettingNotFoundException &ex) {
		cout << "--- main:191 - Unrecoverable error during configuration file loading!" << endl;
		return -3;
	}

	aprsConfig.RetryServerLookup = true;

	cout << "--- main:197 - Configuration parsed successfully" << endl;

	bool result = programConfig.configureLogOutput();

	if (!result) {
		return -5;
	}

	cout << "------------- APRS2RRD version " << SW_VER << " --------------" << endl;
	cout << "---------- Startup time in UTC " << boost::posix_time::to_simple_string(boost::posix_time::second_clock::universal_time()) << " -----------" << endl;	// 20
	cout << "---------------------------------------------------------------" << endl;
	currrentLocale = setlocale(LC_TIME, NULL);
	cout << "--- Current LC_TIME locale: " << currrentLocale << endl;
	currrentLocale = setlocale(LC_NUMERIC, NULL);
	cout << "--- Current LC_NUMERIC locale: " << currrentLocale << endl << endl;

	ProgramConfig::printConfigInPl(mysqlDb, aprsConfig, dataPresence, RRDCount, PlotsCount, zywiecMeteoConfig, holfuyConfig, diffCalculator, sourceConfig, pressureCalculator, limiter, locale);

	cout << "--- main:210 - exitOnException: " << exitOnException << endl;

	serialThread = new SerialAsioThread(syncCondition, syncLock, serialConfig.serialPort, serialConfig.baudrate);

	// if an user want to use serial port it needs to be opened and configured
	if (serialConfig.enable) {
		serialThread->openPort();
	}

	if (holfuyConfig.enable) {
		holfuyClient = std::make_unique<HolfuyClient>(holfuyConfig.stationId, holfuyConfig.apiPassword);
	}

	if (zywiecMeteoConfig.enable) {
		zywiecMeteo = std::make_unique<ZywiecMeteo>(zywiecMeteoConfig.baseUrl, zywiecMeteoConfig.secondaryTemperature);
	}

	// creating a new copy of ASIO thread
	asioThread = new AprsAsioThread(aprsConfig, DEFAULT_APRS_SERVER_TIMEOUT_SECONDS, syncCondition, syncLock);

	// setting a logging level
	asioThread->DebugOutput = Debug;
	serialThread->debug = Debug;

	// check operation mode chosen by an user
	batchMode = programConfig.getBatchMode();
	mainLoopExit = !batchMode;

	if (!batchMode && !aprsConfig.enable && !serialConfig.enable) {
		std::cout << "--- main:239 - You cannot run continuous mode w/o APRS-IS connection or Serial port enabled" << std::endl;
	}

	if (batchMode) {
		std::cout << "--- main:243 - RUNNING IN BATCH MODE" << std::endl;
	}

	// main loop
	do {

		// check if APRS-IS tcp connection is enabled
		if (aprsConfig.enable) {

			// initializing connection
			asioThread->connect();

			// a timeout handling shall be included here. Now it is only waiting
			while (!asioThread->isConnected()) { sleep(1);};

			// here the connection shall be full OK
			isConnectionAlive = true;
		}
		else {
			// if aprs-is is not enabled set the flag to true to proceed further
			isConnectionAlive = true;
		}

		while (isConnectionAlive) {
			try {
				// waiting for new frame to be reveived
				asioThread->receive(false);

				// starting serial receive
				serialThread->receive(false);

				if (aprsConfig.enable || serialConfig.enable) {
					// waiting for new data
					wait_for_data();
				}

				// check if legit packet has been received asynchronously from APRS-IS or serial port
				tcpOrSerialPacketGood = asioThread->isPacketValid() || serialThread->isPacketValid();

				//std::cout << "--- main:282 - tcpOrSerialPacketGood: " << boost::lexical_cast<std::string>(tcpOrSerialPacketGood) << std::endl;

				// checkig if correct data has been received
				if (tcpOrSerialPacketGood || batchMode) {

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
						mysqlDb.InsertTelmetry(telemetry, programConfig.getStationName());

						// wait for another packet if not WX data has been received. Protect against
						// flooding with a lot of data from Holfuy after each heartbeat message from APRS-IS
						if (!wxIsTemp.valid && !telemetry.valid) {
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

					// dwonload data from Zywiec meteo system
					if (zywiecMeteoConfig.enable) {
						zywiecMeteo->downloadMeasuresFromRangeForStation(zywiecMeteoConfig.stationId, response);

						zywiecMeteo->parseJson(response, wxZywiec);

						std::cout << "--- main:337 - Parsing data from Zywiec county meteo system API" << std::endl;

						wxZywiec.PrintData();
					}

					// downloading Holfuy data if it is enabled
					if (holfuyConfig.enable) {
						holfuyClient->download();

						holfuyClient->parse();

						holfuyClient->getWxData(wxHolfuy);

						std::cout << "--- main:350 - Printing data downloaded & parsed from Holfuy API. Ignore 'use' flags" << std::endl;

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

						// insert battery voltage into RRD
						dataPresence.FetchBatteryVoltageInRRD(telemetry.getBatteryVoltage());
					}

					if (wxZywiec.valid) {
						wxTarget.copy(wxZywiec, sourceConfig);
					}

					if (wxHolfuy.valid) {
						wxTarget.copy(wxHolfuy, sourceConfig);
					}

					if (!wxTarget.valid) {
						if (batchMode) {
							std::cout << "-- main:396 - No valid data have been received from configured sources!!" << std::endl;

							throw std::exception();
						}
						else {
							continue;
						}
					}

					// calculating the difference between sources according to user configuration
					// if this feature is disabled completely the function will do nothing and return immediately
					diffCalculator.calculate(wxIsTemp, wxSerialTemp, wxHolfuy, wxZywiec, telemetry, wxDifference);

					// recalculating pressure according to user configuration. If recalculation is not enabled
					// the metod will return the same value as given on input
					if (wxTarget.convertPressure)
						wxTarget.pressure = (int16_t)::roundf(pressureCalculator.convertPressure(wxTarget.pressure, wxTarget.temperature));

					// seting this flag to false will prevent reconverting pressure when its value hasn't been updated
					wxTarget.convertPressure = false;

					// each method below checks if passed WX packet is valid and if no they will
					// exit immediately witout performing any changes

					// printing target data
					std::cout << "--- main:420 - Printing target WX data which will be used for further processing." << std::endl;
					wxTarget.PrintData();

					// limiting slew rates for measurements
					limiter.limitFromSingleFrame(wxLastTarget, wxTarget);

					// geting the data for second source
					dataPresence.GetSecondarySource(wxIsTemp, wxSerialTemp, wxHolfuy, wxZywiec, wxSecondarySrcForPage);

					// inserting the data inside a RRD file
					dataPresence.FetchDataInRRD(&wxTarget, false);

					// insertind diff-data inside RRD files
					dataPresence.FetchDiffInRRD(wxDifference);

					// configuring locale before plotting graphs
					set_locale();

					// replotting the graphs set
					dataPresence.PlotGraphsFromRRD();

					// limit precision of the wind speed
					wxTarget.NarrowPrecisionOfWindspeed();

					// and temperature also
					wxTarget.NarrowPrecisionOfTemperature();

					// generating the website
					dataPresence.GenerateWebiste(wxTarget, wxSecondarySrcForPage, locale, datetimeLocale, telemetry);

					// storing values for slew rate corrections
					wxLastTarget = wxTarget;

					if (mysqlDb.enable == true) {
						try {
							mysqlDb.OpenDBConnection();

							mysqlDb.InsertDiff(wxDifference, diffCalculator, programConfig.getStationName());

							mysqlDb.InsertIntoDb(&wxTarget);

							mysqlDb.InsertIntoDbSchema2(wxTarget, sourceConfig, programConfig.getStationName());

							if (dataPresence.SpecialTelemetry && telemetry.valid) {
								mysqlDb.InsertIntoDbSchemaTatry(wxIsTemp, telemetry, programConfig.getStationName());
							}

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

					if (batchMode && ( !aprsConfig.enable || tcpOrSerialPacketGood)) {
						break;
					}

				}
				else {
					if (Debug) {
						cout << "--- main.cpp:478 - This is not valid APRS packet" << endl;
					}

					//if (Debug)
					//	cout << "--- main.cpp:386 - Inserting data from previous frame into RRD file" << endl;
					// insert previous data into RRD file
					dataPresence.FetchDataInRRD(&wxLastTarget, true);
				}
			}
			catch (ConnectionTimeoutEx &e) {
				// if connection is timed out break internal loop to allow reconnecting
				isConnectionAlive = false;
				break;
			}
			catch (std::exception &e) {
				cout << "--- main:492 - std::exception " << e.what() << std::endl;

				if (exitOnException) {
					std::cout << "--- Exiting application";

					fDebug.close();
					return -482;
				}
			}
			catch (...) {
				cout << "--- main:502 - Unknown exception thrown during processing!" << std::endl;

				if (exitOnException) {
					std::cout << "--- Exiting application";

					fDebug.close();
					return -485;
				}
			}

		}

		if (batchMode) {
			break;
		}

		std::cout << "--- main:518 - Connection to APRS server died. Reconnecting.." << std::endl;

	} while (mainLoopExit);		// end of main loop

	std::cout << "--- Exiting application";

	fDebug.close();


	return 0;
}
