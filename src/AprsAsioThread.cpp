/*
 * AprsAsioThread.cpp
 *
 *  Created on: 20.10.2018
 *      Author: mateusz
 */

#include "AprsAsioThread.h"

#include "SOFTWARE_VERSION.h"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <cstdio>

AprsAsioThread::AprsAsioThread(AprsThreadConfig & config) : conf(config) {
	char buffer[256];

	if (config.StationSSID == 0)
		sprintf(buffer, "user %s pass %d vers %s %s filter p/%s \r\n", config.Call.c_str(), config.Passwd, SW_NAME, SW_VER, config.StationCall.c_str());
    else
		sprintf(buffer, "user %s pass %d vers %s %s filter p/%s-%d \r\n", config.Call.c_str(), config.Passwd, SW_NAME, SW_VER, config.StationCall.c_str(), config.StationSSID);

	this->loginString = std::string(buffer);
}

void AprsAsioThread::workerThread() {
	std::cout << "watek" << std::endl;

	while (true) {
		this->ioservice.run();
	}
}

void AprsAsioThread::connect() {
	boost::asio::ip::tcp::resolver::query q(this->conf.ServerURL, boost::lexical_cast<std::string>(this->conf.ServerPort));

	// resolver do odpytywania o adres ip na podstawie nazwy DNSs
	boost::asio::ip::tcp::resolver resolver(this->ioservice);

	// wyciąganie adresu IP na podstawie
	this->resolverIterator = resolver.resolve(q);

	// boost::resolver prawdopodobnie potrafi zwracać więcej niż jeden adres IP, jeżeli serwer DNS
	// zwrócił więcej niż jeden rekrd A na zapytanie
	boost::asio::ip::tcp::endpoint endpoint = *this->resolverIterator;

	// zestawienie połączenia
	this->tsocket.async_connect(endpoint, boost::bind(&AprsAsioThread::connectedCallback, this, _1));

	// tworzenie wątków które będa odsługiwały we/wy
	this->workersGroup.create_thread(boost::bind(&AprsAsioThread::workerThread, this));

//	this->workersGroup.join_all();

	std::cout << "koniec" << std::endl;
}

void AprsAsioThread::connectedCallback(const boost::system::error_code& ec) {
	std::size_t bytes;

	if (ec) {
		return;
	}
	else {
		this->tsocket.async_send(boost::asio::buffer(this->loginString), &AprsAsioThread::writeCallback);
	}

	return;
}

void AprsAsioThread::writeCallback(const boost::system::error_code& ec,
		std::size_t butesTxed) {
	return;
}

AprsAsioThread::~AprsAsioThread() {
	// TODO Auto-generated destructor stub
}

