/*
 * BannerCreator.h
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#ifndef BANNERCREATOR_H_
#define BANNERCREATOR_H_

#include "BannerCreatorConfig.h"
#include "AprsWXData.h"

#include <string>
#include <tuple>
#include <Magick++.h>

class BannerCreator {

	BannerCreatorConfig cfg;

	const float ARROW_CENTER_SCALE_X;

	const float ARROW_CENTER_SCALE_Y;

	const int ROSE_INT_SIZ_X;

	const int ROSE_INT_SIZ_Y;

	/// @brief 
	Magick::Image image;

	/// @brief 
	Magick::DrawableFont fontBig;
	
	/// @brief 
	Magick::DrawableFont fontNormalCaption;

	/// @brief 
	Magick::Image windrose;

	/// @brief 
	Magick::Image arrow;

	/// @brief 
	/// @param text 
	/// @param font 
	/// @param color 
	/// @param fontSize 
	/// @param x 
	/// @param y 
	void putCenteredText(std::string text, Magick::DrawableFont & font, std::string color, float fontSize, float x, float y);

	/// @brief 
	/// @param runwayDirection 
	void drawRunwayRect(int runwayDirection);

	/// @brief 
	/// @param number 
	/// @param precision 
	/// @return 
	static std::string floatToStringWithPrecision(float number, int precision);

	/// @brief 
	/// @return 
	static std::string currentTimeToString();

public:

	void createBanner(AprsWXData & data);

	bool saveToDisk(std::string fn);

	BannerCreator(BannerCreatorConfig & config);
	virtual ~BannerCreator();
};

#endif /* BANNERCREATOR_H_ */
