#ifndef APRSWXDATA_H
#define APRSWXDATA_H

#include "AprsPacket.h"
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
    public:
        bool val;
        AprsWXData();
        ~AprsWXData();
        char ParseData(AprsPacket* input);
		void PrintData(void);
		void ZeroCorrection(queue <AprsWXData> & qMeteo);
		float QnhQfeCorrection(float qnh, float alti);
        int CopyConvert(char sign, char* input, int* output, int* counter);
		int CopyConvert(unsigned num, char* input, int* output, int* counter);

};

class NotValidWXData: public std::exception {

};

class WXDataOK: public std::exception {

};

#endif // APRSWXDATA_H
