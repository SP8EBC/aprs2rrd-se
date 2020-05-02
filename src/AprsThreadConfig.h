#ifndef APRSTHREADCONFIG_H
#define APRSTHREADCONFIG_H

#include <string>
using namespace std;

class AprsThreadConfig
{
public:
	AprsThreadConfig();
	~AprsThreadConfig();

	bool enable;

	string ServerURL;
	unsigned ServerPort;
	string Call;
	unsigned Passwd;
	
	string StationCall;
	unsigned StationSSID;
	
	string SecondaryCall;
	unsigned SecondarySSID;

	bool RetryServerLookup;

};

#endif // APRSTHREADCONFIG_H
