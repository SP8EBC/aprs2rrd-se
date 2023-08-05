/*
 * BannerCreator.cpp
 *
 *  Created on: Jul 28, 2023
 *      Author: mateusz
 */

#include <boost/date_time/local_time/local_time.hpp>
#include "BannerCreator.h"

#include <cmath>
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

std::tuple<float, float> BannerCreator::calculateArrowPosition(int windDirection)
{

	// division wide 36 pixels, half 18 pixels

	/**
	 * North, x:355, y:89	(x:  355 ,  y1: 71 )
	 * West:  x:616, y:350	(x1: 634 ,  y:  350)
	 * South: x:355, y:611	(x:  355 ,  y1: 629)
	 * East:  x:94,  y:350	(x1: 112 ,  y:  350)
	 * 
	*/

	float x = sin(((windDirection) * M_PI / 180.0)) * cfg.x - cfg.x;

	float y = cos(((windDirection) * M_PI / 180.0)) * cfg.y - cfg.y;

	std::cout << "--- BannerCreator::calculateArrowPosition:70 - x: " << x << ", y: " << y << std::endl;

    return std::tuple<float, float>(x, y);
}

void BannerCreator::createBanner(AprsWXData &data)
{

    // https://stackoverflow.com/questions/54071601/graphicsmagick-ttf-font-performance
/*
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
*/

/*** 10%
 * 		ARROW_CENTER_SCALE_X(0.47f), 
		ARROW_CENTER_SCALE_Y(0.47f),
		ROSE_INT_SIZ_X(261),
		ROSE_INT_SIZ_Y(261),
 * 
*/

/***
 * 	20%
 * 		ARROW_CENTER_SCALE_X(0.44f), 
		ARROW_CENTER_SCALE_Y(0.44f),
		ROSE_INT_SIZ_X(261),
		ROSE_INT_SIZ_Y(261),
 * 
*/

	// prepare arrow to be drawn
	//arrow.resize("45%");
	arrow.rotate(data.wind_direction);
	//arrow.backgroundColor(Magick::Color(0, 0, 0, 0));
	arrow.transparent(Magick::Color("white"));

	// calculate point to place arrow on
	const std::tuple<float, float> angles = calculateArrowPosition(data.wind_direction);

	// put windrose first
	image.composite(windrose, 0, 0, Magick::OverlayCompositeOp);

	// put arrow
	//image.composite(arrow, std::get<0>(angles), std::get<1>(angles), Magick::OverlayCompositeOp);
	image.composite(arrow, Magick::GravityType::CenterGravity, Magick::OverlayCompositeOp);

	}

bool BannerCreator::saveToDisk(std::string fn) {

	if (fn.size() > 1) {
		image.magick("png");
		image.write(fn);

		std::cout << "--- BannerCreator::saveToDisk:138 - Banner saved to file: " << fn << std::endl;

		return true;
	}
	else {
		return false;
	}

}

BannerCreator::BannerCreator(BannerCreatorConfig &config): 
		ARROW_CENTER_SCALE_X(1.0f), 
		ARROW_CENTER_SCALE_Y(1.0f),
		ROSE_INT_SIZ_X(445),
		ROSE_INT_SIZ_Y(445),
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

