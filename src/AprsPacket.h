#ifndef APRSPACKET_H
#define APRSPACKET_H

#include <exception>
#include <cstdint>
#include <string>
#include <vector>


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
    std::string Call;
    uint8_t SSID;
};

class AprsPacket
{
    private:
    public:
        std::string DestAddr;   // Destination address, sliced from packer. In APRS it works as Device-ID
        std::string SrcAddr;    // Source address
        uint8_t SrcSSID;  // SSID of Source
        uint8_t DstSSID;
        std::vector<PathElement> Path;        // Routing Path
        std::string qOrigin;        // APRS-IS originator
        PathElement ToISOriginator;     // APRS-IS originator. It might be Igate callsign or APRS server name, if packet
                                        // was sent directly to Internet from some APRS client.
        char Data[1024];         // Frame payload
        void PrintPacketData();     // Function witch print data from processed packet
        
		static int ParseAPRSISData(char* tInputBuffer, int buff_len, AprsPacket* cTarget);
		static bool SeparateCallSsid(const std::string& input, std::string& call, uint8_t& ssid);
		static bool SeparateCallSsid(const std::string& input, char (&call)[7], uint8_t& ssid);
		
		void clear();

		AprsPacket();          // Class constructor. It prepares all variables by writing zeros to it
		~AprsPacket();
};

#endif // APRSPACKET_H
