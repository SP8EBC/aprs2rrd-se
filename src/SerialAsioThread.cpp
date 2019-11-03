/*
 * SerialAsioThread.cpp
 *
 *  Created on: 30.10.2019
 *      Author: mateusz
 */

#include "SerialAsioThread.h"

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

SerialAsioThread::SerialAsioThread(const std::string& devname, unsigned int baud_rate,
        boost::asio::serial_port_base::parity opt_parity,
        boost::asio::serial_port_base::character_size opt_csize,
        boost::asio::serial_port_base::flow_control opt_flow,
        boost::asio::serial_port_base::stop_bits opt_stop) {

	this->csize = opt_csize;
	this->flow = opt_flow;
	this->parity = opt_parity;
	this->port = opt_parity;
	this->stop = opt_stop;

	this->port = devname;
	this->speed = baud_rate;

	this->handle = 0;

	this->io_service.reset(new boost::asio::io_service());
	this->sp.reset(new boost::asio::serial_port(*this->io_service));
}

SerialAsioThread::~SerialAsioThread() {
	// TODO Auto-generated destructor stub
}

bool SerialAsioThread::openPort() {

	this->sp->open(this->port);

	return false;

//	struct termios tty;
//	struct termios tty_old;
//	memset (&tty, 0, sizeof tty);
//
//	this->handle = open( this->port.c_str(), O_RDWR| O_NOCTTY | O_NONBLOCK );
//
//
//	/* Error Handling */
//	if ( tcgetattr ( handle, &tty ) != 0 ) {
//	   std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
//	}
//
//	/* Save old tty parameters */
//	tty_old = tty;
//
//	/* Set Baud Rate */
//
//	tty.c_iflag &= ~(IMAXBEL|IXOFF|INPCK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON|IGNPAR);
//	tty.c_iflag |= IGNBRK;
//
//	tty.c_oflag &= ~OPOST;
//	tty.c_oflag &= ~CRTSCTS;
//
//	tty.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL|ICANON|ISIG|IEXTEN|NOFLSH|TOSTOP|PENDIN);
//	tty.c_cflag &= ~(CSIZE|PARENB);
//	tty.c_cflag |= CS8|CREAD;
//	tty.c_cc[VMIN] = 1;		// bylo 80
//	tty.c_cc[VTIME] = 1;		// byo 3
//
//	cfsetospeed (&tty, (speed_t)B9600);
//	cfsetispeed (&tty, (speed_t)B9600);
//
//	/* Flush Port, then applies attributes */
//	tcflush( handle, TCIFLUSH );
//	if ( tcsetattr ( handle, TCSANOW, &tty ) != 0) {
//	   std::cout << "Error " << errno << " from tcsetattr" << std::endl;
//	}
//
//	std::cout << "Port serial skonfigurowany " << std::endl;
}

// http://docs.ros.org/indigo/api/ublox_gps/html/gps_8cpp_source.html
// http://docs.ros.org/indigo/api/ublox_gps/html/async__worker_8h_source.html
