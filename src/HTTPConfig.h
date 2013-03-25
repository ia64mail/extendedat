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

#define URL_MAX_LENGHT 257 /*max length of URL parameter value (include terminate key code)*/
#define USER_AGENT_MAX_LENGHT 101 /*max length of USER AGENT parameter value (include terminate key code)*/
#define CONTENT_TYPE_MAX_LENGHT 101 /*max length of CONTENT TYPE parameter value (include terminate key code)*/
#define IP_MAX_LENGHT 16 /*max length of PROXY IP parameter value (include terminate key code)*/

#define DEFAULT_USER_AGENT "unknown" /*default value for user agent*/
#define DEFAULT_PROXY_IP "0.0.0.0" /*default proxy IP address*/
#define DEFAULT_PROXY_PORT 0 /*default proxy port*/
#define DEFAULT_REDIRECTION false /*default redirection state*/
#define DEFAULT_TIMEOUT 120 /*default timeout*/
#define DEFAULT_CONTENT_TYPE "text/html" /*default content type*/

/**
 * Structure for tracking HTTPConfig changes.
 * Each field indicate change status.
 */
struct HTTPCONFIG_CHANGES {
	bool bearerProfileChanged;
	bool urlChanged;
	bool userAgentChanged;
	bool proxyIPChanged;
	bool proxyPortChanged;
	bool redirectionChanged;
	bool breakStartPosChanged;
	bool breakEndPosChanged;
	bool timeoutChanged;
	bool contentTypeChanged;

	public:
	HTTPCONFIG_CHANGES() {
		bearerProfileChanged = true;
		urlChanged = true;
		userAgentChanged = true;
		proxyIPChanged = true;
		proxyPortChanged = true;
		redirectionChanged = true;
		breakStartPosChanged = true;
		breakEndPosChanged = true;
		timeoutChanged = true;
		contentTypeChanged = true;
	}
};

/**
 * This class represent configuration information
 * for setup http-action context.
 */
class HTTPConfig {
private:
	/**
	 * Block of configuration parameter names.
	 */
	static const char bearerProfileParamName[4];
	static const char urlParamName[4];
	static const char userAgentParamName[3];
	static const char proxyIPParamName[6];
	static const char proxyPortParamName[8];
	static const char redirectionParamName[6];
	static const char breakStartPosParamName[6];
	static const char breakEndPosParamName[9];
	static const char timeoutParamName[8];
	static const char contentTypeParamName[8];

	unsigned int bearerProfileID;
	char url[URL_MAX_LENGHT];
	char userAgent[USER_AGENT_MAX_LENGHT];
	char proxyIP[IP_MAX_LENGHT];
	unsigned int proxyPort;
	bool enableRedirection;
	unsigned int breakStartPos;
	unsigned int breakEndPos;
	unsigned int timeout;
	char contentType[CONTENT_TYPE_MAX_LENGHT];
public:
	HTTPConfig(const unsigned int bearerProfileID = 0, const char url[URL_MAX_LENGHT] = "");
	virtual ~HTTPConfig();

	void setBearerProfileID(const char id);
	void setUrl(const char url[URL_MAX_LENGHT]);
	void setUserAgent(const char userAgent[USER_AGENT_MAX_LENGHT]);
	void setProxy(const char proxyIP[IP_MAX_LENGHT], unsigned int proxyPort);
	void setRedirection(const bool enableRedirection);
	void setBreakState(const unsigned int breakStartPos, const unsigned int breakEndPos);
	unsigned int setTimeout(const unsigned int timeout);
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

	const char * const getBearerProfileParamName() const;
	const char * const getUrlParamName() const;
	const char * const getUserAgentParamName() const;
	const char * const getProxyIPParamName() const;
	const char * const getProxyPortParamName() const;
	const char * const getRedirectParamName() const;
	const char * const getBreakStartPosParamName() const;
	const char * const getBreakEndPosParamName() const;
	const char * const getTimeoutParamName() const;
	const char * const getContentTypeParamName() const;

	const bool isEnabledRedirection() const;
	const bool isEnabledProxy() const;
	const bool isEnabledBreakState() const;

	int compareWith(const HTTPConfig &config, HTTPCONFIG_CHANGES &changes) const;
};

#endif /* HTTPCONFIG_H_ */
