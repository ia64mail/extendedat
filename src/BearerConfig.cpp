/*
 * BearerConfig.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: ia64domain
 */

#include "BearerConfig.h"

const char BearerConfig::connectionTypeParamName[] = "CONTYPE";
const char BearerConfig::apnNameParamName[] = "APN";
const char BearerConfig::userNameParamName[] = "USER";
const char BearerConfig::userPasswordParamName[] = "PWD";
const char BearerConfig::phoneNumberParamName[] = "PHONENUM";
const char BearerConfig::connectionSpeedParamName[] = "RATE";

BearerConfig::BearerConfig(unsigned int bearerProfileID,
		const BEARER_CONFIG_CONNECTION_TYPE connectionType, const char * apnName[APN_MAX_LENGHT]) {
	this->bearerProfileID = bearerProfileID;
	this->connectionType = connectionType;
	this->apnName

}

BearerConfig::~BearerConfig() {
	// TODO Auto-generated destructor stub
}

