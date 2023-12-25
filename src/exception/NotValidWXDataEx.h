/*
 * NotValidWXDataEx.h
 *
 *  Created on: 26.08.2018
 *      Author: mateusz
 */

#ifndef NOTVALIDWXDATAEX_H_
#define NOTVALIDWXDATAEX_H_

#include <exception>

class NotValidWXData: public std::exception {
    virtual const char* what() const throw() {
		return "\n--- Niepoprawny pakiet pogodowy\n";
    }
};



#endif /* NOTVALIDWXDATAEX_H_ */
