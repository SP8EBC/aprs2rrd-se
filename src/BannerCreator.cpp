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


void BannerCreator::putCenteredText(const std::string text, Magick::DrawableFont & font, std::string color, float fontSize, float x, float y)
{
	std::list<Magick::Drawable> list;
	list.push_back(font);
	list.push_back(Magick::DrawableText(x, y, text));
	list.push_back(Magick::DrawableStrokeColor(Magick::Color("black")));
	list.push_back(Magick::DrawableFillColor(Magick::Color("black")));
	list.push_back(Magick::DrawableGravity(Magick::GravityType::CenterGravity));

	image.fontPointsize(fontSize);
	image.draw(list);
}

void BannerCreator::drawRunwayRect(int runwayDirection)
{
	std::list<Magick::Drawable> list;

	Magick::Image temporary(Magick::Geometry(50, 600), Magick::Color(0, 0, 0, 0xFFFFU));

    list.push_back(Magick::DrawableStrokeColor("red")); // Outline color 
    list.push_back(Magick::DrawableStrokeWidth(1)); // Stroke width 
    list.push_back(Magick::DrawableFillColor("green")); // Fill color

	// Add a Rectangle to drawing list 
    list.push_back(Magick::DrawableRectangle(0,600, 50,0));

	temporary.draw(list);

	temporary.rotate(cfg.runwayDirection);

	temporary.transparent(Magick::Color("white"));

	image.composite(temporary, Magick::GravityType::CenterGravity, Magick::OverlayCompositeOp);
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

void BannerCreator::createBanner(AprsWXData &data)
{

    // https://stackoverflow.com/questions/54071601/graphicsmagick-ttf-font-performance

	const std::string windspeed = BannerCreator::floatToStringWithPrecision(data.wind_speed, 2);

	if (cfg.drawRunway) {
		drawRunwayRect(cfg.runwayDirection);
	}

	// prepare arrow to be drawn
	arrow.rotate(data.wind_direction);
	arrow.transparent(Magick::Color("white"));

	// put windrose first
	image.composite(windrose, 0, 0, Magick::OverlayCompositeOp);

	// put arrow
	image.composite(arrow, Magick::GravityType::CenterGravity, Magick::OverlayCompositeOp);

	putCenteredText(windspeed, fontBig, "black", 256.0, 0, 0);
	putCenteredText(std::string("Avg Windspeed [m/s]"), fontNormalCaption, "black", 32, 0.0f, -96.0f);

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
		cfg(config), 
		ARROW_CENTER_SCALE_X(1.0f), 
		ARROW_CENTER_SCALE_Y(1.0f),
		ROSE_INT_SIZ_X(445),
		ROSE_INT_SIZ_Y(445),
		image(	Magick::Geometry(config.x, config.y),
				Magick::Color(0xFFFFU, 0xFFFFU, 0xFFFFU, 0xFFFFU)),
		fontBig(config.fontBig),
		fontNormalCaption(config.fontTitle),
		windrose(config.assetsBasePath + "windrose.png"),
		arrow(config.assetsBasePath + "arrow.png"){
}

BannerCreator::~BannerCreator() {
}

