#include "AprsPacket.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "ReturnValues.h"

using namespace std;

AprsPacket::AprsPacket() {
    this->SrcSSID = 0;
    this->DstSSID = 0;
    memset(this->Data,0x00,sizeof(this->Data));

    this->ToISOriginator.Call = "UNINITIALIZED";
}

bool AprsPacket::SeparateCallSsid(const std::string& input, std::string& call,
		uint8_t& ssid) {

	// copying the input string to editable local object
	std::string in = input;

	bool out = false;
	bool hasSsid = (in.find("-") != std::string::npos) ? true : false;

	// removing any '*' from input string - we don't need to preserve H-bits from packet path
	boost::erase_all(in, "*");

	if (hasSsid) {
		std::vector<std::string> callAndSsid;

		boost::split(callAndSsid, in, boost::is_any_of("-"));

		// check if Ssid has a valid leght, not greater than 6 characters
		if (callAndSsid.at(0).size() > 6)
			return false;

		call = callAndSsid.at(0);

		ssid = boost::lexical_cast<int>(callAndSsid.at(1));
	}
	else {
		call = in;
		ssid = 0;
	}

	// validating SSID value
	if (ssid > 15)
		out = false;

	// validating lenght of string
	if (input.size() > 6)
		out = false;

	out = true;
	return out;
}

bool AprsPacket::SeparateCallSsid(const std::string& input, char (&call)[7],
		uint8_t& ssid) {

	std::string intermediate;

	AprsPacket::SeparateCallSsid(input, intermediate, ssid);

	if (intermediate.size() > 6)
		return false;

	std::copy(intermediate.begin(), intermediate.end(), call);

	return true;
}

AprsPacket::~AprsPacket()
{
}

void AprsPacket::PrintPacketData() {
    cout << "-----------------------------------------------" << endl;
    cout << "-- Przetworzone dane stacji: " << this->SrcAddr << "-" << this->SrcSSID << endl;
    cout << "-- Id urządzenia: " << this->DestAddr << endl;
    cout << "-- Długość Ścieżki: " << this->Path.size() << endl;
//    for (int i = 0; i<= this->Path.size() && i <= 5; i++)
//        cout << "---- Element Numer: " << i << " = " << this->Path[i].Call << "-" << this->Path[i].SSID << endl;
    cout << "-- Typ wysyłającego do APRS-IS: " << this->qOrigin << endl;
    cout << "-- Wysyłający do APRS-IS: " << this->ToISOriginator.Call << endl;
    cout << "-- Dane: " << this->Data << endl;
    cout << "----------------------------------------------- \r\n";
}


int AprsPacket::ParseAPRSISData(char* tInputBuffer, int buff_len, AprsPacket* cTarget) {

	// simple regex to match most of callsign systems
	boost::regex callsignPattern("^[A-Z1-9]{3}[A-Z]{1,3}", boost::regex::icase);

	// q-construct regex
	boost::regex qc("q[A-Z]{2}");

	// frame payload
	std::string payload = "";

	bool parseOrginator = false;

	// checkig if input buffer is valid
	if (tInputBuffer == nullptr)
		return NOT_VALID_APRS_PACKET;

	// check if input buffer is valid
    if (*tInputBuffer == '#' || *tInputBuffer == 0x00 || buff_len > 1000 || buff_len < 5)
		return NOT_VALID_APRS_PACKET;

    // converting to string
	std::string input(tInputBuffer, buff_len);

	// vector which will hold source call separated from the rest of frame
	std::vector<std::string> sepratedBySource;

	// vector to keep separated path elements
	std::vector<std::string> pathElements;

	// spltting input frame basing on '>' character
	boost::split(sepratedBySource, input, boost::is_any_of(">"));

	// there must be at least one '>' character which will separate soruce from
	// the rest of frame
	if (sepratedBySource.size() < 2)
		return NOT_VALID_APRS_PACKET;

	// separating a callsign from the SSID
	AprsPacket::SeparateCallSsid(sepratedBySource.at(0), cTarget->SrcAddr, cTarget->SrcSSID);

	// checking if the callsign match with regex
	if (!boost::regex_match(cTarget->SrcAddr, callsignPattern))
		return NOT_VALID_APRS_PACKET;

	// the 'sepratedBySource' vector may consist more than 2 elements if received frame consist
	// status message which is identified by '>' character before it. All in all the APRS2RRD
	// is focused to work on wx frames, so we can just ignore everything after second element.
	std::string path = sepratedBySource.at(1);

	// splitting path elements and the frame payload
	boost::split(pathElements, path, boost::is_any_of(",:"));

	// checking if after splitting there are enough elements to thread this as valid frame
	if (pathElements.size() < 2)
		return NOT_VALID_APRS_PACKET;

	// checking if source identifier is valid
	if (pathElements.at(0).size() > 6)
		return NOT_VALID_APRS_PACKET;

	AprsPacket::SeparateCallSsid(pathElements.at(0), cTarget->DestAddr, cTarget->DstSSID);

	// removing the first element which which consist target address
	pathElements.erase(pathElements.begin());

	// splitting
	for (std::string e : pathElements) {

		// when the originator is already parsed it means that all subseqent elements
		// in the vector will comes from frame payload
		if (cTarget->ToISOriginator.Call != "UNINITIALIZED") {

			// checking if the payload is in initialized state
			if (payload == "")
				payload = e;
			else {
				// if not and there is any content stored in 'payload' object
				payload += ("," + e);
			}

			continue;
		}

		// the last element in the vector should consist frame payload
		//if (e == pathElements.back()) {	// 'back()' returns a value of the last element in vector
		//	payload = e;
		//	continue;
		//}

		// the one before last element in the path consist the callsign of a station which
		// sent this packet from RF to APRS-IS
		if (parseOrginator) {

			// copying the callsign and the ssid to appropriate fields inside an output object
			AprsPacket::SeparateCallSsid(e, cTarget->ToISOriginator.Call, cTarget->ToISOriginator.SSID);

			parseOrginator = false;

			continue;
		}

		// this will be a 'qXX' construct which takes some information about the way this packet entered the IS
		// more details about q constructs here: http://www.aprs-is.net/q.aspx
		if (boost::regex_match(e, qc)) {

			// the q-construct never has any ssid
			uint8_t dummy;

			// copying the callsign and the ssid to appropriate fields inside an output object
			AprsPacket::SeparateCallSsid(e, cTarget->qOrigin, dummy);

			// setting this flag to true to tell a routine that next element in this loop
			// will consist a callsign of the station which sent this packet to the IS
			parseOrginator = true;

			continue;
		}

		// object to store path element
		PathElement pelem;

		// separating callsign from the ssid or WIDEx from 'remaining hops' value
		AprsPacket::SeparateCallSsid(e, pelem.Call, pelem.SSID);

		// storing element in Path vector
		cTarget->Path.push_back(pelem);

	}

	// copying the payload to the output object. There is no need to check the lenght of
	// payload due to 'buff_len > 1000' check done just at the begining of this method
	std::copy(payload.begin(), payload.end(), cTarget->Data);

	return OK;
/*
   int i,ii;  // liczniki do petli
    int pos = 0;    // pozycja w przetwarzanej ramce
    int ctemp;
    char tmp[2];
    char *src_t, *dst_t; // pomocnicze wskazniki do kopiowania danych pomiedzy tabelami
    if (*tInputBuffer == '#' || *tInputBuffer == 0x00 || buff_len > 1000 || buff_len < 5)
		return NOT_VALID_APRS_PACKET;
    src_t = tInputBuffer;
    ///////// Adres nadawcy
    dst_t = cTarget->SrcAddr;
    for(i = 0; i<=9 ;i++) {
        // przepisywanie adresu zrodla/nadawcy
        if( *(src_t + i) == '>') {
            i++;
            break;  // jezeli petla spotkala znak '>' to oznacza to koniec znaku nadawcy
        }
        else if(*(src_t + i) == '-') {
            // opcjonalne SSID nadawcy
            tmp[0] = *(src_t + i + 1);
            tmp[1] = *(src_t + i + 2);
            i++;
            ctemp = atoi(tmp);
            cTarget->SrcSSID = ctemp;
            do {
				if (*(src_t + i + pos) == 0x00 || (i + pos > buff_len))
					return CORRUPTED_APRS_PACKET;
                 // zwiekszanie licznika i do momentu dojscia do >
                 i++;
            } while (*(src_t + i + pos) != '>');
            i++;   // kolejne zwiekszanie o jeden zeby przeskoczyc na pierwszy znak kolejnego elementu
            break;
        }
        else {
            *(dst_t + i) = *(src_t + i);
        }
    }
    pos = i;
    /////// Adres przeznaczenia tu nie ma SSID
    dst_t = cTarget->DestAddr;
    for (i = 0; i <= 6; i++) {
        if (*(src_t + i + pos) == ',') {  // przecinek rozdziela poszczegolne
            i++;
            break;
        }
        else
           *(dst_t + i) = *(src_t + i + pos);
    }
    pos += i;
    ////// sciezka pakietowa
    ii = 0;
                tmp[0] = *(src_t + pos);            // wpisywanie do bufora dwoch kolejnych znakow
                tmp[1] = *(src_t + pos + 1);
    while(ii <= 5 && (tmp[0] != 'q') && (tmp[1] != 'A')) {
//	dst_t = cTarget->Path[ii].Call;
        // przetwarzanie maksymalnie 5 stopni do wyrazenia qA*
        for(i = 0; i<=9 ;i++) {
            if( *(src_t + i + pos) == ',') {
                i++;    // jezeli napotkano przecinek rozdzielajacy elementy to przesun na nastepny znak
                tmp[0] = *(src_t + i + pos);            // wpisywanie do bufora dwoch kolejnych znakow
                tmp[1] = *(src_t + i + pos + 1);
                break;  // i przerwij wykonywanie tej petli for
            }
            else if ( *(src_t + i + pos) == '-') {
                // SSID jezeli wystepuje
                tmp[0] = *(src_t + i + pos + 1);
                tmp[1] = *(src_t + i + pos + 2);
                ctemp = atoi(tmp);
                cTarget->Path[ii].SSID = ctemp;
                do {
					if (*(src_t + i + pos) == 0x00 || (i + pos > buff_len))
						return -1;
                    // zwiekszanie licznika i do momentu dojscia do przecinka
                    i++;
                } while (*(src_t + i + pos) != ',');
                i++;   // kolejne zwiekszanie o jeden zeby przeskoczyc na pierwszy znak kolejnego elementu
                tmp[0] = *(src_t + i + pos);            // wpisywanie do bufora dwoch kolejnych znakow
                tmp[1] = *(src_t + i + pos + 1);
                break;
            }
            else {
                // przepisywanie
                *(dst_t + i) = *(src_t + i + pos);
                tmp[0] = *(src_t + i + pos + 1);            // wpisywanie do bufora dwoch kolejnych znakow
                tmp[1] = *(src_t + i + pos + 2);
            }
        }
        pos += i;
        ii++;
    }
    //cTarget->PathLng = ii - 1;      // dlugosc sciezki pakietowej
 //   pos += 2;
    i = 0;
    // przepisywanie qA*
    dst_t = cTarget->qOrigin;
    do {
		if (*(src_t + i + pos) == 0x00 || (i + pos > buff_len))
			return -1;
        if (i == 3) {
            *(dst_t + i + 1) = '\0';
            break;
        }
        *(dst_t + i) = *(src_t + i + pos);
        i++;
    } while (*(src_t + i + pos) != ',');
    pos += (i + 1);
    *(dst_t + i) = '\0';
    // przepisywanie adres oryginatora do IS
 //   dst_t = cTarget->ToISOriginator.Call;
    for (i = 0; i<=10 ;i++) {
        if( *(src_t + i + pos) == ':' || *(src_t + i + pos) == ',') {
            i++;    //przecinek oznacza poczatek tresci ramki
            break;
        }
        else if ( *(src_t + i + pos) == '-') {
            // SSID jezeli wystepuje
            tmp[0] = *(src_t + i + pos + 1);
            tmp[1] = *(src_t + i + pos + 2);
            ctemp = atoi(tmp);
            cTarget->Path[ii].SSID = ctemp;
            do {
				if (*(src_t + i + pos) == 0x00 || (i + pos > buff_len))
					return -1;				
                i++;
            } while (*(src_t + i + pos) != ':');
            i++;   // kolejne zwiekszanie o jeden zeby przeskoczyc na treść ramki
            break;
        }
        else
            *(dst_t + i) = *(src_t + i + pos);
    }
    pos += i;
    i = 0;
   if (*(src_t + pos - 1) != ':') {
	while (*(src_t + i + pos) != ':') {
		pos++;
	}
	pos++;
   }
//    pos++;
    dst_t = cTarget->Data;
    for (i = 0; (i <= buff_len && *(src_t + i + pos) != '\n') ; i++)
        *(dst_t + i) = *(src_t + i + pos);
	return OK;

	*/
}

void AprsPacket::clear() {

    this->DestAddr = "";
    this->DstSSID = 0;

    this->SrcAddr = "";
    this->SrcSSID = 0;

    this->Path.clear();
    this->qOrigin = "";

    memset(this->Data,0x00,sizeof(this->Data));

    this->ToISOriginator.Call = "UNINITIALIZED";

}
