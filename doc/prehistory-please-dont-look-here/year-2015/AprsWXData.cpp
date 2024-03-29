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
}

AprsWXData::~AprsWXData() {
}

char AprsWXData::ParseData(AprsPacket* input) {
    int i = 0;
//    int j = 0;
    int conv_temp;
//    char tempbuff[9];
    char *src;
//    memset(tempbuff, 0x00, sizeof(tempbuff));
    if (*(input->Data) != '!') {
        this->val = false;
        throw NotValidWXData();     // to nie sa dane pogodowe
    }
    src = input->Data;
    do {
        i++;
		if (*(src + i) == 0x00) 
			return 1;
    } while (*(src + i) != '_'); // pominiecie pozycji i przejsce od razu do danych meteo
    i++;    // przeskoczenie na pierwszy znak danych meteo
    if (this->CopyConvert('/',src,&conv_temp,&i) == 0)   // kierunek    this->wind_direction = conv_temp;
		this->wind_direction = conv_temp;
	else
		return -1;
    i++;
    if (this->CopyConvert('g',src,&conv_temp,&i) == 0)   // siła
		this->wind_speed = (float)conv_temp * 0.44;
	else
		return -1;
    i++;
    if (this->CopyConvert('t',src,&conv_temp,&i) == 0)       // porywy
		this->wind_gusts = (float)conv_temp * 0.44;
	else
		return -1;
    i++;
    if (this->CopyConvert('r',src,&conv_temp,&i) == 0)   // temperatura
		this->temperature = ((float)conv_temp - 32) / 9 * 5;
	else
		return -1;
    i++;
    if (this->CopyConvert('p',src,&conv_temp,&i) == 0)   // deszcz przez ostania godzine
		this->rain60 = conv_temp;
	else
		return -1;
	i++;
    if (this->CopyConvert('P',src,&conv_temp,&i) == 0)   // deszcz przez ostania godzine
		this->rain24 = conv_temp;
	else
		return -1;	
	i++;
    if (this->CopyConvert('b',src,&conv_temp,&i) == 0)   // deszcz przez ostania godzine
		this->rain_day = conv_temp;
	else
		return -1;		
	i++;
    if (this->CopyConvert((unsigned)5,src,&conv_temp,&i) == 0)   // ciśnienie
		this->pressure = conv_temp / 10;
	else;
    this->val = true;
    throw WXDataOK();
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
    } while(j < num);
    *output = atoi(tempbuff);
	return 0;
}


