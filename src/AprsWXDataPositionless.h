#ifndef D630CE72_214E_4A0C_806B_5BB73D6CC123
#define D630CE72_214E_4A0C_806B_5BB73D6CC123

#include "AprsPacket.h"
#include "AprsWXData.h"

class AprsWXDataPositionless {

public:
	static int ParseData(const std::string & in, AprsWXData * output);


};

#endif /* D630CE72_214E_4A0C_806B_5BB73D6CC123 */
