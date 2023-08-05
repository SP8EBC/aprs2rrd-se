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
	const int x;

	/**
	 * Horizontal banner size
	 */
	const int y;

	std::string font;

	std::string fontTitle;

	/**
	 * Base path to all assets used for banner generation
	*/
	std::string assetsBasePath;

	std::string outputFile;

	bool transparent;

	BannerCreatorConfig();
	virtual ~BannerCreatorConfig();
};

#endif /* BANNERCREATORCONFIG_H_ */
