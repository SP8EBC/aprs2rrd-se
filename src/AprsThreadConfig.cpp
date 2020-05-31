#include "AprsThreadConfig.h"

AprsThreadConfig::AprsThreadConfig()
{
	SecondaryCall = std::string();
	SecondarySSID = 0;
	ServerPort = 14580;
	enable = false;
	RetryServerLookup = false;
	StationSSID = 0;
	Passwd = 0;
}

AprsThreadConfig::~AprsThreadConfig()
{
}

std::string AprsThreadConfig::getStationCallStr() {
	std::string out;

	if (this->StationSSID != 0) {
		out = this->StationCall;
		out += std::to_string(this->StationSSID);
	}
	else {
		out = this->StationCall;
	}

	return out;
}

std::string AprsThreadConfig::getSecondaryCallStr() {
	std::string out;

	if (this->SecondarySSID != 0) {
		out = this->SecondaryCall;
		out += std::to_string(this->SecondarySSID);
	}
	else {
		out = this->StationCall;
	}

	return out;
}
