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

	Magick::Image image;

	Magick::DrawableFont font;

public:

	void createBanner(AprsWXData & data);

	bool saveToDisk(std::string fn);

	BannerCreator(BannerCreatorConfig & config);
	virtual ~BannerCreator();
};

#endif /* BANNERCREATOR_H_ */
