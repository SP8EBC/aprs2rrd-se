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

	/// @brief if banner creator should be enabled
	bool enable;

	/**
	 * Vertical banner size
	 */
	const int x;

	/// @brief Horizontal banner size
	const int y;

	/// @brief 
	std::string fontBig;

	/// @brief 
	std::string fontTitle;

	/// @brief Base path to all assets used for banner generation
	std::string assetsBasePath;

	std::string outputFile;

	bool transparent;

	/// @brief Controls background color if transparency is disabled
	bool blackOrWhiteBackground;

	/// @brief If output banner should contains a minick of runway direction
	bool drawRunway;

	/// @brief 
	int runwayDirection;

	BannerCreatorConfig();
	virtual ~BannerCreatorConfig();
};

#endif /* BANNERCREATORCONFIG_H_ */
