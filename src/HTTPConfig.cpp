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

/**
 * Default constructor.
 * The mandatory fields bearerProfileID and url
 * should be initialized. Other fields will be
 * initialized with default values.
 */
HTTPConfig::HTTPConfig(const unsigned int bearerProfileID, const char url[URL_MAX_LENGHT]) {
	this->bearerProfileID = bearerProfileID;
	strcpy(this->url, url);

	setUserAgent(DEFAULT_USER_AGENT);
	setProxy(DEFAULT_PROXY_IP, DEFAULT_PROXY_PORT);
	setRedirection(DEFAULT_REDIRECTION);
	setBreakState(0,0);
	setTimeout(DEFAULT_TIMEOUT);
	setContentType(DEFAULT_CONTENT_TYPE);
}

HTTPConfig::~HTTPConfig() {
}

/**
 * Set profile ID used for this HTTP session.
 */
void HTTPConfig::setBearerProfileID(const char id){
	this->bearerProfileID = id;
}

/**
 * Set URL used for this HTTP session.
 */
void HTTPConfig::setUrl(const char url[URL_MAX_LENGHT]) {
	strcpy(this->url, url);
}

/**
 * Set user agent identification used for this HTTP session.
 */
void HTTPConfig::setUserAgent(const char userAgent[USER_AGENT_MAX_LENGHT]) {
	strcpy(this->userAgent, userAgent);
}

/**
 * Set proxy IP if required for this HTTP session.
 */
void HTTPConfig::setProxy(const char proxyIP[IP_MAX_LENGHT], unsigned int proxyPort) {
	strcpy(this->proxyIP, proxyIP);
	this->proxyPort = proxyPort;
}

/**
 * Set proxy port if required for this HTTP session.
 */
void HTTPConfig::setRedirection(const bool enableRedirection) {
	this->enableRedirection = enableRedirection;
}

/**
 * Set break state used for this HTTP session.
 * This parameters used for HTTP method "GET" and allow
 * resuming broken transfer.
 * If the value of breakEndPos is bigger than breakStartPos,
 * the transfer scope is from breakStartPos to breakEndPos.
 * If the value of breakEndPos is smaller than breakStartPos,
 * the transfer scope is from breakStartPos to the end of the file.
 * If both breakStartPos and breakEndPos are 0, the
 * resume broken transfer function is disabled.
 *
 * Not all the HTTP Server supports "BREAK" and "BREAKEND" parameters!
 */
void HTTPConfig::setBreakState(const unsigned int breakStartPos, const unsigned int breakEndPos) {
	this->breakStartPos = breakStartPos;
	this->breakEndPos = breakEndPos;
}

/**
 * Set HTTP session timeout in seconds. Scope: 30-1000s.
 */
unsigned int HTTPConfig::setTimeout(const unsigned int timeout) {
	if(timeout >= 30 || timeout <= 1000) {
		this->timeout = timeout;
	}
	return this->timeout;
}

/**
 * Set content type header value used for this HTTP session.
 */
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
	if(strcmp(DEFAULT_PROXY_IP, getProxyIP()) == 0 && DEFAULT_PROXY_PORT == getProxyPort()) {
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

/**
 * Compare this configuration with parameter specified one
 * and result of comparison store into HTTPCONFIG_CHANGES structure.
 *
 * Return count of not equal properties.
 */
int HTTPConfig::compareWith(const HTTPConfig &config, HTTPCONFIG_CHANGES &changes) const {
	int nChanges = 0;

	if((changes.bearerProfileChanged = (getBearerProfileID() != config.getBearerProfileID())) == true) {
		nChanges++;;
	}

	if((changes.urlChanged = (strcmp(getUrl(), config.getUrl()) != 0)) == true) {
		nChanges++;
	}

	if((changes.userAgentChanged = (strcmp(getUserAgent(), config.getUserAgent()) != 0)) == true) {
		nChanges++;
	}

	if((changes.proxyIPChanged = (strcmp(getProxyIP(), config.getProxyIP()) != 0)) == true) {
		nChanges++;
	}

	if((changes.proxyPortChanged = (getProxyPort() != config.getProxyPort())) == true) {
		nChanges++;
	}

	if((changes.redirectionChanged = (isEnabledRedirection() != config.isEnabledRedirection())) == true) {
		nChanges++;
	}

	if((changes.breakStartPosChanged = (getBreakStartPos() != config.getBreakStartPos())) == true) {
		nChanges++;
	}

	if((changes.breakEndPosChanged = (getBreakEndPos() != config.getBreakEndPos())) == true) {
		nChanges++;
	}

	if((changes.timeoutChanged = (getTimeout() != config.getTimeout())) == true) {
		nChanges++;
	}

	if((changes.contentTypeChanged = (strcmp(getContentType(), config.getContentType()) != 0)) == true) {
		nChanges++;
	}

	return nChanges;
}
