/*
 * SerialAsioThread.h
 *
 *  Created on: 30.10.2019
 *      Author: mateusz
 */

#ifndef SERIALASIOTHREAD_H_
#define SERIALASIOTHREAD_H_

#include <cstdint>
#include <memory>
#include <condition_variable>

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <boost/asio/serial_port_base.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/thread.hpp>
#include <boost/asio/buffer.hpp>

#include "AprsPacket.h"

#define FEND	(uint8_t)0xC0
#define FESC	(uint8_t)0xDB
#define TFEND	(uint8_t)0xDC
#define TFESC	(uint8_t)0xDD

enum SerialAsioThreadState {
	SERIAL_WAITING,
	SERIAL_RXING_FRAME,
	SERIAL_FRAME_RXED
};

class SerialAsioThread {

	std::unique_ptr<boost::asio::serial_port> sp;
	std::unique_ptr<boost::asio::io_service> io_service;

	std::unique_ptr<boost::asio::io_service::work> work;

    boost::asio::serial_port_base::parity parity;
    boost::asio::serial_port_base::character_size csize;
    boost::asio::serial_port_base::flow_control flow;
    boost::asio::serial_port_base::stop_bits stop;

    uint32_t handle;

    std::string port;
    uint32_t speed;

    // Content of AX25 frame received from the TNC after decoding
    AprsPacket packet;

    // A group of threads used to service I/O in background
	boost::thread_group workersGroup;

    boost::asio::streambuf in_buf;

	std::condition_variable syncCondition;

	std::mutex syncLock;

	// buffer for data incoming from RS232 port
	uint8_t buffer[512];

	// index used to move across the buffer during transmission
	uint16_t bufferIndex = 0;

	uint16_t startIndex = 0;

	uint16_t endIndex = 0;

	SerialAsioThreadState state;

	std::size_t lastBytesTransfered = 0;

	void workerThread();

	std::size_t asyncReadHandler(
	  const boost::system::error_code& error, // Result of operation.

	  std::size_t bytes_transferred           // Number of bytes transfered (received)
	  	  	  	  	  	  	  	  	  	  	  // so far, starting from the call to read function
	);

	void asyncReadCompletionHandler(
	  const boost::system::error_code& error, // Result of operation.

	  std::size_t bytes_transferred           // Number of bytes copied into the
	                                          // buffers. If an error occurred,
	                                          // this will be the  number of
	                                          // bytes successfully transferred
	                                          // prior to the error.
	);


public:
	SerialAsioThread(const std::string& devname, unsigned int baud_rate,
	        boost::asio::serial_port_base::parity opt_parity,
	        boost::asio::serial_port_base::character_size opt_csize,
	        boost::asio::serial_port_base::flow_control opt_flow,
	        boost::asio::serial_port_base::stop_bits opt_stop);
	virtual ~SerialAsioThread();

	bool openPort();

	void triggerRx();

	bool waitForRx();
};

#endif /* SERIALASIOTHREAD_H_ */
