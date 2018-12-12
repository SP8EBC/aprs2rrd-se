#ifndef APRSPACKET_H
#define APRSPACKET_H

#include <exception>
#include <cstdint>
#include <string>


class NotValidAprsPacket: public std::exception {
    virtual const char* what() const throw() {
		return "\n--- Niepoprawny pakiet APRS\n";
    }
};

class PacketParsedOK: public std::exception {
    virtual const char* what() const throw() {
		return "\n--- Pakiet przetworzony OK\n";
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
        char Data[1024];         // Data from frame
        void PrintPacketData();     // Function witch print data from processed packet
        
		static int ParseAPRSISData(char* tInputBuffer, int buff_len, AprsPacket* cTarget);
		static bool SeparateCallSsid(const std::string& input, std::string& call, uint8_t& ssid);
		static bool SeparateCallSsid(const std::string& input, char (&call)[7], uint8_t& ssid);
		
		void clear();

		AprsPacket();          // Class constructor. It prepares all variables by writing zeros to it
		~AprsPacket();
};

#endif // APRSPACKET_H
