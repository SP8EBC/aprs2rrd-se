/*
 * BannerCreatorConfig.cpp
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#include "BannerCreatorConfig.h"

BannerCreatorConfig::BannerCreatorConfig() : x(890), y (890),
											fontBig("/usr/share/fonts/truetype/digital-7/digital-7 (mono).ttf"),
											fontTitle("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
											assetsBasePath(""),
											transparent(true),
											blackOrWhiteBackground(false),
											drawRunway(false),
											runwayDirection(180) {
	// TODO Auto-generated constructor stub

}

BannerCreatorConfig::~BannerCreatorConfig() {
	// TODO Auto-generated destructor stub
}

