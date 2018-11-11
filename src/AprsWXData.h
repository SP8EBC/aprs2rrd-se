#ifndef APRSWXDATA_H
#define APRSWXDATA_H

#include "AprsPacket.h"
#include "NotValidWXDataEx.h"
#include <exception>
#include <queue>

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
        bool val;
        AprsWXData();
        ~AprsWXData();
		void PrintData(void);
		void ZeroCorrection(queue <AprsWXData> & qMeteo);
		float QnhQfeCorrection(float qnh, float alti);
		short DirectionCorrection(short direction, short correction);
		void DirectionCorrection(short correction);

		static int ParseData(AprsPacket input, AprsWXData* output);
        static int CopyConvert(char sign, char* input, int* output, int* counter);
		static int CopyConvert(unsigned num, char* input, int* output, int* counter);

};

class WXDataOK: public std::exception {

};

#endif // APRSWXDATA_H
