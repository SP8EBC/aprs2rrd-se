#include "AprsWXData.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


#include "main.h"

bool AprsWXData::DebugOutput = false;

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
    valid = false;

    useHumidity = false;
    usePressure = false;
    useTemperature = false;
    useWind = false;

    //DebugOutput = false;
}

AprsWXData::~AprsWXData() {
}

int AprsWXData::ParseData(AprsPacket input, AprsWXData* output) {

    int i = 0;
    int conv_temp;
    //char *src;
    std::vector<std::string> extractedWx;	// this vector will be used as an output from boost::split method

    output->valid = false;

    if (*(input.Data) != '!') {
        output->valid = false;
        return -1;     // to nie sa dane pogodowe
    }
    //src = input.Data;

    // converting char array to copy of string class for convinence
    std::string source(input.Data);

    // the 'split' method doesn't copy a character which was used as split point!
    boost::split(extractedWx, source, boost::is_any_of("_"));

    // if the extracted vector has less than 2 elements it means that this is not
    // valid frame
    if (extractedWx.size() < 2)
    	return -1;

    // Underscore '_' divides an APRS wx frame to part with position data (first one) and
    // the second one which holds measurement data
    std::string wxData = extractedWx.at(1);



/*
    do {
        i++;
		if (*(src + i) == 0x00)
			return -1;
		if (i > 30)
			return -1;
    } while (*(src + i) != '_'); // pominiecie pozycji i przejsce od razu do danych meteo
*/

    output->wind_speed = 0.0;
    output->wind_gusts = 0.0;
    output->wind_direction = 0;
    output->humidity = 0;
    output->temperature = 0;
    output->pressure = 0;
    output->rain60 = 0;
    output->rain24 = 0;
    output->rain_day = 0;
    output->valid = false;

    //i++;    // przeskoczenie na pierwszy znak danych meteo
    if (AprsWXData::CopyConvert('/',wxData,conv_temp,i) == 0)   // kierunek    this->wind_direction = conv_temp;
    	output->wind_direction = conv_temp;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('g',wxData,conv_temp,i) == 0)   // siła
    	output->wind_speed = (float)conv_temp * 0.44;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('t',wxData,conv_temp,i) == 0)       // porywy
    	output->wind_gusts = (float)conv_temp * 0.44;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('r',wxData,conv_temp,i) == 0)   // temperatura
    	output->temperature = ((float)conv_temp - 32) / 9 * 5;
	else
		return -1;
    i++;
    if (AprsWXData::CopyConvert('p',wxData,conv_temp,i) == 0)   // deszcz przez ostania godzine
    	output->rain60 = conv_temp;
	else
		return -1;
	i++;
    if (AprsWXData::CopyConvert('P',wxData,conv_temp,i) == 0)   // deszcz przez ostania godzine
    	output->rain24 = conv_temp;
	else
		return -1;
	i++;
    if (AprsWXData::CopyConvert('b',wxData,conv_temp,i) == 0)   // deszcz przez ostania godzine
    	output->rain_day = conv_temp;
	else
		return -1;
	i++;
    if (AprsWXData::CopyConvert((unsigned)5,wxData,conv_temp,i) == 0)   // ciśnienie
    	output->pressure = conv_temp / 10;
	else;
	i++;
    if (AprsWXData::CopyConvert((unsigned)2,wxData,conv_temp,i) == 0)
    	output->humidity = conv_temp;
	else;
    output->valid = true;
    return 0;
}

void AprsWXData::PrintData(void) {
    if (this->valid == true && AprsWXData::DebugOutput == true) {
        printf("--------- DANE POGODOWE -------\r");
        printf("-- Siła wiatru: %f \r", this->wind_speed);
        printf("-- Porywy: %f \r", this->wind_gusts);
        printf("-- Kierunek: %d \r", this->wind_direction);
        printf("-- Temperatura: %f \r", this->temperature);
        printf("-- Cisnienie: %d \r", this->pressure);
        printf("-- Wilgotność: %d \r", this->humidity);
        printf("--------------------------------\r");
    }
}

void AprsWXData::ZeroCorrection(queue <AprsWXData> & qMeteo) {
	AprsWXData temp;
	temp.pressure = 0;
	if (qMeteo.size() != 0) {
		if (this->temperature == 0 || this->humidity == 0 || this->pressure == 0) {
			temp = qMeteo.back();
			if (this->DebugOutput == true)
				cout << "----- Korekcja ";
		}
		else;
		if (this->temperature == 0) {
			if (this->DebugOutput == true)
				cout << "t: " << this->temperature << " -> " << temp.temperature << "; ";
			this->temperature = temp.temperature;
		}
		if (this->humidity == 0) {
			if (this->DebugOutput == true)
				cout << "h: " << this->humidity << " -> " << temp.humidity << "; ";
			this->humidity = temp.humidity;
		}
		if (this->pressure == 0) {
			if (this->DebugOutput == true)
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

/**
 * This method shall read an input string, starting from position given by value of 'counter'
 * parameter and ends at first presence of 'sign' character. Read numerical value shall be
 * lexical casted to int and stored at 'output' value
 */
int AprsWXData::CopyConvert(char sign, std::string& input, int& output, int& counter) {
    size_t position;

    // finding a position of character 'sign' in input string starting from given position 'counter'
    position = input.find_first_of(sign, counter);

    // checking if character 'sign' is present in an input string or not
    if (position == std::string::npos)
    	return -1;

    // creating aux substring with value for converrsion
    std::string valueToConv = input.substr(counter, position - counter);

    try {
    	// converting value
    	output = boost::lexical_cast<int>(valueToConv);
    }
    catch (const boost::bad_lexical_cast& ex) {
    	std::cout << ex.what() << std::endl;
    	output = 0;
    }

    // storing a position where 'sign' was found
    counter = (int)position;

	return 0;
}

/**
 * This method shall read an input string, starting from position given by value of 'counter'
 * parameter and ends at position 'counter' + 'num'. Read numerical value shall be
 * lexical casted to int and stored at 'output' value
 */
int AprsWXData::CopyConvert(unsigned num, std::string& input, int& output, int& counter) {
    char tempbuff[9];
    memset(tempbuff, 0x00, sizeof(tempbuff));

    // if caler want to read more characters than input string stores
    if (input.length() < num + counter)
    	return -1;

    // creating aux substring with value for converrsion
    std::string valueToConv = input.substr(counter, num);

    try {
    	// converting value
    	output = boost::lexical_cast<int>(valueToConv);
    }
    catch (const boost::bad_lexical_cast& ex) {
    	std::cout << ex.what() << std::endl;
    	output = 0;
    }

    // storing a position where 'sign' was found
    counter = (int)num + counter;

	return 0;
}

AprsWXData::AprsWXData(const AprsWXData& in) {

	this->humidity = in.humidity;
	this->pressure = in.pressure;
	this->rain24 = in.rain24;
	this->rain60 = in.rain60;
	this->rain_day = in.rain_day;
	this->temperature = in.temperature;
	this->wind_direction = in.wind_direction;
	this->wind_gusts = in.wind_gusts;
	this->wind_speed = in.wind_speed;

	this->valid = in.valid;

	this->useHumidity = in.useHumidity;
	this->usePressure = in.usePressure;
	this->useWind = in.useWind;
	this->useTemperature = in.useTemperature;

	this->DebugOutput = in.DebugOutput;
}

AprsWXData& AprsWXData::operator =(AprsWXData& _in) {

	this->humidity = _in.humidity;
	this->pressure = _in.pressure;
	this->rain24 = _in.rain24;
	this->rain60 = _in.rain60;
	this->rain_day = _in.rain_day;
	this->temperature = _in.temperature;
	this->wind_direction = _in.wind_direction;
	this->wind_gusts = _in.wind_gusts;
	this->wind_speed = _in.wind_speed;

	this->valid = _in.valid;

	this->useHumidity = _in.useHumidity;
	this->usePressure = _in.usePressure;
	this->useWind = _in.useWind;
	this->useTemperature = _in.useTemperature;

	this->DebugOutput = _in.DebugOutput;

	return * this;
}

void AprsWXData::copy(AprsWXData& source, bool withoutTemperature, bool onlyTemperature) {

	if (!source.valid) {
		return;
	}

	this->valid = true;

	if (onlyTemperature) {
		this->useTemperature = true;
		this->usePressure = false;
		this->useWind = false;
		this->useHumidity = false;

		this->temperature = source.temperature;

		return;
	}

	if (withoutTemperature) {
		this->useTemperature = false;
		this->usePressure = true;
		this->useWind = true;
		this->useHumidity = true;
	}
	else {
		this->useTemperature = true;
		this->usePressure = true;
		this->useWind = true;
		this->useHumidity = true;

		this->temperature = source.temperature;
	}

	this->humidity = source.humidity;
	this->pressure = source.pressure;
	this->rain24 = source.rain24;
	this->rain60 = source.rain60;
	this->rain_day = source.rain_day;
	this->wind_direction = source.wind_direction;
	this->wind_gusts = source.wind_gusts;
	this->wind_speed = source.wind_speed;

	return;

}

void AprsWXData::copy(float temperature, bool onlyTemperature) {
	if (onlyTemperature) {
		this->useTemperature = true;
		this->usePressure = false;
		this->useWind = false;
		this->useHumidity = false;
	}

	this->useTemperature = true;
	this->temperature = temperature;

}



short AprsWXData::DirectionCorrection(AprsWXData& packet, short direction, short correction) {
    short out;

	if (!packet.valid)
		return -255;	// in case of error return invalid wind direction

	if (correction == 0)
		return -255;

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

void AprsWXData::DirectionCorrection(AprsWXData& packet, short correction) {
	if (!packet.valid)
		return;

	if (correction == 0)
		return;

    short direction = packet.wind_direction;
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

    packet.wind_direction = out;
}
