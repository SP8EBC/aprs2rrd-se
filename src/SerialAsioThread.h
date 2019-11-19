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

#define SERIAL_BUFER_LN 512

enum SerialAsioThreadState {
	SERIAL_NOT_CONFIGURED,
	SERIAL_CLOSED,
	SERIAL_IDLE,
	SERIAL_WAITING,
	SERIAL_RXING_FRAME,
	SERIAL_FRAME_RXED,
	SERIAL_FRAME_DECODED,
	SERIAL_ERROR
};

class SerialAsioThread {

	std::unique_ptr<boost::asio::serial_port> sp;
	std::unique_ptr<boost::asio::io_service> io_service;

	std::unique_ptr<boost::asio::io_service::work> work;

    boost::asio::serial_port_base::parity parity;
    boost::asio::serial_port_base::character_size csize;
    boost::asio::serial_port_base::flow_control flow;
    boost::asio::serial_port_base::stop_bits stop;

    // A string path to serial port (like /dev/ttyUSB0)
    std::string port;

    // Serial port speed in bauds
    uint32_t speed;

    // Content of AX25 frame received from the TNC after decoding
    AprsPacket packet;

    // A group of threads used to service I/O in background
	boost::thread_group workersGroup;

	// Shared pointers used for synchronizing RS232 transmission with the rest of app
	std::shared_ptr<std::condition_variable> syncCondition;

	// a mutex is used by an unique lock which then is used as a object the condition variable
	// locks the thead on (wait_for method)
	std::shared_ptr<std::mutex> syncLock;

	// buffer for data incoming from RS232 port
	uint8_t buffer[SERIAL_BUFER_LN];

	// index used to move across the buffer during transmission
	uint16_t bufferIndex = 0;

	uint16_t startIndex = 0;

	// an index in buffer when received KISS frame ends
	uint16_t endIndex = 0;

	SerialAsioThreadState state;

	std::size_t lastBytesTransfered = 0;

	bool packetValid = false;

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

	// This constructor is used if external synchronization needs to be used
	// which means that there are more than one communication threads are
	// running within an app (like few KISS TNCs or KISS TNC + APRS-IS).
	// In such case whole processing loop needs to be rather synchronized
	// on single, global entities to provide universal way of informing
	// that new data from some soruce is avaliable
	SerialAsioThread(std::shared_ptr<std::condition_variable> syncCondition,
	std::shared_ptr<std::mutex> syncLock,
	std::string devname, unsigned int baud_rate);

	// This is constructor which can be used in 'standalone' mode when
	// KISS TNC will be used on it's own. The constructor will create
	// local (per class instance) condition_variable and mutex
	// to wait for reception locally
	SerialAsioThread(const std::string& devname, unsigned int baud_rate);
	virtual ~SerialAsioThread();

	// This method shall be used if an user want non-standard parameters
	void configure(const std::string& devname, unsigned int baud_rate,
	        boost::asio::serial_port_base::parity opt_parity,
	        boost::asio::serial_port_base::character_size opt_csize,
	        boost::asio::serial_port_base::flow_control opt_flow,
	        boost::asio::serial_port_base::stop_bits opt_stop);

	bool openPort();

	void receive(bool wait);

	bool waitForRx();

	bool isPacketValid();

	AprsPacket getPacket();

	bool debug = false;
};

#endif /* SERIALASIOTHREAD_H_ */
