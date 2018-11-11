/*
 * AprsAsioThread.cpp
 *
 *  Created on: 20.10.2018
 *      Author: mateusz
 */

#include "AprsAsioThread.h"
#include "ReturnValues.h"
#include "ConnectionTimeoutEx.h"

#include "SOFTWARE_VERSION.h"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>

AprsAsioThread::AprsAsioThread(AprsThreadConfig & config, uint8_t timeoutInSeconds) :
																						conf(config),
																						outputPacketValid(false),
																						timeout(timeoutInSeconds)
{
	char buffer[256];

	if (config.StationSSID == 0)
		sprintf(buffer, "user %s pass %d vers %s %s filter p/%s \r\n", config.Call.c_str(), config.Passwd, SW_NAME, SW_VER, config.StationCall.c_str());
    else
		sprintf(buffer, "user %s pass %d vers %s %s filter p/%s-%d \r\n", config.Call.c_str(), config.Passwd, SW_NAME, SW_VER, config.StationCall.c_str(), config.StationSSID);

	this->loginString = std::string(buffer);

	this->connected = false;

}

void AprsAsioThread::workerThread() {

//	while (true) {
		this->ioservice.run();
//	}
}

void AprsAsioThread::connect() {
	// creating a copy of query class which is then used by the resolver to 'convert' a domain addres to ip
	boost::asio::ip::tcp::resolver::query q(this->conf.ServerURL, boost::lexical_cast<std::string>(this->conf.ServerPort));

	// resolver which will convert a domain name to ip
	boost::asio::ip::tcp::resolver resolver(this->ioservice);

	try {
		// resolving IP address
		this->resolverIterator = resolver.resolve(q);
	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		return;
	}

	// resolver returns an iterator, because the DNS server could return more than one A record
	boost::asio::ip::tcp::endpoint endpoint = *this->resolverIterator;

	// connecting
	this->tsocket.async_connect(endpoint, boost::bind(&AprsAsioThread::connectedCallback, this, _1));

	// checking if there is any thread already
	if (this->workersGroup.size() == 0)
		// creating a thread which will handle i/o
		this->workersGroup.create_thread(boost::bind(&AprsAsioThread::workerThread, this));

}

void AprsAsioThread::connectedCallback(const boost::system::error_code& ec) {

	if (ec) {
		std::cout << ec.message() << std::endl;
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

void AprsAsioThread::receive() {

	bool result = false;

	// locking a mutex which will be used to synchronize
	this->mutexRxSync.lock();

	// starting asynchronous read which will last until end of line will be received
	boost::asio::async_read_until(this->tsocket, this->in_buf, "\r\n", boost::bind(&AprsAsioThread::newLineCallback, this, _1));

	// waiting for receive
	result = this->mutexRxSync.timed_lock(boost::posix_time::seconds(this->timeout));

	if (result) {
		this->mutexRxSync.unlock();
		return;
	}
	else {
		this->mutexRxSync.unlock();
		throw ConnectionTimeoutEx();
	}

}

void AprsAsioThread::newLineCallback(const boost::system::error_code& ec) {
	if (ec) {
		return;
	}
	else {

		// temporary vector used for converting data received from the aprs-is server
		// to form which is acceptable by the APRS packet parser
		vector<char> tempVctForParsing;

		// const buffer with data from streambuf
		boost::asio::streambuf::const_buffers_type data = this->in_buf.data();

		// iterators to the begining and the end of streambuf data
		auto begin = boost::asio::buffers_begin(data);	// this iterator will be incremented
		auto end = boost::asio::buffers_end(data);


		do {
			tempVctForParsing.push_back(*begin);	// adding a current content of the iterator
			std::cout << *begin;
			begin++;	// incrementing an iterator to the next element
		} while (begin != end);	// until we reach the end of the streambuf

		// parsing the data to AprsPacket
		int status = AprsPacket::ParseAPRSISData(tempVctForParsing.data(), tempVctForParsing.size(), &this->outputPacket);

		this->in_buf.consume(this->in_buf.size());

		if (status == OK) {
			this->outputPacketValid = true;
		}
		else {
			this->outputPacketValid = false;
		}

		this->mutexRxSync.unlock();
		std::cout << "rx done" << std::endl;
		return;
	}
}

void AprsAsioThread::disconnect() {
	this->tsocket.close();
	this->ioservice.stop();
}

bool AprsAsioThread::isConnected() {
	return connected;
}

AprsPacket AprsAsioThread::getPacket() {
	return this->outputPacket;
}

bool AprsAsioThread::isPacketValid() {
	return this->outputPacketValid;
}

AprsAsioThread::~AprsAsioThread() {
	this->tsocket.close();
	this->ioservice.stop();
}

