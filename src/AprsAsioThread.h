/*
 * AprsAsioThread.h
 *
 *  Created on: 20.10.2018
 *      Author: mateusz
 */

#ifndef APRSASIOTHREAD_H_
#define APRSASIOTHREAD_H_

#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "AprsThreadConfig.h"

class AprsAsioThread {
	AprsThreadConfig & conf;

	boost::asio::io_service ioservice;
	boost::asio::ip::tcp::socket tsocket {ioservice};
	boost::asio::ip::tcp::resolver::iterator resolverIterator;

	void workerThread();

public:
	void connect();

	AprsAsioThread(AprsThreadConfig & config);
	virtual ~AprsAsioThread();
};

#endif /* APRSASIOTHREAD_H_ */
