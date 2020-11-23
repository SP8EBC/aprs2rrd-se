/*
 * XMLMemoryHandler.h
 *
 *  Created on: 01.05.2020
 *      Author: mateusz
 */

#ifndef XMLMEMORYHANDLER_H_
#define XMLMEMORYHANDLER_H_

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XercesDefs.hpp>

#ifdef USE_XERCES_3_2
using namespace xercesc_3_2;
#else
using namespace xercesc_3_1;
#endif

class XMLMemoryHandler : public HandlerBase {

	int errorCounter = 0;

public:
	XMLMemoryHandler();
	virtual ~XMLMemoryHandler();

//    void startElement(const XMLCh* const name, xercesc_3_1::AttributeList& attributes);
//    void characters(const XMLCh* const chars, const unsigned int length);
//    void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
//    void resetDocument();


    void warning(const SAXParseException& exc);
    void error(const SAXParseException& exc);
    void fatalError(const SAXParseException& exc);
};

#endif /* XMLMEMORYHANDLER_H_ */
