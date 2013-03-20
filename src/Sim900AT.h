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
	char callOriginalNumber[10];
	CALLED_PARTY_CRITERION callNumberType;
	int adressBookId;
} CALL_DETAILS;

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
public:
	Sim900AT(PortIO * portIO);
	virtual ~Sim900AT();

	unsigned short getLastMobileEquipmentErrorStatus() const;

	COMMON_AT_RESULT testAT();
	SIMCARD_STATE checkSimCardLockState();
	COMMON_AT_RESULT unlockSimCard(const char * const password);
	CALL_STATE startVoiceCall(const char * const phoneNumber);
	int getListCurrentCalls(CALL_DETAILS * const details, const int &size);
};

#endif /* SIM900AT_H_ */
