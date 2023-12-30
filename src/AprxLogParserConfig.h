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
    int maximumPacketAge;

    /**
     * @brief Set to true to load all weather packets from given timestamp, not just the last one. 
     *        Invoking more than onve may (and probably will) lead to corrupting database by placing
     *        many records with the same data (including the same timestamp)
    */
    bool batchLoad;

    long long batchLoadFrom;

    long long batchLoadTo;

    AprxLogParserConfig() {
        enabled = false;
        logTimeInLocal = true;
        sourceSsid = 0;
        maximumPacketAge = 0;
        batchLoad = false;
        batchLoadFrom = 0;
        batchLoadTo = 0;
    }
};

#endif
