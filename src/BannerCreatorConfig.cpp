/*
 * BannerCreatorConfig.cpp
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#include "BannerCreatorConfig.h"

BannerCreatorConfig::BannerCreatorConfig() : x(1024), y (1024),
											font("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
											fontTitle("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
											assetsBasePath(""),
											transparent(true) {
	// TODO Auto-generated constructor stub

}

BannerCreatorConfig::~BannerCreatorConfig() {
	// TODO Auto-generated destructor stub
}

