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
	// TODO Auto-generated constructor stub
	this->valid = false;

}

Telemetry::~Telemetry() {
	// TODO Auto-generated destructor stub
}

int Telemetry::Telemetry::ParseData(AprsPacket input, Telemetry* output) {
    char *src;
    int numi, c1i, c2i, c3i, c4i, c5i;

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

	if (Debug) {
		std::cout << "-----------------------------------------" << std::endl;
		std::cout << "--- Przetworzono dane telemetryczne -----" << std::endl;
		std::cout << "--- Kanal 1 =  " << output->getCh1() << std::endl;
		std::cout << "--- Kanal 2 =  " << output->getCh2() << std::endl;
		std::cout << "--- Kanal 3 =  " << output->getCh3() << std::endl;
		std::cout << "--- Kanal 4 =  " << output->getCh4() << std::endl;
		std::cout << "--- Kanal 5 =  " << output->getCh5() << std::endl;
		std::cout << "-----------------------------------------" << std::endl;

	}

	output->valid = true;

	delete str;
    return 0;

}

float Telemetry::getCh1() {
	return (ch1a * ch1 * ch1) + (ch1b * ch1) + ch1c;
}

float Telemetry::getCh2() {
	return (ch2a * ch2 * ch2) + (ch2b * ch2) + ch2c;

}

float Telemetry::getCh3() {
	return (ch3a * ch3 * ch3) + (ch3b * ch3) + ch3c;

}

float Telemetry::getCh4() {
	return (ch4a * ch4 * ch4) + (ch4b * ch4) + ch4c;

}

float Telemetry::getCh5() {
	return (ch5a * ch5 * ch5) + (ch5b * ch5) + ch5c;

}
