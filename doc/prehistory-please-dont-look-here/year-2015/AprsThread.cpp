#include "AprsThread.h"
#include "AprsThreadConfig.h"
#include "SOFTWARE_VERSION.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>

using namespace std;

AprsThread::AprsThread()
{
	this->Buffer = new char[1024];
	this->BufferPointer = this->Buffer;
	this->cLastConfig = NULL;
	this->BufferOverflowFaults = 0;
}

AprsThread::~AprsThread()
{
}

void AprsThread::AprsISThread(bool in) {
		char receive_temp;
		int ret, ii;
		this->BufferLen = 0;
        this->BufferPointer = this->Buffer; // ustawianie na poczatek bufora
		memset(this->Buffer, 0x00, 1024);
		ii = 0;
		
        do {
            // Waiting for data
    //        currtimeint = (int)time(NULL);
            ret = read(this->sockfd,&receive_temp,1);
            *BufferPointer++ = receive_temp;    // dodawanie znaku do bufora
			int erret = errno;
            if (ret <= -1) {
                /*  Check when last data from APRS server was reveiced. Normally each server sends
                some ping messages in 30 sec interval. If connection become frozen this routine
                will reset it*/
                throw AprsConnectionFrozen(this, erret);
            }
			this->BufferLen++;
			ii++;
			if (ii >= 1022) {
				this->BufferOverflowFaults++;
				throw BufferOverflow(this, erret);
			}
        } while (receive_temp != '\n');
        *BufferPointer++ = '\0';        // adding NULL-terminator at the end of string
        cout << Buffer;
        this->LastServerCommTime = (int)time(NULL);     // updating time
}

void AprsThread::AprsISConnect(AprsThreadConfig* cConfiguration) {
	this->logintext = new char [96];
    struct timeval timeouts;
	int return_temp;
    char receive_temp;
	time_t temp;
	stringstream porttemp;
	string porttempstring;
	struct addrinfo hints;
	
	this->cLastConfig = cConfiguration;
	this->server = 0x00;
	porttemp << cConfiguration->ServerPort;
	porttempstring = porttemp.str();
	
	hints.ai_family = AF_INET;
	hints.ai_protocol = 6;	// tcp
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	
	if (cConfiguration->StationSSID == 0)
		sprintf(this->logintext, "user %s pass %d vers %s %s filter p/%s \r\n", cConfiguration->Call.c_str(), cConfiguration->Passwd, SW_NAME, SW_VER, cConfiguration->StationCall.c_str());
    else
		sprintf(this->logintext, "user %s pass %d vers %s %s filter p/%s-%d \r\n", cConfiguration->Call.c_str(), cConfiguration->Passwd, SW_NAME, SW_VER, cConfiguration->StationCall.c_str(), cConfiguration->StationSSID);		
	this->LoginText = this->logintext;
	delete this->logintext;
	
	return_temp = getaddrinfo(cConfiguration->ServerURL.c_str(), porttempstring.c_str(), &hints, &this->ServerAddressSA);
    if (return_temp != 0 ) {
      //  printf("--- Nie znaleziono serwera APRS-IS");
		throw AprsServerNotFound();
    }

    timeouts.tv_sec = 39;      // timeout value in second
    timeouts.tv_usec = 0;
 
	this->sockfd = socket(this->ServerAddressSA->ai_family, this->ServerAddressSA->ai_socktype, this->ServerAddressSA->ai_protocol);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeouts, sizeof(timeouts));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeouts, sizeof(timeouts));   // setting timeout val
	do {
		cout << "--- Łączenie z serwerem...." << endl;
        return_temp = connect(this->sockfd,this->ServerAddressSA->ai_addr,this->ServerAddressSA->ai_addrlen);    // trying to connect
		if (return_temp < 0) {
            cout << "--- Nawiązywanie połaczenia zakończone niepowodzeniem" << endl;
            sleep(10);      // waing for short while before next attempt
        }
    } while (return_temp != 0);
    cout << "--- Nawiązano połączenie" <<endl;
    do {
        // waiting for hello message from APRS-IS server
        nn = read(this->sockfd,&receive_temp,1);
        cout << receive_temp;
    } while (receive_temp != '\n');
    nn = write(this->sockfd,this->LoginText.c_str(),this->LoginText.size());  // sending login credentials

	temp = time(NULL);
    this->LastServerCommTime = (int)temp;
	
	throw AprsServerConnected();
}

void AprsThread::AprsISConnect(void) {
	if (this->cLastConfig != NULL)
		this->AprsISConnect(this->cLastConfig);
} 

void AprsThread::AprsISDisconnect(void) {
	close(this->sockfd);
	memset(&this->serv_addr, 0x00, sizeof(this->serv_addr));
	memset(&this->server, 0x00, sizeof(this->server));
}

void AprsConnectionFrozen::Reconnect() {
	this->cWhat->AprsISDisconnect();
	this->cWhat->AprsISConnect();
}
