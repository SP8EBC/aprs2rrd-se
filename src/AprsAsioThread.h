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
#include <boost/asio/deadline_timer.hpp>
#include <boost/thread.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread.hpp>

#include <condition_variable>

#include "AprsThreadConfig.h"
#include "AprsPacket.h"

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

	// timer używany do obsługi timeotów przy nawiązywaniu połączenia i komunikacji z serwerem APRS-IS
	boost::asio::deadline_timer timer {ioservice};

	// we cannot use Mutex to synchronize two threads. As stack overflow claims mutexes shall be designalized
	// and signzlized in the same thread
	//
	// https://stackoverflow.com/questions/1105745/pthread-mutex-assertion-error
	// https://stackoverflow.com/questions/44635333/pthread-mutex-lock-assertion-fails
	// https://stackoverflow.com/questions/16907072/how-do-i-use-a-boost-condition-variable-to-wait-for-a-thread-to-complete-process
	//
	//boost::timed_mutex mutexRxSync;

	std::condition_variable syncCondition;

	std::mutex syncLock;

	boost::thread_group workersGroup;

	// string with login string, to be send to server after connection will been established
	std::string loginString;

	// input buffer for data received from server
    boost::asio::streambuf in_buf;

    // AprsPacket copy to store data received
    AprsPacket outputPacket;

    // flag which will confirm that data stored inside 'outputPacket' are valid
    bool outputPacketValid;

    bool connected;

    // timeout value in seconds
    uint8_t timeout;

	void connectedCallback(const boost::system::error_code &ec);

	void newLineCallback(const boost::system::error_code &ec);

	static void writeCallback(const boost::system::error_code &ec, std::size_t butesTxed);


	// metoda wywoływana w wątku (grupie wątków) służąca do obsługi io_service
	void workerThread();

public:
	void connect();
	void receive();
	void disconnect();

	bool isConnected();

	AprsPacket getPacket();
	bool isPacketValid();

	AprsAsioThread(AprsThreadConfig & config, uint8_t timeoutInSeconds);
	virtual ~AprsAsioThread();

	bool DebugOutput;
};

#endif /* APRSASIOTHREAD_H_ */
