/*
 * AsioWorker.h
 *
 *  Created on: 17.11.2019
 *      Author: mateusz
 */

#ifndef ASIOWORKER_H_
#define ASIOWORKER_H_

#include <memory>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

class AsioWorker {
	boost::asio::io_service io_service;

	boost::asio::io_service::work work{io_service};

	boost::thread_group workersGroup;

	void workerThread();

public:

	boost::asio::io_service* getIoServicePtr();

	boost::asio::io_service::work* getWorkPtr();

	void startWorker();

	AsioWorker();
	virtual ~AsioWorker();
};

#endif /* ASIOWORKER_H_ */
