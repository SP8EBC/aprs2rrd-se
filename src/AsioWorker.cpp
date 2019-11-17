/*
 * AsioWorker.cpp
 *
 *  Created on: 17.11.2019
 *      Author: mateusz
 */

#include "AsioWorker.h"

AsioWorker::AsioWorker() {
	// TODO Auto-generated constructor stub

}

boost::asio::io_service* AsioWorker::getIoServicePtr() {
	return &this->io_service;
}

boost::asio::io_service::work* AsioWorker::getWorkPtr() {
	return &this->work;
}

void AsioWorker::workerThread() {
	this->io_service.run();
}

void AsioWorker::startWorker() {
	// checking if there is any thread already
	if (this->workersGroup.size() == 0)
		// creating a thread which will handle i/o
		this->workersGroup.create_thread(boost::bind(&AsioWorker::workerThread, this));
}

AsioWorker::~AsioWorker() {
	// TODO Auto-generated destructor stub
}

