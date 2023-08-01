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
#include <ImageMagick-6/Magick++.h>

class BannerCreator {

	BannerCreatorConfig cfg;

	/**
	 * loaded image with windrose
	*/
	Magick::Image windrose;

	/**
	 * Loaded image with an arrow
	*/
	Magick::Image arrow;

	Magick::Image image;

	Magick::DrawableFont font, fontTitle;

	void putText(std::string text, Magick::DrawableFont font, std::string color, float fontSize, float x, float y);

	static std::string floatToStringWithPrecision(float number, int precision);

	static std::string currentTimeToString();

public:

	void createBanner(AprsWXData & data);

	bool saveToDisk(std::string fn);

	BannerCreator(BannerCreatorConfig & config);
	virtual ~BannerCreator();
};

#endif /* BANNERCREATOR_H_ */
