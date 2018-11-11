#include "AprsWXData.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "main.h"

AprsWXData::AprsWXData() {
    wind_speed = 0.0;
    wind_gusts = 0.0;
    wind_direction = 0;
    humidity = 0;
    temperature = 0;
    pressure = 0;
    rain60 = 0;
    rain24 = 0;
    rain_day = 0;
    val = false;
}

AprsWXData::~AprsWXData() {
}

int AprsWXData::ParseData(AprsPacket input, AprsWXData* output) {

    int i = 0;
    int conv_temp;
    char *src;

    output->val = false;

    if (*(input.Data) != '!') {
        output->val = false;
        return -1;     // to nie sa dane pogodowe
    }
    src = input.Data;
    do {
        i++;
		if (*(src + i) == 0x00)
			return -1;
		if (i > 30)
			return -1;
    } while (*(src + i) != '_'); // pominiecie pozycji i przejsce od razu do danych meteo

    output->wind_speed = 0.0;
    output->wind_gusts = 0.0;
    output->wind_direction = 0;
    output->humidity = 0;
    output->temperature = 0;
    output->pressure = 0;
    output->rain60 = 0;
    output->rain24 = 0;
    output->rain_day = 0;
    output->val = false;

    i++;    // przeskoczenie na pierwszy znak danych meteo
    if (AprsWXData::CopyConvert('/',src,&conv_temp,&i) == 0)   // kierunek    this->wind_direction = conv_temp;
    	output->wind_direction = conv_temp;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('g',src,&conv_temp,&i) == 0)   // siła
    	output->wind_speed = (float)conv_temp * 0.44;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('t',src,&conv_temp,&i) == 0)       // porywy
    	output->wind_gusts = (float)conv_temp * 0.44;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('r',src,&conv_temp,&i) == 0)   // temperatura
    	output->temperature = ((float)conv_temp - 32) / 9 * 5;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('p',src,&conv_temp,&i) == 0)   // deszcz przez ostania godzine
    	output->rain60 = conv_temp;
	else
		return -1;
	i++;
    if (AprsWXData::CopyConvert('P',src,&conv_temp,&i) == 0)   // deszcz przez ostania godzine
    	output->rain24 = conv_temp;
	else
		return -1;
	i++;
    if (AprsWXData::CopyConvert('b',src,&conv_temp,&i) == 0)   // deszcz przez ostania godzine
    	output->rain_day = conv_temp;
	else
		return -1;
	i++;
    if (AprsWXData::CopyConvert((unsigned)5,src,&conv_temp,&i) == 0)   // ciśnienie
    	output->pressure = conv_temp / 10;
	else;
	i++;
    if (AprsWXData::CopyConvert((unsigned)2,src,&conv_temp,&i) == 0)
    	output->humidity = conv_temp;
	else;
    output->val = true;
    return 0;
}

void AprsWXData::PrintData(void) {
    if (this->val == true) {
        printf("--------- DANE POGODOWE ------- \r\n");
        printf("-- Siła wiatru: %f \r\n", this->wind_speed);
        printf("-- Porywy: %f \r\n", this->wind_gusts);
        printf("-- Kierunek: %d \r\n", this->wind_direction);
        printf("-- Temperatura: %f \r\n", this->temperature);
        printf("-- Cisnienie: %d \r\n", this->pressure);
        printf("-- Wilgotność: %d \r\n", this->humidity);
    }
}

void AprsWXData::ZeroCorrection(queue <AprsWXData> & qMeteo) {
	AprsWXData temp;
	temp.pressure = 0;
	if (qMeteo.size() != 0) {
		if (this->temperature == 0 || this->humidity == 0 || this->pressure == 0) {
			temp = qMeteo.back();
			if (Debug == true)
				cout << "----- Korekcja ";
		}
		else;
		if (this->temperature == 0) {
			if (Debug == true)
				cout << "t: " << this->temperature << " -> " << temp.temperature << "; ";
			this->temperature = temp.temperature;
		}
		if (this->humidity == 0) {
			if (Debug == true)
				cout << "h: " << this->humidity << " -> " << temp.humidity << "; ";
			this->humidity = temp.humidity;
		}
		if (this->pressure == 0) {
			if (Debug == true)
				cout << "p: " << this->humidity << " -> " << temp.humidity << "; ";
			this->pressure = temp.pressure;
		}
	}
	if (this->humidity < 1 || this->humidity > 99)
		this->humidity = 1;
	cout << endl;
}

float AprsWXData::QnhQfeCorrection(float qnh, float alti) {
	return (qnh / pow (2.7182818f, (-0.000127605011f * alti) ));
}

int AprsWXData::CopyConvert(char sign, char* input, int* output, int* counter) {
    int j = 0;
    char tempbuff[9];
    memset(tempbuff, 0x00, sizeof(tempbuff));
    do {
		if (*(input + *counter) == 0x00)
			return -1;
        // siła wiatru
        tempbuff[j] = *(input + *counter);
        *counter += 1;
        j++;
		if (j > 8)
			return -1;
    } while(*(input + *counter) != sign);
    *output = atoi(tempbuff);
	return 0;
}

int AprsWXData::CopyConvert(unsigned num, char* input, int* output, int* counter) {
    unsigned j = 0;
    char tempbuff[9];
    memset(tempbuff, 0x00, sizeof(tempbuff));
    do {
		if (*(input + *counter) == 0x00)
			return -1;
        // siła wiatru
        tempbuff[j] = *(input + *counter);
        *counter += 1;
        j++;
		if (j > 8)
			return -1;
    } while(j < num);
    *output = atoi(tempbuff);
	return 0;
}

short AprsWXData::DirectionCorrection(short direction, short correction) {
    short out;

    if (direction + correction > 360)
    {
        out = (direction + correction) - 360;
    }
    else if (direction + correction < 0)
    {
        out = (direction + correction) + 360;
    }
    else
        out = direction + correction;

    return out;
}

void AprsWXData::DirectionCorrection(short correction) {
    short direction = this->wind_direction;
    short out;

    if (direction + correction > 360)
    {
        out = (direction + correction) - 360;
    }
    else if (direction + correction < 0)
    {
        out = (direction + correction) + 360;
    }
    else
        out = direction + correction;

    this->wind_direction = out;
}
