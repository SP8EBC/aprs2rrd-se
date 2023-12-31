#include "AprsPacket.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>

using namespace std;

AprsPacket::AprsPacket() {
    unsigned i;
    memset(this->DestAddr,0x00,sizeof(this->DestAddr));
    memset(this->SrcAddr,0x00,sizeof(this->SrcAddr));
    this->SrcSSID = 0;
    memset(this->Data,0x00,sizeof(this->Data));
    for (i = 0; i<=5; i++) {
        memset(this->Path[i].Call, 0x00, sizeof(this->Path[i].Call));
        this->Path[i].SSID = 0;
    }
    memset(this->ToISOriginator.Call, 0x00, sizeof(this->ToISOriginator.Call));
    this->PathLng = 0;
}

AprsPacket::~AprsPacket()
{
}

void AprsPacket::PrintPacketData() {
    cout << "-----------------------------------------------" << endl;
    cout << "-- Przetworzone dane stacji: " << this->SrcAddr << "-" << this->SrcSSID << endl;
    cout << "-- Id urządzenia: " << this->DestAddr << endl;
    cout << "-- Długość Ścieżki: " << this->PathLng + 1 << endl;
    for (int i = 0; i<= this->PathLng && i <= 5; i++)
        cout << "---- Element Numer: " << i << " = " << this->Path[i].Call << "-" << this->Path[i].SSID << endl;
    cout << "-- Typ wysyłającego do APRS-IS: " << this->qOrigin << endl;
    cout << "-- Wysyłający do APRS-IS: " << this->ToISOriginator.Call << endl;
    cout << "-- Dane: " << this->Data << endl;
    cout << "----------------------------------------------- \r\n";
}


short AprsPacket::ParseAPRSISData(char* tInputBuffer, int buff_len, AprsPacket* cTarget) {
   int i,ii;  // liczniki do petli
    int pos = 0;    // pozycja w przetwarzanej ramce
    int ctemp;
    char tmp[2];
    char *src_t, *dst_t; // pomocnicze wskazniki do kopiowania danych pomiedzy tabelami
    if (*tInputBuffer == '#')
		throw NotValidAprsPacket();
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
					return -1;
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
	dst_t = cTarget->Path[ii].Call;
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
    cTarget->PathLng = ii - 1;      // dlugosc sciezki pakietowej
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
    dst_t = cTarget->ToISOriginator.Call;
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
    throw PacketParsedOK();
	return 0;
}

