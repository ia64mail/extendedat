/*
 * HTTPConfig.cpp
 *
 *  Created on: Mar 21, 2013
 *      Author: ia64domain
 */

#include "HTTPConfig.h"

using namespace std;

const char HTTPConfig::bearerProfileParamName[] = "CID";
const char HTTPConfig::urlParamName[] = "URL";
const char HTTPConfig::userAgentParamName[] = "UA";
const char HTTPConfig::proxyIPParamName[] = "PROIP";
const char HTTPConfig::proxyPortParamName[] = "PROPORT";
const char HTTPConfig::redirectionParamName[] = "REDIR";
const char HTTPConfig::breakStartPosParamName[] = "BREAK";
const char HTTPConfig::breakEndPosParamName[] = "BREAKEND";
const char HTTPConfig::timeoutParamName[] = "TIMEOUT";
const char HTTPConfig::contentTypeParamName[] = "CONTENT";

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

void HTTPConfig::setUrl(const char url[URL_MAX_LENGHT]) {
	strcpy(this->url, url);
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

const char * const HTTPConfig::getBearerProfileParamName() const {
	return HTTPConfig::bearerProfileParamName;
}

const char * const HTTPConfig::getUrlParamName() const {
	return HTTPConfig::urlParamName;
}

const char * const HTTPConfig::getUserAgentParamName() const {
	return HTTPConfig::userAgentParamName;
}

const char * const HTTPConfig::getProxyIPParamName() const {
	return HTTPConfig::proxyIPParamName;
}

const char * const HTTPConfig::getProxyPortParamName() const {
	return HTTPConfig::proxyPortParamName;
}

const char * const HTTPConfig::getRedirectParamName() const {
	return HTTPConfig::redirectionParamName;
}

const char * const HTTPConfig::getBreakStartPosParamName() const {
	return HTTPConfig::breakStartPosParamName;
}

const char * const HTTPConfig::getBreakEndPosParamName() const {
	return HTTPConfig::breakEndPosParamName;
}

const char * const HTTPConfig::getTimeoutParamName() const {
	return HTTPConfig::timeoutParamName;
}

const char * const HTTPConfig::getContentTypeParamName() const {
	return HTTPConfig::contentTypeParamName;
}

const bool HTTPConfig::isEnabledRedirection() const {
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

int HTTPConfig::compareWith(const HTTPConfig &config, HTTPCONFIG_CHANGES &changes) const {
	int nChanges = 0;

	while(true) {
		if((changes.bearerProfileChanged = (getBearerProfileID() == config.getBearerProfileID())) == true) {
			nChanges++;;
		}

		if((changes.urlChanged = (strcmp(getUrl(), config.getUrl()) == 0)) == true) {
			nChanges++;
		}

		if((changes.userAgentChanged = (strcmp(getUserAgent(), config.getUserAgent()) == 0)) == true) {
			nChanges++;
		}

		if((changes.proxyIPChanged = (strcmp(getProxyIP(), config.getProxyIP()) == 0)) == true) {
			nChanges++;
		}

		if((changes.proxyPortChanged = (getProxyPort() == config.getProxyPort())) == true) {
			nChanges++;
		}

		if((changes.redirectionChanged = (isEnabledRedirection() == config.isEnabledRedirection())) == true) {
			nChanges++;
		}

		if((changes.breakStartPosChanged = (getBreakStartPos() == config.getBreakStartPos())) == true) {
			nChanges++;
		}

		if((changes.breakEndPosChanged = (getBreakEndPos() == config.getBreakEndPos())) == true) {
			nChanges++;
		}

		if((changes.timeoutChanged = (getTimeout() == config.getTimeout())) == true) {
			nChanges++;
		}

		if((changes.contentTypeChanged = (strcmp(getContentType(), config.getContentType()) == 0)) == true) {
			nChanges++;
		}
	}

	return nChanges;
}
