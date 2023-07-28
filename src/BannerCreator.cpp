/*
 * BannerCreator.cpp
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#include "BannerCreator.h"

#include <list>

void BannerCreator::createBanner(AprsWXData & data) {

	std::list<Magick::Drawable> thingsToDraw;

	thingsToDraw.push_back(font);
	thingsToDraw.push_back(Magick::DrawableText(50, 75, "dupa"));
	thingsToDraw.push_back(Magick::DrawableStrokeColor(Magick::Color("black")));
	thingsToDraw.push_back(Magick::DrawableFillColor(Magick::Color(0, 0, 0, 255)));

	image.draw(thingsToDraw);

}

bool BannerCreator::saveToDisk(std::string fn) {

	image.magick("png");
	image.write(fn);

	return true;
}

BannerCreator::BannerCreator(BannerCreatorConfig &config) :
		cfg(config),
		image(	Magick::Geometry(config.x, config.y),
				Magick::Color(0, 0, 0, 255)),
		font(config.font){
}

BannerCreator::~BannerCreator() {
}

