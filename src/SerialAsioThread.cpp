/*
 * SerialAsioThread.cpp
 *
 *  Created on: 30.10.2019
 *      Author: mateusz
 */

#include "SerialAsioThread.h"

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


SerialAsioThread::SerialAsioThread(const std::string& devname, unsigned int baud_rate,
        boost::asio::serial_port_base::parity opt_parity,
        boost::asio::serial_port_base::character_size opt_csize,
        boost::asio::serial_port_base::flow_control opt_flow,
        boost::asio::serial_port_base::stop_bits opt_stop) {

	this->csize = opt_csize;
	this->flow = opt_flow;
	this->parity = opt_parity;
//	this->port = opt_parity;
	this->stop = opt_stop;

	this->port = devname;
	this->speed = baud_rate;

	this->handle = 0;

	this->io_service.reset(new boost::asio::io_service());
	this->sp.reset(new boost::asio::serial_port(*this->io_service));
	::memset(this->buffer, 0x00, 512);
}

SerialAsioThread::~SerialAsioThread() {
	// TODO Auto-generated destructor stub
}

void SerialAsioThread::workerThread() {
	this->io_service->run();
}

std::size_t SerialAsioThread::asyncReadHandler(const boost::system::error_code& error, // Result of operation.
		std::size_t bytes_transferred) {

	return 1;

}

void SerialAsioThread::asyncReadCompletionHandler(
		const boost::system::error_code& error, // Result of operation.
		std::size_t bytes_transferred) {

	return;
}

bool SerialAsioThread::openPort() {

	this->sp->open(this->port);

	// checking if there is any thread already
	if (this->workersGroup.size() == 0)
		// creating a thread which will handle i/o
		this->workersGroup.create_thread(boost::bind(&SerialAsioThread::workerThread, this));

	auto readHandlerPtr = boost::bind(&SerialAsioThread::asyncReadHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
	auto completionHandlerPtr = boost::bind(&SerialAsioThread::asyncReadCompletionHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);

	boost::asio::async_read(*this->sp, boost::asio::buffer(buffer, 512), readHandlerPtr, completionHandlerPtr);

	return false;

}

void SerialAsioThread::waitForRx() {
	std::unique_lock<std::mutex> lock(this->syncLock);

	auto result = this->syncCondition.wait_for(lock, std::chrono::seconds(666));

}
// http://docs.ros.org/indigo/api/ublox_gps/html/gps_8cpp_source.html
// http://docs.ros.org/indigo/api/ublox_gps/html/async__worker_8h_source.html
