/*
 * BannerCreator.cpp
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#include "BannerCreator.h"

#include <list>

void BannerCreator::createBanner(AprsWXData & data) {

	const std::string direction = "Kierunek: " + std::string(to_string(data.wind_direction));

	std::list<Magick::Drawable> title;
	title.push_back(fontTitle);
	title.push_back(Magick::DrawableText(250, 25, "Dane pogodowe"));
	title.push_back(Magick::DrawableStrokeColor(Magick::Color("white")));
	title.push_back(Magick::DrawableFillColor(Magick::Color(0, 0, 0, 0)));

	std::list<Magick::Drawable> winddirection;
	winddirection.push_back(font);
	winddirection.push_back(Magick::DrawableText(230, 45, direction));
	winddirection.push_back(Magick::DrawableStrokeColor(Magick::Color("white")));
	winddirection.push_back(Magick::DrawableFillColor(Magick::Color(0, 0, 0, 0)));

	arrow.resize("50%");
	arrow.rotate(-45.0 + data.wind_direction);
	arrow.backgroundColor(Magick::Color(0, 0, 0, 0));
	arrow.transparent(Magick::Color("black"));

	windrose.resize("50%");

	image.composite(arrow, 7, 3, Magick::OverlayCompositeOp);
	image.transparent(Magick::Color("black"));
	image.composite(windrose, 15, 7, Magick::OverlayCompositeOp);
	image.draw(title);
	image.draw(winddirection);

}

bool BannerCreator::saveToDisk(std::string fn) {

	image.magick("png");
	image.write(fn);

	return true;
}

BannerCreator::BannerCreator(BannerCreatorConfig &config) :
		cfg(config),
		image(	Magick::Geometry(config.x, config.y),
				Magick::Color(0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU)),
		font(config.font),
		fontTitle(config.fontTitle),
		windrose(config.assetsBasePath + "windrose.png"),
		arrow(config.assetsBasePath + "arrow.png"){
}

BannerCreator::~BannerCreator() {
}

