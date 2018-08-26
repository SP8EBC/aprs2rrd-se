#ifndef APRSTHREAD_H
#define APRSTHREAD_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <exception>
#include <iostream>

#include "AprsThreadConfig.h"
#include "AprsPacket.h"

class AprsThread
{

friend class AprsPacket;	

private:
    int portno, n, nn;
    struct sockaddr_in *serv_addr;   // struct with server address and port
    struct hostent *server;         // wskaznik na strukture z wynikiem działania gethostbyname
	struct addrinfo *ServerAddressSA;
	
	char* logintext;
	std::string LoginText;
	int sockfd;
	char* BufferPointer;
	
	AprsThreadConfig* cLastConfig;
	
public:
	AprsThread();
	~AprsThread();
	
	void AprsISThread(bool in);
	void AprsISConnect(AprsThreadConfig* cConfiguration);
	void AprsISConnect(void);
	void AprsISDisconnect(void);

	int LastServerCommTime;
	short BufferOverflowFaults;

	char* Buffer;
	int BufferLen;
	
	void zero(void ) {
		this->BufferLen = 0;
        this->BufferPointer = this->Buffer; // ustawianie na poczatek bufora
		this->BufferLen = 1;
		*this->Buffer = '\n';
	}
};

class AprsConnectionFrozen: public exception {
	AprsThread* cWhat;
	int errn;
	public:
		AprsConnectionFrozen(AprsThread* cIn, int err) {
			cWhat = cIn;
			errn = err;
		}
		virtual const char* what() const throw() {
			return "--- Brak komunikacji z serwerem - Automatyczne wznawianie";
		}
		void Reconnect();
};

class AprsServerNotFound: public exception {
public:
	virtual const char* what() const throw() {
		return "--- Nie znaleziono podanego serwera";
	}
};

class ReadRetZero: public exception {
	AprsThread* cWhat;
	int errn;
public:
	virtual const char* what() const throw() {
		return "--- Błąd odczytu socketa";
	}
	ReadRetZero(AprsThread* cIn, int error) {
		cWhat = cIn;
		errn = error;
		cWhat->BufferLen = 0;
	}
	void error(void) {
		std::cout << "--- errno: " << errn << endl;
	}
};

class BufferOverflow: public exception {
	AprsThread* cWhat;
	int errn;
public:
	BufferOverflow(AprsThread* cIn, int error) {
		cWhat = cIn;
		errn = error;
	}
	virtual const char* what() const throw() {
		return "--- Przepełnienie bufora przy odbiorze danych";
	}
	void error(void) {
		std::cout << "--- errno: " << errn << endl;
		std::cout << "--- buffer: " << cWhat->Buffer << endl;
		if (cWhat->BufferOverflowFaults >= 5) {
			std::cout << "--- cWhat->BufferOverflowFaults >= 5... Resetuje";
			this->cWhat->AprsISDisconnect();
			this->cWhat->AprsISConnect();
			this->cWhat->BufferOverflowFaults = 0;
		}
	}
};

class AprsServerConnected: public exception {

};

#endif // APRSTHREAD_H
