/*
 * BannerCreator.cpp
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#include <boost/date_time/local_time/local_time.hpp>
#include "BannerCreator.h"

#include <list>
#include <sstream>
#include <iomanip>


void BannerCreator::putText(std::string text, Magick::DrawableFont font, std::string color, float fontSize, float x, float y)
{
	std::list<Magick::Drawable> list;
	list.push_back(font);
	list.push_back(Magick::DrawableText(x, y, text));
	list.push_back(Magick::DrawableStrokeColor(Magick::Color("black")));
	list.push_back(Magick::DrawableFillColor(Magick::Color("black")));

	image.fontPointsize(fontSize);
	image.draw(list);
}

std::string BannerCreator::floatToStringWithPrecision(float number, int precision)
{
	std::stringstream ss;

	ss << std::setprecision(precision) << number;

    return ss.str();
}

std::string BannerCreator::currentTimeToString()
{
	boost::posix_time::ptime localtime(boost::date_time::second_clock<boost::posix_time::ptime>::local_time());

	boost::posix_time::time_facet * formatter = new boost::posix_time::time_facet();
	formatter->format("%d %B %H:%M");

	std::stringstream ss;
	ss.imbue(std::locale(std::locale::classic(), formatter));

	ss << localtime;

    return ss.str();
}

void BannerCreator::createBanner(AprsWXData & data) {

	// https://stackoverflow.com/questions/54071601/graphicsmagick-ttf-font-performance

	const std::string direction = "Kierunek wiatru [stopnie]: " + std::string(to_string(data.wind_direction));
	const std::string windspeed = "Prędkość wiatru [m/s]: " + BannerCreator::floatToStringWithPrecision(data.wind_speed, 2);
	const std::string windgusts = "Maksymalne porywy [m/s]: " + BannerCreator::floatToStringWithPrecision(data.wind_gusts, 2);
	const std::string temperatu = "Temperatura [C]: " + BannerCreator::floatToStringWithPrecision(data.temperature, 2);

	std::list<Magick::Drawable> title_list;
	title_list.push_back(fontTitle);
	title_list.push_back(Magick::DrawableText(250, 25, "Dane pogodowe"));
	title_list.push_back(Magick::DrawableStrokeColor(Magick::Color("black")));
	title_list.push_back(Magick::DrawableFillColor(Magick::Color(0, 0, 0, 0)));


	arrow.resize("50%");
	arrow.rotate(-45.0 + data.wind_direction);
	arrow.backgroundColor(Magick::Color(0, 0, 0, 0));
	arrow.transparent(Magick::Color("black"));

	windrose.resize("50%");

	image.composite(arrow, 7, 3, Magick::OverlayCompositeOp);
	image.transparent(Magick::Color("black"));
	image.composite(windrose, 15, 7, Magick::OverlayCompositeOp);
	image.fontPointsize(24.0);
	image.draw(title_list);
	putText(direction, font, "black", 16.0, 230, 55);
	putText(windspeed, font, "black", 16.0, 230, 75);
	putText(windgusts, font, "black", 16.0, 230, 95);
	putText(temperatu, font, "black", 16.0, 230, 115);

	std::list<Magick::Drawable> lastUpdateList;
	lastUpdateList.push_back(font);
	lastUpdateList.push_back(Magick::DrawableText(230, 170, "Ostatnia aktualizacja: " + BannerCreator::currentTimeToString()));
	lastUpdateList.push_back(Magick::DrawableStrokeColor(Magick::Color("black")));
	lastUpdateList.push_back(Magick::DrawableFillColor(Magick::Color("black")));
		image.fontPointsize(12.0);
	image.draw(lastUpdateList);


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

