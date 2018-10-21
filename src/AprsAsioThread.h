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
#include <boost/thread.hpp>

#include "AprsThreadConfig.h"

class AprsAsioThread {
	AprsThreadConfig & conf;

	// obiek io_service zajmujący się obsługą wejścia/wyjścia
	boost::asio::io_service ioservice;

	// obiekt klasy work jest niezbędny do sterowania kolejką zadań wykonywanych przez io_service
	// bez niego wywołanie metody ioservice.run(); zakończyło by się natychmiastowo bez względu na to
	// czy program chciałby wysłać/odebrać jakieś dane czy nie. Zapętlone, ciagłe wywołanie tej metody
	// zjadło by całe zasoby procesora
	boost::asio::io_service::work wrk {ioservice};
	boost::asio::ip::tcp::socket tsocket {ioservice};
	boost::asio::ip::tcp::resolver::iterator resolverIterator;

	boost::thread_group workersGroup;

	std::string loginString;

	void connectedCallback(const boost::system::error_code &ec);

	static void writeCallback(const boost::system::error_code &ec, std::size_t butesTxed);

	// metoda wywoływana w wątku (grupie wątków) służąca do obsługi io_service
	void workerThread();

public:
	void connect();

	AprsAsioThread(AprsThreadConfig & config);
	virtual ~AprsAsioThread();
};

#endif /* APRSASIOTHREAD_H_ */
