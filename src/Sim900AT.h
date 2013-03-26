/*
 * Sim900AT.h
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */
#include <iostream>
#include <regex.h>
#include <cstdlib>
#include "Config.h"
#include "Util.h"
#include "PortIO.h"
#include "HTTPConfig.h"
#include "BearerConfig.h"

#ifndef SIM900AT_H_
#define SIM900AT_H_

#ifdef LOGGING
#define MAX_ERROR_MSG 0x1000
#endif

/**
 * Common DCE statuses
 */
typedef enum {
	DCE_FAIL, DCE_OK, DCE_ERROR
} COMMON_AT_RESULT;

/**
 * Simcard operation statuses
 */
typedef enum {
	SIM_FAIL, SIM_READY, SIM_PIN_REQUIRED, SIM_PUK_REQUIRED,
	SIM_PIN2_REQUIRED, SIM_PUK2_REQUIRED,
	DCE_SIM_REQUIRED, DCE_PUK_REQUIRED
} SIMCARD_STATE;

/**
 * Calling statuses during mobile originated call
 */
typedef enum {
	CALL_FAIL, CALL_CONNECT_DATA, CALL_CONNECT_VOICE,
	CALL_BUSY, CALL_NO_CARRIER, CALL_NO_DIALTONE, CALL_NO_ANSWER
} CALL_STATE;

/**
 * Call direction criterion
 */
typedef enum {
	CALLDIRECTION_IN, CALLDIRECTION_OUT
} CALL_DIRECTION_CRITERION;

/**
 * Call status criterion during ME examination
 */
typedef enum {
	CALLSTATUS_ACTIVE, CALLSTATUS_HELD, CALLSTATUS_DIALING, CALLSTATUS_ALERTING,
	CALLSTATUS_INCOMING, CALLSTATUS_WAITING, CALLSTATUS_DISCONNECT
} CALL_STATUS_CRITERION;

/**
 * Call mode criterion
 */
typedef enum {
	CALLMODE_VOICE, CALLMODE_DATA, CALLMODE_FAX
} CALL_MODE_CRITERION;

/**
 * Criterion for called party number
 * Bits
 * 7 6 5
 * 0 0 0 unknown (Note 2)
 * 0 0 1 international number (Note 3, Note 5)
 * 0 1 0 national number (Note 3)
 * 0 1 1 network specific number (Note 4)
 * 1 0 0 dedicated access, short code
 */
typedef enum {
	CALLEDPARTY_UNKNOWN, CALLEDPARTY_INTERNATIONAL, CALLEDPARTY_NATIOANL,
	CALLEDPARTY_NETWORK_SPECIFIC, CALLEDPARTY_DEDICATED
} CALLED_PARTY_CRITERION;

/**
 * Detailed information about current calls
 */
typedef struct {
	int callID;
	CALL_DIRECTION_CRITERION direction;
	CALL_STATUS_CRITERION status;
	CALL_MODE_CRITERION mode;
	bool isConference;
	char callOriginalNumber[15];
	CALLED_PARTY_CRITERION callNumberType;
	int adressBookId;
} CALL_DETAILS;

/**
 * HangUp call modes.
 */
typedef enum {
	HANGUP_ALL, HANGUP_ALL_AND_CLEAN, HANGUP_CSD_CALLS,
	HANGUP_GPRS_CALLS, HANGUP_CS_CALLS_EXCLUDE_ONHOLD,
	HANGUP_ONHOLD_CALLS
} HANGUP_MODE;

/**
 * Detailed information about PacketDataProtocol context settings
 */
#define PDP_CONTEXT_AUTO_IP "0.0.0.0" /*0.0.0.0 dynamic address will be requested*/
#define PDP_CONTEXT_AUTO_COMPRESSION_LEVEL -1 /*default level will be used*/
typedef struct {
	int contextProfileID; /*can be 1 or 2, 3 always defined and locked*/
	char accessPointName[50]; /*logical name of GPRS Gateway Service Node*/
	char ipAddress[16]; /*IP address of DCE for this profile, use PDP_CONTEXT_AUTO_IP as default*/
	int dataCompresionLevel; /*PDP data compression level, 0 - off, use PDP_CONTEXT_AUTO_COMPRESSION_LEVEL as default*/
	int headerCompresionLevel; /*PDP header compression level, 0 - off, , use PDP_CONTEXT_AUTO_COMPRESSION_LEVEL as default*/
} PDP_CONTEXT_DETAILS;

/**
 * Bearer state.
 */
typedef enum {
	BEARER_CONNECTING, BEARER_CONNECTED, BEARER_CLOSING, BEARER_CLOSED
} BEARER_MODE;

/**
 * Detailed information about current bearer status
 */
typedef struct {
	int bearerProfileID; /*unique identifier for bearer profile*/
	BEARER_MODE mode; /*current operation mode*/
	char ipAddress[16]; /*IP address of DCE for this profile*/
} BEARER_STATUS;

/**
 * HTTP action methods.
 */
typedef enum {
	HTTP_ACTION_METHOD_GET, HTTP_ACTION_METHOD_POST, HTTP_ACTION_METHOD_HEAD
} HTTP_ACTION_METHOD;

typedef struct {
	HTTP_ACTION_METHOD method; /*HTTP method type*/
	unsigned short httpResponcecCode; /*HTTP response code*/
	unsigned int size; /*size of requested resource*/
} HTTP_ACTION_STATUS;

/**
 *
 */
class Sim900AT {
private:
	PortIO * portIO;

	/**
	 * Mobile equipment error code.
	 */
	unsigned short lastCMEError;

	/**
	 * Find matches into string.
	 *
	 * @see http://www.cplusplus.com/reference/regex/ECMAScript/
	 */
	int match_regex(const char * const regex_text, const char * const to_match,
			const int n_matches, regmatch_t * const matches) const;

	/**
	 * Reset last response CMEE status.
	 */
	void resetLastMobileEquipmentErrorStatus();

	/**
	 * Update last response CMEE status.
	 */
	void updateLastMobileEquipmentErrorStatus(const char * const responce);

	COMMON_AT_RESULT changeStateIPBearer(const unsigned int &bearerProfileID, const unsigned int changeStateCode);
	COMMON_AT_RESULT setIPBearerParameter(const char * atCommand);
	COMMON_AT_RESULT configureHTTP(const char * atCommand);
	COMMON_AT_RESULT setHTTPContext(const HTTPConfig &config, const HTTPCONFIG_CHANGES changes);
public:
	Sim900AT(PortIO * portIO);
	virtual ~Sim900AT();

	unsigned short getLastMobileEquipmentErrorStatus() const;

	COMMON_AT_RESULT testAT();
	SIMCARD_STATE checkSimCardLockState();
	COMMON_AT_RESULT unlockSimCard(const char * const password);

	CALL_STATE startCall(const char * const phoneNumber, const bool isVoice = false);
	int getListCurrentCalls(CALL_DETAILS * const details, const int &size);
	COMMON_AT_RESULT hangUpCall(const HANGUP_MODE mode = HANGUP_ALL);

	COMMON_AT_RESULT startUSSDCall(const char * const ussdRequest, char * const ussdResponse);

	COMMON_AT_RESULT definePaketDataProtocolContextProfile(const PDP_CONTEXT_DETAILS &details);

	COMMON_AT_RESULT setIPBearerConfig(const BearerConfig &config);
	COMMON_AT_RESULT openIPBearer(const unsigned int &bearerProfileID);
	COMMON_AT_RESULT closeIPBearer(const unsigned int &bearerProfileID);
	COMMON_AT_RESULT getIPBearerState(const unsigned int &bearerProfileID, BEARER_STATUS &status);

	COMMON_AT_RESULT initialiseHTTP();
	COMMON_AT_RESULT terminateHTTP();

	COMMON_AT_RESULT getHTTPContext(HTTPConfig &config);
	COMMON_AT_RESULT initialiseHTTPContext(const HTTPConfig &config);
	COMMON_AT_RESULT updateHTTPContext(const HTTPConfig &config);

	COMMON_AT_RESULT setCurrentHTTPAction(const HTTP_ACTION_METHOD &method, HTTP_ACTION_STATUS &status);
	COMMON_AT_RESULT readHTTPResponse(const int startAdress, const int size, char * const response);
};

#endif /* SIM900AT_H_ */
