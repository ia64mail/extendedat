/*
 * HTTPConfig.cpp
 *
 *  Created on: Mar 21, 2013
 *      Author: ia64domain
 */

#include "HTTPConfig.h"

using namespace std;

HTTPConfig::HTTPConfig(const unsigned int bearerProfileID, const char url[URL_MAX_LENGHT]) {
	this->bearerProfileID = bearerProfileID;
	strcpy(this->url, url);

	setUserAgent("unknown");
	setProxy("", 0);
	setRedirection(true);
	setBreakState(0,0);
	setTimeout(120);
	setContentType("");
}

HTTPConfig::~HTTPConfig() {
}

void HTTPConfig::setUserAgent(const char userAgent[USER_AGENT_MAX_LENGHT]) {
	strcpy(this->userAgent, userAgent);
}

void HTTPConfig::setProxy(const char proxyIP[16], unsigned int proxyPort) {
	strcpy(this->proxyIP, proxyIP);
	this->proxyPort = proxyPort;
}

void HTTPConfig::setRedirection(const bool enableRedirection) {
	this->enableRedirection = enableRedirection;
}

void HTTPConfig::setBreakState(const unsigned int breakStartPos, const unsigned int breakEndPos) {
	this->breakStartPos = breakStartPos;
	this->breakEndPos = breakEndPos;
}

int HTTPConfig::setTimeout(const unsigned int timeout) {
	if(timeout < 30 || timeout > 1000) {
		return -1;
	}

	this->timeout = timeout;
	return timeout;
}

void HTTPConfig::setContentType(const char contentType[CONTENT_TYPE_MAX_LENGHT]) {
	strcpy(this->contentType, contentType);
}

const unsigned int & HTTPConfig::getBearerProfileID() const {
	return this->bearerProfileID;
}

const char * const HTTPConfig::getUrl() const {
	return this->url;
}

const char * const HTTPConfig::getUserAgent() const{
	return this->userAgent;
}

const char * const HTTPConfig::getProxyIP() const {
	return this->proxyIP;
}

const unsigned int & HTTPConfig::getProxyPort() const {
	return this->proxyPort;
}

const unsigned int & HTTPConfig::getBreakStartPos() const {
	return this->breakStartPos;
}
const unsigned int & HTTPConfig::getBreakEndPos() const {
	return this->breakEndPos;
}

const unsigned int & HTTPConfig::getTimeout() const {
	return this->timeout;
}

const char * const HTTPConfig::getContentType() const {
	return this->contentType;
}

const bool HTTPConfig::isEnabledREdirection() const {
	return this->enableRedirection;
}

const bool HTTPConfig::isEnabledProxy() const {
	if(strcmp("", getProxyIP()) == 0 && getProxyPort() == 0) {
		return false;
	}
	return true;
}

const bool HTTPConfig::isEnabledBreakState() const {
	if(getBreakStartPos()  == 0 && getBreakEndPos() == 0) {
		return false;
	}
	return true;
}

