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
#include <iostream>


SerialAsioThread::SerialAsioThread(const std::string& devname, unsigned int baud_rate) {

	this->workerThreadObjPtr = nullptr;

	this->csize = boost::asio::serial_port_base::character_size();
	this->flow = boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none);
	this->parity = boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none);
	this->stop = boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one);

	this->port = devname;
	this->speed = baud_rate;
//
//	this->io_service.reset(new boost::asio::io_service());
//	this->sp.reset(new boost::asio::serial_port(*this->io_service));
//	this->work.reset(new boost::asio::io_service::work (*this->io_service));

	syncCondition.reset(new std::condition_variable());
	syncLock.reset(new std::mutex());

	::memset(this->buffer, 0x00, 512);

	this->state = SERIAL_CLOSED;
}

SerialAsioThread::~SerialAsioThread() {

	this->sp->cancel();
	this->sp->close();
	this->io_service->stop();
	//this->workersGroup.join_all();
}


SerialAsioThread::SerialAsioThread(
		std::shared_ptr<std::condition_variable> syncCondition,
		std::shared_ptr<std::mutex> syncLock,
		std::string devname, unsigned int baud_rate)
					: syncCondition(syncCondition), syncLock(syncLock) {

	this->workerThreadObjPtr = nullptr;

	this->csize = boost::asio::serial_port_base::character_size();
	this->flow = boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none);
	this->parity = boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none);
	this->stop = boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one);

	::memset(this->buffer, 0x00, 512);

	this->port = devname;
	this->speed = baud_rate;

	this->state = SERIAL_CLOSED;

}

void SerialAsioThread::closePort() {
	this->sp->cancel();
	this->sp->close();
	this->io_service->stop();
	this->io_service->reset();
	this->work.reset();
	delete this->workerThreadObjPtr;
	this->io_service.reset();
	this->sp.reset();
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

	if (error) {
		std::cout << "--- asyncReadHandler: error " << error.message() << std::endl;
		this->state = SERIAL_ERROR;
	}

	boost::asio::deadline_timer t(*this->io_service, boost::posix_time::milliseconds(1));

	uint8_t rx_byte = this->buffer[this->bufferIndex];

	std::cout << "-- asyncReadHandler: bytes_transferred: " << (int32_t) bytes_transferred << std::endl;

	t.wait();

	switch (this->state) {
	case SERIAL_ERROR:
	case SERIAL_FRAME_DECODED: {

		// clearing buffer after processing
		::memset(this->buffer, 0x00, SERIAL_BUFER_LN);

		this->bufferIndex = 0;

		this->state = SERIAL_WAITING;

		return 0;

	}
	case SERIAL_RXING_FRAME: {


		// Check if FEND has been reveived which in this state means that this is the end of the frame
		if (rx_byte == FEND) {

			this->endIndex = this->bufferIndex;

			this->state = SERIAL_FRAME_RXED;

			return 0;
		}

		break;
	}
	case SERIAL_WAITING: {

		// Check if FEND has been reveived which in this state means that this is a begin of
		// new frame
		if (rx_byte == FEND) {
			if (this->debug)
				std::cout << "--- asyncReadHandler: Begin of a frame has been sent by KISS modem" << std::endl;

			this->state = SERIAL_RXING_FRAME;

			this->startIndex = this->bufferIndex;
		}

		break;

	}
	default:
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
		std::cout << "--- asyncReadCompletionHandler: error " << error.message() << std::endl;
		this->state = SERIAL_ERROR;
	}
	else if (this->state == SERIAL_FRAME_RXED) {

		decoding_status = Ax25Decoder::ParseFromKissBuffer(this->buffer, this->bufferIndex, this->packet);

		if (this->debug && decoding_status) {
			std::cout << "--- asyncReadCompletionHandler: A packet has been received from KISS modem" << std::endl;
			this->packet.PrintPacketData();
		}

		this->packetValid = decoding_status;

		// notifing all that receiving is done
		this->syncCondition->notify_all();

		this->state = SERIAL_FRAME_DECODED;
	}
	else if (this->state == SERIAL_ERROR){
		if (this->debug)
			std::cout << "--- asyncReadCompletionHandler: An error happened during transmission through serial port." << std::endl;

		this->packetValid = false;

		// notifing all that receiving is done
		this->syncCondition->notify_all();
	}
	else {
		;
	}

	return;
}

bool SerialAsioThread::openPort() {

	this->io_service.reset(new boost::asio::io_service());
	this->sp.reset(new boost::asio::serial_port(*this->io_service));
	this->work.reset(new boost::asio::io_service::work (*this->io_service));

	this->sp->open(this->port);

	this->workerThreadObjPtr = new boost::thread(boost::bind(&SerialAsioThread::workerThread, this));

	this->state = SERIAL_IDLE;

	return true;

}

void SerialAsioThread::receive(bool wait) {

	if (this->state == SERIAL_CLOSED || this->state == SERIAL_NOT_CONFIGURED)
		return;

	if (this->state != SERIAL_IDLE) {
		if (this->debug)
			std::cout << "--- Canceling pending r/w operations. " << std::endl;

		//this->sp->cancel();
		closePort();
		openPort();

		// clearing buffer after processing
		::memset(this->buffer, 0x00, SERIAL_BUFER_LN);

		this->bufferIndex = 0;

		boost::asio::deadline_timer t(*this->io_service, boost::posix_time::milliseconds(1));

		t.wait();
	}

	auto readHandlerPtr = boost::bind(&SerialAsioThread::asyncReadHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
	auto completionHandlerPtr = boost::bind(&SerialAsioThread::asyncReadCompletionHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);


	boost::asio::async_read(*this->sp, boost::asio::buffer(buffer, 512), readHandlerPtr, completionHandlerPtr);

	this->state = SERIAL_WAITING;

	this->packetValid = false;

	if (this->debug)
		std::cout << "--- Receiving data from a serial KISS modem initiated. " << std::endl;

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
