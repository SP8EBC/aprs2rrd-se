/*
 * BannerCreatorConfig.h
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#ifndef BANNERCREATORCONFIG_H_
#define BANNERCREATORCONFIG_H_

#include <string>

class BannerCreatorConfig {
public:

	/**
	 * Vertical banner size
	 */
	int x;

	/**
	 * Horizontal banner size
	 */
	int y;

	std::string font;

	BannerCreatorConfig();
	virtual ~BannerCreatorConfig();
};

#endif /* BANNERCREATORCONFIG_H_ */
