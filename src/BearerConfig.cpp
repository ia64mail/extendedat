/*
 * BearerConfig.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: ia64domain
 */

#include "BearerConfig.h"

using namespace std;

const char BearerConfig::connectionTypeParamName[] = "CONTYPE";
const char BearerConfig::apnNameParamName[] = "APN";
const char BearerConfig::userNameParamName[] = "USER";
const char BearerConfig::userPasswordParamName[] = "PWD";
const char BearerConfig::phoneNumberParamName[] = "PHONENUM";
const char BearerConfig::connectionSpeedParamName[] = "RATE";

const char BearerConfig::connectionTypeGPRSValue[] = "GPRS";
const char BearerConfig::connectionTypeCSDValue[] = "CSD";

/**
 * Create config for GPRS connection.
 * Profile ID and APN name are required.
 */
BearerConfig::BearerConfig(unsigned int bearerProfileID, const char apnName[APN_MAX_LENGHT],
		const char userName[USER_MAX_LENGHT], const char userPassword[PWD_MAX_LENGHT]) {
	this->bearerProfileID = bearerProfileID;
	configureGRPS(apnName, userName, userPassword);
}

/**
 * Create config for CSD connection.
 * Profile ID, APN name, phone number and connection speed are required.
 */
BearerConfig::BearerConfig(unsigned int bearerProfileID, const char apnName[APN_MAX_LENGHT],
		const char phoneNumber[PNUM_MAX_LENGHT],
		const BEARER_CONFIG_CONNECTION_SPEED connectionSpeed,
		const char userName[USER_MAX_LENGHT], const char userPassword[PWD_MAX_LENGHT]) {
	this->bearerProfileID = bearerProfileID;
	configureCSD(apnName, phoneNumber, connectionSpeed, userName, userPassword);
}

BearerConfig::~BearerConfig() {

}

/**
 * Reconfigure existed config for GPRS connection.
 * Profile ID and APN name are required.
 */
void BearerConfig::configureGRPS(const char apnName[APN_MAX_LENGHT],
		const char userName[USER_MAX_LENGHT],	const char userPassword[PWD_MAX_LENGHT]) {
	setConnectionType(BEARER_PARAM_CONTYPE_GPRS);
	setApnName(apnName);
	setUserName(userName);
	setUserPassword(userPassword);
}

/**
 * Reconfigure existed config for CSD connection.
 * Profile ID, APN name, phone number and connection speed are required.
 */
void BearerConfig::configureCSD(const char apnName[APN_MAX_LENGHT], const char phoneNumber[PNUM_MAX_LENGHT],
		const BEARER_CONFIG_CONNECTION_SPEED connectionSpeed,
		const char userName[USER_MAX_LENGHT], const char userPassword[PWD_MAX_LENGHT]) {
	setConnectionType(BEARER_PARAM_CONTYPE_CSD);
	setApnName(apnName);
	setPhoneNumber(phoneNumber);
	setConnectionSpeed(connectionSpeed);
	setUserName(userName);
	setUserPassword(userPassword);
}

/**
 * Return true if user or password was set for this config.
 */
bool BearerConfig::isUserNameAndPasswordDefined() const {
	if(strlen(getUserName()) == 0 && strlen(getUserPassword()) == 0) {
		return false;
	}
	return true;
}

/**
 * Get numeric value for BEARER_CONFIG_CONNECTION_SPEED param
 */
const unsigned char BearerConfig::getConnectionSpeedValue() const {
	unsigned char val;

	switch(getConnectionSpeed()) {
	case BEARER_PARAM_CSD_SPEED_4800:
		val = 1;
		break;
	case BEARER_PARAM_CSD_SPEED_9600:
		val = 2;
		break;
	case BEARER_PARAM_CSD_SPEED_14400:
		val = 3;
		break;
	case BEARER_PARAM_CSD_SPEED_2400:
	default:
		val = 0;
		break;
	}

	return val;
}

void BearerConfig::setConnectionType(const BEARER_CONFIG_CONNECTION_TYPE connectionType){
	this->connectionType = connectionType;
}

void BearerConfig::setApnName(const char apnName[APN_MAX_LENGHT]){
	strcpy(this->apnName, apnName);
}

void BearerConfig::setUserName(const char userName[USER_MAX_LENGHT]){
	strcpy(this->userName, userName);
}

void BearerConfig::setUserPassword(const char userPassword[PWD_MAX_LENGHT]){
	strcpy(this->userPassword, userPassword);
}

void BearerConfig::setPhoneNumber(const char phoneNumber[PNUM_MAX_LENGHT]){
	strcpy(this->phoneNumber, phoneNumber);
}

void BearerConfig::setConnectionSpeed(const BEARER_CONFIG_CONNECTION_SPEED connectionSpeed){
	this->connectionType = connectionType;
}

const unsigned int & BearerConfig::getBearerProfileID() const{
	return this->bearerProfileID;
}

const BEARER_CONFIG_CONNECTION_TYPE & BearerConfig::getConnectionType() const{
	return this->connectionType;
}

const char * const BearerConfig::getConnectionTypeValue() const {
	switch(getConnectionType()) {
	case BEARER_PARAM_CONTYPE_CSD:
		return connectionTypeCSDValue;
		break;
	case BEARER_PARAM_CONTYPE_GPRS:
	default:
		return connectionTypeGPRSValue;
		break;
	}

	return connectionTypeGPRSValue;
}

const char * const BearerConfig::getApnName() const{
	return this->apnName;
}

const char * const BearerConfig::getUserName() const{
	return this->userName;
}

const char * const BearerConfig::getUserPassword() const{
	return userPassword;
}

const char * const BearerConfig::getPhoneNumber() const{
	return this->phoneNumber;
}

const BEARER_CONFIG_CONNECTION_SPEED & BearerConfig::getConnectionSpeed() const{
	return this->connectionSpeed;
}

const char * const BearerConfig::getConnectionTypeParamName() const{
	return BearerConfig::connectionTypeParamName;
}

const char * const BearerConfig::getApnNameParamName() const{
	return BearerConfig::apnNameParamName;
}

const char * const BearerConfig::getUserNameParamName() const{
	return BearerConfig::userNameParamName;
}

const char * const BearerConfig::getUserPasswordParamName() const{
	return BearerConfig::userPasswordParamName;
}

const char * const BearerConfig::getPhoneNumberParamName() const{
	return phoneNumberParamName;
}

const char * const BearerConfig::getConnectionSpeedParamName() const{
	return connectionSpeedParamName;
}
