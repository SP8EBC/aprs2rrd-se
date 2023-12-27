/*
 * AprxLogParserConfig.h
 *
 *  Created on: Aug 21, 2023
 *      Author: mateusz
 */

#ifndef APRXLOGPARSERCONFIG_H_
#define APRXLOGPARSERCONFIG_H_

#include <string>
#include <cstdint>

class AprxLogParserConfig {

public:
    bool enabled;
    
    std::string logFile;

    bool logTimeInLocal;

    std::string sourceCallsign;

    uint8_t sourceSsid;

    /**
     * @brief How old in minutes rf-log entry can be to be used as valid 
     * 
    */
    uint8_t maximumPacketAge;

    /**
     * @brief Set to true to load all weather packets from given timestamp, not just the last one. 
     *        Invoking more than onve may (and probably will) lead to corrupting database by placing
     *        many records with the same data (including the same timestamp)
    */
    bool batchLoad;


    uint64_t batchLoadFrom;

    uint64_t batchLoadTo;
};

#endif
