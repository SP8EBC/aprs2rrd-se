#ifndef D630CE72_214E_4A0C_806B_5BB73D6CC123
#define D630CE72_214E_4A0C_806B_5BB73D6CC123

#include "AprsPacket.h"
#include "AprsWXData.h"

#define APRS_WX_DATA_POSITIONLESS_IGNORE_TS_FROM_FRAME		(true)

class AprsWXDataPositionless {

public:
	static int ParseData(const std::string & in, AprsWXData * output, bool ignoreTimestampFromFrame);


};

#endif /* D630CE72_214E_4A0C_806B_5BB73D6CC123 */
