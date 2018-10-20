/*
 * AprsAsioThread.cpp
 *
 *  Created on: 20.10.2018
 *      Author: mateusz
 */

#include "AprsAsioThread.h"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

AprsAsioThread::AprsAsioThread(AprsThreadConfig & config) : conf(config) {

}

void AprsAsioThread::workerThread() {
	this->ioservice.run();
}

void AprsAsioThread::connect() {
	// resolver do odpytywania o adres ip na podstawie nazwy DNSs
	boost::asio::ip::tcp::resolver::query q(this->conf.ServerURL, boost::lexical_cast<std::string>(this->conf.ServerPort));

	boost::asio::ip::tcp::resolver resolver(this->ioservice);

	this->resolverIterator = resolver.resolve(q);

	boost::asio::ip::tcp::endpoint endpoint = *this->resolverIterator;

	this->tsocket.async_connect(endpoint, boost::bind(&AprsAsioThread::connectedCallback, this, _1));
}

void AprsAsioThread::connectedCallback(const boost::system::error_code& ec) {
	if (ec) {
		return;
	}
	else return;
}

AprsAsioThread::~AprsAsioThread() {
	// TODO Auto-generated destructor stub
}

