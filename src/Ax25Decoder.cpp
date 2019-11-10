/*
 * Ax25Decoder.cpp
 *
 *  Created on: 10.11.2019
 *      Author: mateusz
 */

#include "Ax25Decoder.h"
#include <vector>

#define FEND	(uint8_t)0xC0
#define FESC	(uint8_t)0xDB
#define TFEND	(uint8_t)0xDC
#define TFESC	(uint8_t)0xDD

Ax25Decoder::Ax25Decoder() {
	// TODO Auto-generated constructor stub

}

Ax25Decoder::~Ax25Decoder() {
	// TODO Auto-generated destructor stub
}

void Ax25Decoder::ParseFromKissBuffer(uint8_t* data, uint16_t data_ln,
		AprsPacket& out) {

	if (data_ln < 12)
		return;

	std::vector<uint8_t> buffer(data, data + data_ln);

	uint16_t idx = 0;

	uint8_t c = 0;
	uint8_t call_buffer[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

	bool last = true;

	// skip FEND and Kiss Command
	if (data[0] == FEND && data[1] == 0x00) {
		idx = 2;
	}

	// parsing DST address
	for (int i = 0; i < 6; i++) {
		c = ((buffer[idx++]) >> 1);

		// convert spaces to nulls if callsign has less than 6 charcters
		if (c == ' ')
			c = 0x0;

		call_buffer[i] = c;
	}

	// converting to a string
	std::string dst_call((const char*)call_buffer, 8);
	out.DestAddr = dst_call;

	// parsing DST ssid
	c = (buffer[idx++] >> 8) & 0xF;
	out.DstSSID = c;

	// parsing SRC callsign
	for (int i = 0; i < 6; i++) {
		c = ((buffer[idx++]) >> 1);

		// convert spaces to nulls if callsign has less than 6 charcters
		if (c == ' ')
			c = 0x0;

		call_buffer[i] = c;
	}

	// converting to a string
	std::string src_call((const char*)call_buffer, 8);
	out.SrcAddr = src_call;

	// parsing DST ssid
	c = buffer[idx++];
	out.SrcSSID = (c >> 8) & 0xF;

	// parsing digipeaters in path
	do {
		// the AX25 hasn't got a field with a length of the path and it the same way thr path could have any
		// length it is required by the sender. It could have only destination and source (which are mandatory)
		// or long path with many WIDE or digipeaters callsigns.

		// checking if source is the last element or not
		if ((c & 0x01) == 0x01)
			continue;

	} while (last != true);

}
