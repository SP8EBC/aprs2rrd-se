/*
 * PlotFileDefinition.cpp
 *
 *  Created on: 06.05.2020
 *      Author: mateusz
 */


#include "PlotFileDefinition.h"

PlotFileDefinition::PlotFileDefinition() {
	this->eType = PlotType::N;
	this->sPath.clear();
	this->DS0PlotColor = 0;
	this->DS1PlotColor = 0;
	this->Height = 0;
	this->Width = 0;
	this->Title.clear();
	this->eDS0PlotType = PlotGraphType::NN;
	this->eDS1PlotType = PlotGraphType::NN;
	this->sDS0Name.clear();
	this->sDS1Name.clear();
	this->sDS0Path.clear();
	this->sDS1Path.clear();
	this->ScaleStep = 0;
	this->LabelStep = 0;
	this->Exponent = -1;
	this->DoubleDS = false;
	this->MinScaleR = 0.0f;
	this->MaxScale = 0.0f;
	this->eDS0RRAType = RRAType::NNN;
	this->eDS1RRAType = RRAType::NNN;

	this->timeScaleLn = 20 * 3600;

	this->MaxScaleR = 0.0f;
	this->MinScale = 0.0f;
}

void PlotFileDefinition::Zero(void) {
	this->eType = PlotType::N;
	this->sPath.clear();
	this->DS0PlotColor = 0;
	this->DS1PlotColor = 0;
	this->Height = 0;
	this->Width = 0;
	this->Title.clear();
	this->eDS0PlotType = PlotGraphType::NN;
	this->eDS1PlotType = PlotGraphType::NN;
	this->sDS0Name.clear();
	this->sDS1Name.clear();
	this->sDS0Path.clear();
	this->sDS1Path.clear();
	this->ScaleStep = 0;
	this->LabelStep = 0;
	this->Exponent = -1;
}

PlotFileDefinition::~PlotFileDefinition() {
}

