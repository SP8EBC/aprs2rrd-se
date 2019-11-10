/*
 * Ax25Decoder.h
 *
 *  Created on: 10.11.2019
 *      Author: mateusz
 */

#ifndef AX25DECODER_H_
#define AX25DECODER_H_

#include "AprsPacket.h"

#include <cstdint>

class Ax25Decoder {
public:
	Ax25Decoder();
	virtual ~Ax25Decoder();

	static void ParseFromKissBuffer(uint8_t* data, uint16_t data_ln, AprsPacket& out);
};

#endif /* AX25DECODER_H_ */
