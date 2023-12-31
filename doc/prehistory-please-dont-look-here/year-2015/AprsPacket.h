#ifndef APRSPACKET_H
#define APRSPACKET_H

#include <exception>


class NotValidAprsPacket: public std::exception {
    virtual const char* what() const throw() {
		return "\n--- Brak komunikacji z serwerem\n";
    }
};

class PacketParsedOK: public std::exception {
    virtual const char* what() const throw() {
		return "\n--- Brak komunikacji z serwerem\n";
    }
};

struct PathElement {
    char Call[10];
    unsigned char SSID;
};

class AprsPacket
{
    private:
    public:
        char DestAddr[7];   // Destination address, sliced from packer. In APRS it works as Device-ID
        char SrcAddr[7];    // Source address
        unsigned char SrcSSID;  // SSID of Source
        PathElement Path[5];        // Routing Path
        unsigned char PathLng;      // Number of elements in path
        char qOrigin[4];        // APRS-IS originator
        PathElement ToISOriginator;     // APRS-IS originator. It might be Igate callsign or APRS server name, if packet
                                        // was sent directly to Internet from some APRS client.
        char Data[128];         // Data from frame
        void PrintPacketData();     // Function witch print data from processed packet
        
		short ParseAPRSISData(char* tInputBuffer, int buff_len, AprsPacket* cTarget);
		
		AprsPacket();          // Class constructor. It prepares all variables by writing zeros to it
		~AprsPacket();
};

#endif // APRSPACKET_H
