/*
 * RRDFileDefinition.cpp
 *
 *  Created on: 06.05.2020
 *      Author: mateusz
 */

#include "RRDFileDefinition.h"

void RRDFileDefinition::Zero(void) {
	this->eType = PlotType::N;
	this->sPath.clear();
}

RRDFileDefinition::~RRDFileDefinition() {
}

bool RRDFileDefinition::operator ==(const RRDFileDefinition& _in) {
	if (_in.eType == this->eType)
		return true;
	else
		return false;
}

RRDFileDefinition::RRDFileDefinition() {
	this->eType = PlotType::N;
	this->sPath = "";
}

RRDFileDefinition::RRDFileDefinition(PlotType t) {
	this->eType = t;
	this->sPath = "";
}
