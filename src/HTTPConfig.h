/*
 * HTTPConfig.h
 *
 *  Created on: Mar 21, 2013
 *      Author: ia64domain
 */

#include <iostream>
#include <regex.h>
#include <cstdlib>
#include <cstring>

#ifndef HTTPCONFIG_H_
#define HTTPCONFIG_H_

#define URL_MAX_LENGHT 256
#define USER_AGENT_MAX_LENGHT 100
#define CONTENT_TYPE_MAX_LENGHT 100

class HTTPConfig {
private:
	unsigned int bearerProfileID;
	char url[URL_MAX_LENGHT];
	char userAgent[USER_AGENT_MAX_LENGHT];
	char proxyIP[16];
	unsigned int proxyPort;
	bool enableRedirection;
	unsigned int breakStartPos;
	unsigned int breakEndPos;
	unsigned int timeout;
	char contentType[CONTENT_TYPE_MAX_LENGHT];
public:
	HTTPConfig(const unsigned int bearerProfileID, const char url[URL_MAX_LENGHT]);
	virtual ~HTTPConfig();

	void setUserAgent(const char userAgent[USER_AGENT_MAX_LENGHT]);
	void setProxy(const char proxyIP[16], unsigned int proxyPort);
	void setRedirection(const bool enableRedirection);
	void setBreakState(const unsigned int breakStartPos, const unsigned int breakEndPos);
	int setTimeout(const unsigned int timeout);
	void setContentType(const char contentType[CONTENT_TYPE_MAX_LENGHT]);

	const unsigned int & getBearerProfileID() const;
	const char * const getUrl() const;
	const char * const getUserAgent() const;
	const char * const getProxyIP() const;
	const unsigned int & getProxyPort() const;
	const unsigned int & getBreakStartPos() const;
	const unsigned int & getBreakEndPos() const;
	const unsigned int & getTimeout() const;
	const char * const getContentType() const;

	const bool isEnabledREdirection() const;
	const bool isEnabledProxy() const;
	const bool isEnabledBreakState() const;
};

#endif /* HTTPCONFIG_H_ */
