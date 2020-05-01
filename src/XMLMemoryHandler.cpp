/*
 * XMLMemoryHandler.cpp
 *
 *  Created on: 01.05.2020
 *      Author: mateusz
 */

#include "XMLMemoryHandler.h"

XMLMemoryHandler::XMLMemoryHandler() {
	// TODO Auto-generated constructor stub

}

XMLMemoryHandler::~XMLMemoryHandler() {
	// TODO Auto-generated destructor stub
}

void XMLMemoryHandler::warning(const xercesc_3_1::SAXParseException& exc) {
	errorCounter++;
}

void XMLMemoryHandler::error(const xercesc_3_1::SAXParseException& exc) {
	errorCounter++;
}

void XMLMemoryHandler::fatalError(const xercesc_3_1::SAXParseException& exc) {
	errorCounter++;
}
