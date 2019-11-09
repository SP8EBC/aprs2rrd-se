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

class SerialAsioThread {

	std::unique_ptr<boost::asio::serial_port> sp;
	std::unique_ptr<boost::asio::io_service> io_service;

	//boost::asio::serial_port sp;

    boost::asio::serial_port_base::parity parity;
    boost::asio::serial_port_base::character_size csize;
    boost::asio::serial_port_base::flow_control flow;
    boost::asio::serial_port_base::stop_bits stop;

    uint32_t handle;

    std::string port;
    uint32_t speed;

	boost::thread_group workersGroup;

    boost::asio::streambuf in_buf;

	std::condition_variable syncCondition;

	std::mutex syncLock;

	void workerThread();

	uint8_t buffer[512];

	//boost::asio::mutable_buffer boostBuffer = boost::asio::buffer(buffer);

	std::size_t asyncReadHandler(
	  const boost::system::error_code& error, // Result of operation.

	  std::size_t bytes_transferred           // Number of bytes copied into the
	                                          // buffers. If an error occurred,
	                                          // this will be the  number of
	                                          // bytes successfully transferred
	                                          // prior to the error.
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

	void waitForRx();
};

#endif /* SERIALASIOTHREAD_H_ */
