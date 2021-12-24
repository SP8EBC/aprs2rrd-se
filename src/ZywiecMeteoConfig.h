#ifndef _ZYWIEC_CONFIG
#define _ZYWIEC_CONFIG

#include <cstdint>
#include <string>

class ZywiecMeteoConfig{ 

public:
	std::string baseUrl;
	
	uint32_t stationId;

	bool secondaryTemperature;

	bool enable;
};

#endif
