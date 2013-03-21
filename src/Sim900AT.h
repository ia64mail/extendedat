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
 * Bearer parameter names.
 */
#define BEARER_PARAM_CONTYPE_CSD "CSD" /*CSD connection type*/
#define BEARER_PARAM_CONTYPE_GPRS "GPRS" /*GPRS connection type*/
#define BEARER_PARAM_CSD_SPEED_2400 0 /*2400 b/s*/
#define BEARER_PARAM_CSD_SPEED_4800 1 /*4800 b/s*/
#define BEARER_PARAM_CSD_SPEED_9600 2 /*9600 b/s*/
#define BEARER_PARAM_CSD_SPEED_14400 3 /*14400 b/s*/
typedef enum {
	BEARER_PARAM_CONTYPE, /*connection type, can be one of predefined BEARER_PARAM_CONTYPE_XXX*/
	BEARER_PARAM_APN, /*logical name of GPRS Gateway Service Node*/
	BEARER_PARAM_USER, /*user name*/
	BEARER_PARAM_PWD, /*user password*/
	BEARER_PARAM_PHONENUM, /*phone number for CSD call*/
	BEARER_PARAM_RATE /*connection rate for CSD, can be one of predefined BEARER_PARAM_CSD_SPEED_XXXX*/
} BEARER_PARAM_NAME;

/**
 * Detailed information about bearer parameters
 */
typedef struct {
	int bearerProfileID; /*unique identifier for bearer profile*/
	BEARER_PARAM_NAME paramName; /*logical name of GPRS Gateway Service Node*/
	char paramValue[50]; /*IP address of DCE for this profile*/
} BEARER_PARAMETER_DETAILS;

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
	COMMON_AT_RESULT definePaketDataProtocolContextProfile(const PDP_CONTEXT_DETAILS &details);
	COMMON_AT_RESULT setIPBearerParameters(const BEARER_PARAMETER_DETAILS &details);
	COMMON_AT_RESULT openIPBearer(const unsigned int &bearerProfileID);
	COMMON_AT_RESULT closeIPBearer(const unsigned int &bearerProfileID);
	COMMON_AT_RESULT getIPBearerState(const unsigned int &bearerProfileID, BEARER_STATUS &status);
	COMMON_AT_RESULT initialiseHTTP();
	COMMON_AT_RESULT terminateHTTP();
	COMMON_AT_RESULT configureHTTP(const HTTPConfig &config);
};

#endif /* SIM900AT_H_ */
