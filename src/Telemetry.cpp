/*
 * Telemetry.cpp
 *
 *  Created on: 26.08.2018
 *      Author: mateusz
 */

#include "Telemetry.h"
#include "NotValidWXDataEx.h"

#include <string>
#include <stdexcept>
#include <iostream>

bool Telemetry::Debug = false;

Telemetry::Telemetry() {
	this->valid = false;

	this->ch1 = 0;
	this->ch1a = 0.0f;
	this->ch1b = 1.0f;
	this->ch1c = 0.0f;

	this->ch2 = 0;
	this->ch2a = 0.0f;
	this->ch2b = 1.0f;
	this->ch2c = 0.0f;

	this->ch3 = 0;
	this->ch3a = 0.0f;
	this->ch3b = 1.0f;
	this->ch3c = 0.0f;

	this->ch4  = 0;
	this->ch4a = 0.0f;
	this->ch4b = 1.0f;
	this->ch4c = 0.0f;

	this->ch5  = 0;
	this->ch5a = 0.0f;
	this->ch5b = 1.0f;
	this->ch5c = 0.0f;

	this->digital = 0;

	this->num = 0;

}

Telemetry::~Telemetry() {
}

// TODO Do something with the fact that there is no way to choose if the telemetry shall be parsed
// 		from Secondary or Primary call
int Telemetry::Telemetry::ParseData(AprsPacket input, Telemetry* output) {
    char *src;
    int numi, c1i, c2i, c3i, c4i, c5i, digi = 0;

    output->ch1 = 0;
    output->ch2 = 0;
    output->ch3 = 0;
    output->ch4 = 0;
    output->ch5 = 0;

    if (*(input.Data) != 'T') {
        output->valid = false;
        return -1;
    }
    src = input.Data;

    std::string *str = new std::string(src);

    // numer kolejny ramki telemetrycznej
    std::size_t hashPosition = str->find_first_of("#");
    if (hashPosition == std::string::npos) {
        output->valid = false;
    	delete str;
        return -1;

    }
    std::string num = str->substr(hashPosition + 1, 3);
    try {
    	numi = std::stoi(num, nullptr, 10);
    }
    catch (std::invalid_argument &e) {
        output->valid = false;
    	delete str;
    	return -1;
    }
    output->num = numi;

    // kanał pierwszy
    std::size_t firstComma = str->find_first_of(",", hashPosition);
    if (firstComma == std::string::npos) {
        output->valid = false;
    	delete str;
        return -1;

    }
    std::string c1 = str->substr(firstComma + 1, 3);
    try {
    	c1i = std::stoi(c1, nullptr, 10);
	}
	catch (std::invalid_argument &e) {
        output->valid = false;
		delete str;
		return -1;
	}
    output->ch1 = c1i;

    // kanał drugi
    std::size_t secondComma = str->find_first_of(",", firstComma + 1);
    if (secondComma == std::string::npos) {
        output->valid = false;
    	delete str;
        return -1;

    }
    std::string c2 = str->substr(secondComma + 1, 3);
    try {
		c2i = std::stoi(c2, nullptr, 10);
	}
	catch (std::invalid_argument &e) {
        output->valid = false;
		delete str;
		return -1;
	}
    output->ch2 = c2i;

    // kanał trzeci
    std::size_t thirdComma = str->find_first_of(",", secondComma + 1);
    if (secondComma == std::string::npos) {
        output->valid = false;
    	delete str;
        return -1;

    }
    std::string c3 = str->substr(thirdComma + 1, 3);
    try {
		c3i = std::stoi(c3, nullptr, 10);
	}
	catch (std::invalid_argument &e) {
        output->valid = false;
		delete str;
		return -1;
	}
    output->ch3 = c3i;

    // kanał czwarty
    std::size_t fourthComma = str->find_first_of(",", thirdComma + 1);
    if (secondComma == std::string::npos) {
        output->valid = false;
    	delete str;
        return -1;

    }
    std::string c4 = str->substr(fourthComma + 1, 3);
    try {
    	c4i = std::stoi(c4, nullptr, 10);
	}
	catch (std::invalid_argument &e) {
        output->valid = false;
		delete str;
		return -1;
	}
    output->ch4 = c4i;

    // kanał piąty
    std::size_t fifthComma = str->find_first_of(",", fourthComma + 1);
    if (secondComma == std::string::npos) {
        output->valid = false;
    	delete str;
        return -1;

    }
    std::string c5 = str->substr(fifthComma + 1, 3);
    try {
		c5i = std::stoi(c5, nullptr, 10);
	}
	catch (std::invalid_argument &e) {
        output->valid = false;
		delete str;
		return -1;
	}
    output->ch5 = c5i;

    // digital channels
    std::size_t sixthComma = str->find_first_of(",", fifthComma + 1);
    if (sixthComma == std::string::npos) {
        output->valid = false;
    	delete str;
        return -1;

    }
    std::string dig = str->substr(sixthComma + 1, 8);
    for (char c : dig) {
    	digi |= (c == '0' ? 0 : 1);
    	digi <<= 1;
    }
    digi >>= 1;
    output->digital = digi;

	if (Telemetry::Debug) {
		std::cout << "-----------------------------------------" << std::endl;
		std::cout << "--- Telemetry data have been parsed -----" << std::endl;
		std::cout << "--- Kanal 1 =  " << output->getCh1() << std::endl;
		std::cout << "--- Kanal 2 =  " << output->getCh2() << std::endl;
		std::cout << "--- Kanal 3 =  " << output->getCh3() << std::endl;
		std::cout << "--- Kanal 4 =  " << output->getCh4() << std::endl;
		std::cout << "--- Kanal 5 =  " << output->getCh5() << std::endl;
		std::cout << "--- Digital =  b" << dig << ", " << (int)output->digital << std::endl;
		std::cout << "-----------------------------------------" << std::endl;

	}

	output->valid = true;

	output->call = input.SrcAddr + "-" + std::to_string((int)input.SrcSSID);

	delete str;
    return 0;

}

float Telemetry::getCh1() const {
	return (ch1a * ch1 * ch1) + (ch1b * ch1) + ch1c;
}

float Telemetry::getCh2() const {
	return (ch2a * ch2 * ch2) + (ch2b * ch2) + ch2c;

}

float Telemetry::getCh3() const {
	return (ch3a * ch3 * ch3) + (ch3b * ch3) + ch3c;

}

float Telemetry::getCh4() const {
	return (ch4a * ch4 * ch4) + (ch4b * ch4) + ch4c;

}

float Telemetry::getCh5() const {
	return (ch5a * ch5 * ch5) + (ch5b * ch5) + ch5c;

}
