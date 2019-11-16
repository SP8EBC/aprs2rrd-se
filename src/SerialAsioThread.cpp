/*
 * SerialAsioThread.cpp
 *
 *  Created on: 30.10.2019
 *      Author: mateusz
 */

#include "SerialAsioThread.h"
#include "Ax25Decoder.h"

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <condition_variable>


SerialAsioThread::SerialAsioThread(const std::string& devname, unsigned int baud_rate) {

	this->csize = boost::asio::serial_port_base::character_size();
	this->flow = boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none);
	this->parity = boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none);
	this->stop = boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one);

	this->port = devname;
	this->speed = baud_rate;

	this->io_service.reset(new boost::asio::io_service());
	this->sp.reset(new boost::asio::serial_port(*this->io_service));
	this->work.reset(new boost::asio::io_service::work (*this->io_service));

	syncCondition.reset(new std::condition_variable());
	syncLock.reset(new std::mutex());

	::memset(this->buffer, 0x00, 512);

	this->state = SERIAL_CLOSED;
}

SerialAsioThread::~SerialAsioThread() {

	this->sp->cancel();
	this->sp->close();
	this->io_service->stop();
}


SerialAsioThread::SerialAsioThread(
		std::shared_ptr<std::condition_variable> syncCondition,
		std::shared_ptr<std::mutex> syncLock,
		std::string devname, unsigned int baud_rate)
					: syncCondition(syncCondition), syncLock(syncLock) {

	this->csize = boost::asio::serial_port_base::character_size();
	this->flow = boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none);
	this->parity = boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none);
	this->stop = boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one);

	this->io_service.reset(new boost::asio::io_service());
	this->sp.reset(new boost::asio::serial_port(*this->io_service));
	this->work.reset(new boost::asio::io_service::work (*this->io_service));

	::memset(this->buffer, 0x00, 512);

	this->port = devname;
	this->speed = baud_rate;

	this->state = SERIAL_CLOSED;

}

void SerialAsioThread::configure(const std::string& devname,
		unsigned int baud_rate,
		boost::asio::serial_port_base::parity opt_parity,
		boost::asio::serial_port_base::character_size opt_csize,
		boost::asio::serial_port_base::flow_control opt_flow,
		boost::asio::serial_port_base::stop_bits opt_stop) {

	this->csize = opt_csize;
	this->flow = opt_flow;
	this->parity = opt_parity;
	this->stop = opt_stop;

	this->port = devname;
	this->speed = baud_rate;

	this->state = SERIAL_CLOSED;
}

void SerialAsioThread::workerThread() {
	this->io_service->run();
}

std::size_t SerialAsioThread::asyncReadHandler(const boost::system::error_code& error, // Result of operation.
		std::size_t bytes_transferred) {

	uint8_t rx_byte = this->buffer[this->bufferIndex];

	switch (this->state) {
	case SERIAL_FRAME_RXED: {

		// clearing buffer after processing
		::memset(this->buffer, 0x00, 512);

		this->bufferIndex = 0;

		this->state = SERIAL_WAITING;

		return 0;

		//break;
	}
	case SERIAL_RXING_FRAME: {


		// Check if FEND has been reveived which in this state means that this is the end of the frame
		if (rx_byte == FEND) {

			this->endIndex = this->bufferIndex;

			return 0;
		}

		break;
	}
	case SERIAL_WAITING: {

		// Check if FEND has been reveived which in this state means that this is a begin of
		// new frame
		if (rx_byte == FEND) {
			this->state = SERIAL_RXING_FRAME;

			this->startIndex = this->bufferIndex;
		}

		break;

	}
	default:
		//return 0;
		break;
	}

	// calculating how many bytes has been transfered before this function call
	// 'bytes_transfered' parameter count bytes from the begin of whole transaction
	uint16_t index_increment = bytes_transferred - this->lastBytesTransfered;

	this->lastBytesTransfered = bytes_transferred;

	// increasing buffer Index
	this->bufferIndex += index_increment;

	return 1;

}

void SerialAsioThread::asyncReadCompletionHandler(
		const boost::system::error_code& error,
		std::size_t bytes_transferred) {

	bool decoding_status = false;

	if (error) {
		this->state = SERIAL_ERROR;
	}
	else {

		decoding_status = Ax25Decoder::ParseFromKissBuffer(this->buffer, this->bufferIndex, this->packet);

		this->state = SERIAL_FRAME_RXED;
	}

	this->packetValid = decoding_status;

	// notifing all that receiving is done
	this->syncCondition->notify_all();

	return;
}

bool SerialAsioThread::openPort() {

	this->sp->open(this->port);

	// checking if there is any thread already
	if (this->workersGroup.size() == 0)
		// creating a thread which will handle i/o
		this->workersGroup.create_thread(boost::bind(&SerialAsioThread::workerThread, this));

	this->state = SERIAL_IDLE;

	return true;

}

void SerialAsioThread::receive(bool wait) {

	auto readHandlerPtr = boost::bind(&SerialAsioThread::asyncReadHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
	auto completionHandlerPtr = boost::bind(&SerialAsioThread::asyncReadCompletionHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);


	boost::asio::async_read(*this->sp, boost::asio::buffer(buffer, 512), readHandlerPtr, completionHandlerPtr);

	this->state = SERIAL_WAITING;

	this->packetValid = false;

	if (wait)
		this->waitForRx();

}

bool SerialAsioThread::waitForRx() {
	std::unique_lock<std::mutex> lock(*this->syncLock);

	auto result = this->syncCondition->wait_for(lock, std::chrono::seconds(99));

	if (result == std::cv_status::timeout) {
		lock.unlock();
		return false;
	}
	else {
		lock.unlock();
		return true;
	}

}

bool SerialAsioThread::isPacketValid() {
	return this->packetValid;
}

AprsPacket SerialAsioThread::getPacket() {
	return this->packet;
}
// http://docs.ros.org/indigo/api/ublox_gps/html/gps_8cpp_source.html
// http://docs.ros.org/indigo/api/ublox_gps/html/async__worker_8h_source.html
