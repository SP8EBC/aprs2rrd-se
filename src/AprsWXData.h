#ifndef APRSWXDATA_H
#define APRSWXDATA_H

#include "AprsPacket.h"
#include "NotValidWXDataEx.h"
#include <exception>
#include <queue>
#include <string>

using namespace std;

class AprsWXData
{
    public:
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
    public:
        bool valid;

        // default constructor
        AprsWXData();

        // copy constructor
        AprsWXData(const AprsWXData& in);

        // assigment operator
        AprsWXData& operator= (AprsWXData&);

        ~AprsWXData();
		void PrintData(void);
		void ZeroCorrection(queue <AprsWXData> & qMeteo);
		float QnhQfeCorrection(float qnh, float alti);

		void copy(AprsWXData & source, bool withoutTemperature, bool onlyTemperature);
		void copy(float temperature, bool onlyTemperature);


		static int ParseData(AprsPacket input, AprsWXData* output);
        static int CopyConvert(char sign, std::string& input, int& output, int& counter);
		static int CopyConvert(unsigned num, std::string& input, int& output, int& counter);
		static short DirectionCorrection(AprsWXData& packet, short direction, short correction);
		static void DirectionCorrection(AprsWXData& packet, short correction);

		bool DebugOutput;

};

class WXDataOK: public std::exception {

};

#endif // APRSWXDATA_H
