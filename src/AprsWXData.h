#ifndef APRSWXDATA_H
#define APRSWXDATA_H

#include "AprsPacket.h"
#include "NotValidWXDataEx.h"
#include "DataSourceConfig.h"
#include "Telemetry.h"
#include "WxDataSource.h"
#include <exception>
#include <queue>
#include <string>
#include <utility>

using namespace std;

class AprsWXData
{
    public:
		WxDataSource dataSource;

		std::string call;
		unsigned ssid;

		// these flags were added because they helps in DiffCalculator distinguish between pri and sec
		// 'copy' methods doen't rely on them as they were developed before
		bool is_primary;
		bool is_secondary;

        float wind_speed;
        float wind_gusts;
        int wind_direction;
        float temperature;
        short pressure;
        short rain60;
        short rain24;
        short rain_day;
        unsigned short humidity;

        bool useTemperature;	// ustawianie na true jeżeli program ma użyć zapisanej wartości temperatury
        bool useWind;
        bool usePressure;
        bool useHumidity;

		/**
		 * @brief 	Used by aprx-rf log parser only. Packet timestamp in timezone used by APRX, but 
		 * 			this timezone is NOT stored in this structure
		 * 
		*/
        boost::posix_time::ptime packetLocalTimestmp;

		/**
		 * @brief 	This is UTC timestamp of the frame, calculated by a call to @link{TimeTools::getEpochFromPtime} made 
		 * 			from @link{AprxLogParser::getLastPacketForStation}
		 * 
		 * @attention 	There is an assumption made here that APRS2RRD is run on an account with the same timezone as 
		 * 				what APRX uses. There is no information about TZ stored in aprx-rf.log file, it is just local
		 * 				time. Whatever the local time means.
		*/
		uint64_t packetUtcTimestamp;


        int packetAgeInSecondsUtc;
        int packetAgeInSecondsLocal;

	    bool convertPressure;

		/**
		 * @brief additional historic temperature send in custom & optional section like [0_1258_277][1_629_270][2_0_265][3_2516_274]
		 */
		std::vector<std::pair<uint64_t, float>> additionalTemperature;
    public:
        bool valid;

        // default constructor
        AprsWXData();

        // copy constructor
        AprsWXData(const AprsWXData& in);

        // assigment operator
        AprsWXData& operator= (AprsWXData&);

        // subtraction operator
        AprsWXData& operator-(AprsWXData&);

        ~AprsWXData();
		void PrintData(void) const;
		void ZeroCorrection(queue <AprsWXData> & qMeteo);
		float QnhQfeCorrection(float qnh, float alti);
		void NarrowPrecisionOfWindspeed();
		void NarrowPrecisionOfTemperature();
		void CheckPrimaryOrSecondaryAprsis(const DataSourceConfig & config);

		void copy(AprsWXData & source, bool withoutTemperature, bool onlyTemperature);
		void copy(float temperature, bool onlyTemperature);
		void copy(const AprsWXData & source, const DataSourceConfig & config);
		void copy(const Telemetry & source, const DataSourceConfig & config);

		// zeroing these flags to false will control which data will be inserted into RRD databse
		// This affects only RRD as webpage and MySQL ignore this
		inline static void zeroUse(AprsWXData & source) {
			source.useHumidity = false;
			source.usePressure = false;
			source.useTemperature = false;
			source.useWind = false;
		}

		static int ParseData(AprsPacket input, AprsWXData* output);
        static int CopyConvert(char sign, std::string& input, int& output, int& counter);
		static int CopyConvert(unsigned num, std::string& input, int& output, int& counter);
		static short DirectionCorrection(AprsWXData& packet, short direction, short correction);
		static void DirectionCorrection(AprsWXData& packet, short correction);

		static bool DebugOutput;

};

//class WXDataOK: public std::exception {
//
//};

#endif // APRSWXDATA_H
