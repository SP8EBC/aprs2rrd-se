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
#include "AprxLogParser.h"
#include "AprxLogParserConfig.h"
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
#include "WeatherlinkClient.h"
#include "BannerCreator.h"
#include "BannerCreatorConfig.h"
#include "Thingspeak.h"

#include "ConnectionTimeoutEx.h"
#include "DataPresentation.h"

#include "TimeTools.h"

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

bool telemetryParsed = false;
bool weatherParsed = false;

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

	std::cout << "--- main:100 - Using configuration from file: " << configFn << std::endl;

	Config config;
	ProgramConfig programConfig(configFn);

	SerialAsioThread * serialThread;
	AprsThreadConfig aprsConfig;
	AprxLogParser aprxLogParser;
	AprxLogParserConfig aprxLogParserConfig;
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
	WeatherlinkClient weatherlinkClient;
	BannerCreatorConfig bannerCreatorConfig;
	std::unique_ptr<BannerCreator> bannerCreator; // = std::make_unique<BannerCreator>(bannerCreatorConfig);
	Thingspeak_Config thingspeakConfig;
	std::unique_ptr<Thingspeak> thingspeak;

	char datetimeLocale[16];
	char * currrentLocale;

	Telemetry telemetry;

	RRDFileDefinition sVectorRRDTemp;
	PlotFileDefinition cVectorPNGTemp;

	AprsWXData wxIsTemp, wxSerialTemp, wxTelemetry, wxHolfuy, wxZywiec, wxDavis, wxThingspeak;
	AprsWXData wxTarget; // target wx data to be inserted into RRD DB & printed onto website
	AprsWXData wxLastTarget;
	AprsWXData wxDifference;
	AprsWXData wxSecondarySrcForPage;
	AprsPacket isRxPacket;
	AprsPacket serialRxPacket;

	queue <AprsPacket> qPackets;
	queue <AprsWXData> qMeteo;

	// data parsed from APRX rf-log file. used only if this is enabled
	std::vector<AprsWXData> wxDataFromAprx;

	// iterator 
	std::vector<AprsWXData>::const_iterator wxDataFromAprxIterator;

	/**
	 * FIXME: test
	*/
	// aprxLogParserConfig.enabled = true;
	// aprxLogParserConfig.batchLoad = false;
	// aprxLogParserConfig.logFile = "./test_wdir/aprx-rf-2.log";
	// aprxLogParserConfig.batchLoadFrom = 1703415792ULL;
	// aprxLogParserConfig.batchLoadTo = 1703484192ULL;
	// aprxLogParserConfig.sourceCallsign = "SR9NSK";
	// aprxLogParserConfig.sourceSsid = 1;
	// aprxLogParserConfig.maximumPacketAge = 120;
	// aprxLogParserConfig.logTimeInLocal = true;

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
		cout << "--- main:186 - Opening configuration file" << endl;
	}

	catch(const FileIOException &ex)
	{
		printf("--- main:191 - The configuration file cannot be opened.\r\n");
		return -1;
	}
	catch(const ParseException &ex) {
		printf("--- main:195 - Error during parsing a content of configuration file near line %d \r\n", ex.getLine());
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
		programConfig.getWeatherlinkConfig(weatherlinkClient.config);
		programConfig.getBannerConfig(bannerCreatorConfig);
		programConfig.getAprxLogParserConfig(aprxLogParserConfig);
		programConfig.getThingspeakConfig(thingspeakConfig);

		dataPresence.DebugOutput = Debug;
		mysqlDb.Debug = Debug;
		AprsWXData::DebugOutput = Debug;
		Telemetry::Debug = Debug;

		sourceConfig.holfuyNumber = holfuyConfig.stationId;
		sourceConfig.zywiecNumber = zywiecMeteoConfig.stationId;
		sourceConfig.davisWeatherlinkNumber = weatherlinkClient.config.DID;

		set_locale();

		(void)setlocale(LC_NUMERIC, "en_US.UTF-8");

	}
	catch (const SettingNotFoundException &ex) {
		cout << "--- main:238 - Unrecoverable error during configuration file loading!" << endl;
		return -3;
	}

	aprsConfig.RetryServerLookup = true;

	cout << "--- main:244 - Configuration parsed successfully" << endl;

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

	ProgramConfig::printConfigInPl(
			mysqlDb,
			aprsConfig,
			dataPresence,
			RRDCount,
			PlotsCount,
			zywiecMeteoConfig,
			holfuyConfig,
			diffCalculator,
			sourceConfig,
			pressureCalculator,
			limiter,
			locale,
			weatherlinkClient.config,
			bannerCreatorConfig, 
			thingspeakConfig);

	cout << "--- main:277 - float epsilon: " << std::numeric_limits<float>::epsilon() << endl;

	cout << "--- main:279 - exitOnException: " << exitOnException << endl;

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

	if (thingspeakConfig.enable) {
		thingspeak = std::make_unique<Thingspeak>(thingspeakConfig);
	}

	// creating a new copy of ASIO thread
	asioThread = new AprsAsioThread(aprsConfig, DEFAULT_APRS_SERVER_TIMEOUT_SECONDS, syncCondition, syncLock);

	// setting a logging level
	asioThread->DebugOutput = Debug;
	serialThread->debug = Debug;

	// check operation mode chosen by an user
	batchMode = programConfig.getBatchMode();
	mainLoopExit = !batchMode;

	if ((aprxLogParserConfig.enabled && aprsConfig.enable) || (aprxLogParserConfig.enabled && serialConfig.enable)) {
		std::cout << "--- main:312 - You cannot use APRX rf-log file parser and aprs-is client at the same time" << std::endl;
		return -7;
	}
	else if (aprxLogParserConfig.enabled) {
		batchMode = true;
		mainLoopExit = false;
		TimeTools::initBoostTimezones("date_time_zonespec.csv");

		sourceConfig.primaryCall = aprxLogParserConfig.sourceCallsign;
		sourceConfig.primarySsid = aprxLogParserConfig.sourceSsid;

		sourceConfig.secondaryCall = aprxLogParserConfig.sourceCallsign;
		sourceConfig.secondarySsid = aprxLogParserConfig.sourceSsid;
	}

	if (!batchMode && !aprsConfig.enable && !serialConfig.enable) {
		std::cout << "--- main:328 - You cannot run continuous mode w/o APRS-IS connection or Serial port enabled" << std::endl;
		return -6;
	}

	if (batchMode) {
		std::cout << "--- main:333 - RUNNING IN BATCH MODE" << std::endl;
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

		// if APRX rf-log file parser is enabled
		if (aprxLogParserConfig.enabled) {

			// create an instance of aprx rf-log parser
			AprxLogParser aprxLogParser(aprxLogParserConfig.logFile, aprxLogParserConfig.logTimeInLocal);
			
			std::vector<AprsWXData> unfilteredPackets;

			// check if batch load is selected 
			if (aprxLogParserConfig.batchLoad) {
				// get all packets in configured timerange
				unfilteredPackets = aprxLogParser.getAllWeatherPacketsInTimerange(aprxLogParserConfig.batchLoadFrom, aprxLogParserConfig.batchLoadTo);
			}
			else {
				// if not, get all packets for last X minutes
				unfilteredPackets = aprxLogParser.getAllWeatherPacketsInTimerange(aprxLogParserConfig.maximumPacketAge, 0, true);
			}

			//  check if any packets have been found
			if (unfilteredPackets.size() == 0) {
				std::cout << "--- main:376 - No weather packets have been found in rf-log" << std::endl;
				// no sense in continuing execution
				isConnectionAlive = false;
			}
			else {
				// filter parsed data to keep only packets sent by station with selected callsign and ssid
				wxDataFromAprx = AprxLogParser::filterPacketsPerCallsign(aprxLogParserConfig.sourceCallsign, aprxLogParserConfig.sourceSsid, unfilteredPackets);
			}

			if (wxDataFromAprx.size() == 0) {
				std::cout << "--- main:386 - No weather packets from rf-log left after filtering" << std::endl;
				// no sense in continuing execution
				isConnectionAlive = false;			
			}
			else {
				wxDataFromAprxIterator = wxDataFromAprx.begin();
			}
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
						weatherParsed = AprsWXData::ParseData(isRxPacket, &wxIsTemp);

						// Parsing telemetry data
						telemetryParsed = Telemetry::ParseData(isRxPacket, &telemetry);

						// marking if this packet was received from primary or secondary callsign (or none of them)
						wxIsTemp.CheckPrimaryOrSecondaryAprsis(sourceConfig);

						// storing the telemetry values in db. This metod checks internally if that function is enabled
						// and telemetry data are valid.
						mysqlDb.InsertTelmetry(telemetry, programConfig.getStationName());

						if (telemetry.valid) {
							// insert battery voltage into RRD
							dataPresence.FetchBatteryVoltageInRRD(telemetry.getBatteryVoltage());

							try {
								mysqlDb.OpenDBConnection();

								mysqlDb.InsertIntoDbSchemaTatry(isRxPacket, wxIsTemp, telemetry, programConfig.getStationName(), weatherParsed, telemetryParsed);

								mysqlDb.CloseDBConnection();
							}
							catch(UnsufficientConfig &e) {
								cout << e.what();
							}
							catch(BadSrvAddr &e) {
								cout << e.what();
							}
							catch(ConnError &e) {
								cout << e.what();
							}
							catch (AlreadyDisconnected &e) {
								cout << e.what();
							}

						}

						// wait for another packet if not WX data has been received. Protect against
						// flooding with a lot of data from Holfuy after each heartbeat message from APRS-IS
						if (!wxIsTemp.valid) {
							continue;
						}

						// setting the source of this wx packet, by default assume that it is primary
						//wxIsTemp.dataSource = WxDataSource::IS_PRIMARY;
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
						wxSerialTemp.dataSource = WxDataSource::SERIAL;
					}
					else;

					// dwonload data from Zywiec meteo system
					if (zywiecMeteoConfig.enable) {
						zywiecMeteo->downloadMeasuresFromRangeForStation(zywiecMeteoConfig.stationId, response);

						zywiecMeteo->parseJson(response, wxZywiec);

						std::cout << "--- main:492 - Parsing data from Zywiec county meteo system API" << std::endl;

						wxZywiec.PrintData();
					}

					// downloading Holfuy data if it is enabled
					if (holfuyConfig.enable) {
						holfuyClient->download();

						holfuyClient->parse();

						holfuyClient->getWxData(wxHolfuy);

						std::cout << "--- main:505 - Printing data downloaded & parsed from Holfuy API. Ignore 'use' flags" << std::endl;

						wxHolfuy.PrintData();
					}

					if (weatherlinkClient.config.enable) {
						weatherlinkClient.download();

						weatherlinkClient.parse();

						weatherlinkClient.getWxData(wxDavis);
					}

					if (thingspeakConfig.enable) {
						thingspeak->download();

						thingspeak->getWeatherData(wxThingspeak);
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

					if (wxZywiec.valid) {
						wxTarget.copy(wxZywiec, sourceConfig);
					}

					if (wxHolfuy.valid) {
						wxTarget.copy(wxHolfuy, sourceConfig);
					}

					if (wxDavis.valid) {
						wxTarget.copy(wxDavis, sourceConfig);
					}

					if (wxThingspeak.valid) {
						wxTarget.copy(wxThingspeak, sourceConfig);
					}

					if (aprxLogParserConfig.enabled) {
						// get packet from current iterator position
						AprsWXData currentPacket = *wxDataFromAprxIterator;

						// set the source of this packet to allow further processing
						currentPacket.dataSource = WxDataSource::IS_PRIMARY;

						wxTarget.copy(currentPacket, sourceConfig);

						// copy timestamps
						wxTarget.packetUtcTimestamp = currentPacket.packetUtcTimestamp;
						wxTarget.packetLocalTimestmp = currentPacket.packetLocalTimestmp;
					}

					if (!wxTarget.valid) {
						if (batchMode) {
							std::cout << "-- main:587 - No valid data have been received from configured sources!!" << std::endl;

							throw std::exception();
						}
						else {
							continue;
						}
					}

					// calculating the difference between sources according to user configuration
					// if this feature is disabled completely the function will do nothing and return immediately
					diffCalculator.calculate(wxIsTemp, wxSerialTemp, wxHolfuy, wxZywiec, wxDavis, telemetry, wxDifference);

					// recalculating pressure according to user configuration. If recalculation is not enabled
					// the metod will return the same value as given on input
					if (wxTarget.convertPressure)
						wxTarget.pressure = (int16_t)::roundf(pressureCalculator.convertPressure(wxTarget.pressure, wxTarget.temperature));

					// seting this flag to false will prevent reconverting pressure when its value hasn't been updated
					wxTarget.convertPressure = false;

					// each method below checks if passed WX packet is valid and if no they will
					// exit immediately witout performing any changes

					// printing target data
					std::cout << "--- main:612 - Printing target WX data which will be used for further processing." << std::endl;
					wxTarget.PrintData();

					// limiting slew rates for measurements
					limiter.limitFromSingleFrame(wxLastTarget, wxTarget);

					// geting the data for second source
					dataPresence.GetSecondarySource(wxIsTemp, wxSerialTemp, wxHolfuy, wxZywiec, wxDavis, wxSecondarySrcForPage);

					// inserting the data inside a RRD file
					dataPresence.FetchDataInRRD(&wxTarget, false);

					// limit precision of the wind speed
					wxTarget.NarrowPrecisionOfWindspeed();

					// and temperature also
					wxTarget.NarrowPrecisionOfTemperature();

					// insertind diff-data inside RRD files
					dataPresence.FetchDiffInRRD(wxDifference);

					// configuring locale before plotting graphs
					set_locale();

					// do not generate HTML and RRD plots if there is no sense to do so
					if (!aprxLogParserConfig.batchLoad) {
						// replotting the graphs set
						dataPresence.PlotGraphsFromRRD();

						// generating the website
						dataPresence.GenerateWebiste(wxTarget, wxSecondarySrcForPage, locale, datetimeLocale, telemetry);
					}

					if (bannerCreatorConfig.enable) {
						// reinitialize banner generator
						bannerCreator = std::make_unique<BannerCreator>(bannerCreatorConfig);

						// generate banner
						bannerCreator->createBanner(wxTarget);

						// save banner on disk
						bannerCreator->saveToDisk(bannerCreatorConfig.outputFile);
					}

					// storing values for slew rate corrections
					wxLastTarget = wxTarget;

					if (mysqlDb.enable == true) {
						try {
							mysqlDb.OpenDBConnection();

							mysqlDb.InsertDiff(wxDifference, diffCalculator, programConfig.getStationName());

							mysqlDb.InsertIntoDb(&wxTarget);

							mysqlDb.InsertIntoDbSchema2(isRxPacket, wxTarget, sourceConfig, programConfig.getStationName());

							if (dataPresence.SpecialTelemetry && telemetry.valid) {
								mysqlDb.InsertIntoDbSchemaTatry(isRxPacket, wxIsTemp, telemetry, programConfig.getStationName(), weatherParsed, telemetryParsed);
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


					if (aprxLogParserConfig.enabled) {
						
						// move iterator forward
						wxDataFromAprxIterator++;

						// check if the and of data has been reached
						if (wxDataFromAprxIterator == wxDataFromAprx.end()) {
							isConnectionAlive = false;

							break;
						}
					}
					else {
						if (batchMode && ( !aprsConfig.enable || tcpOrSerialPacketGood)) {
							break;
						}
					}

				}
				else {
					if (Debug) {
						cout << "--- main.cpp:710 - This is not valid APRS packet" << endl;
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
				cout << "--- main:697 - std::exception " << e.what() << std::endl;

				if (exitOnException) {
					std::cout << "--- Exiting application";

					fDebug.close();
					return -482;
				}
			}
			catch (...) {
				cout << "--- main:707 - Unknown exception thrown during processing!" << std::endl;

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

		std::cout << "--- main:723 - Connection to APRS server died. Reconnecting.." << std::endl;

	} while (mainLoopExit);		// end of main loop

	std::cout << "--- Exiting application";

	fDebug.close();


	return 0;
}
