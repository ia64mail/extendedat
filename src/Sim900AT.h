/*
 * Sim900AT.h
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */
#include <iostream>
#include <regex.h>
#include <cstdlib>
#include "PortIO.h"

#ifndef SIM900AT_H_
#define SIM900AT_H_

#define LOGGING

#ifdef LOGGING
#define MAX_ERROR_MSG 0x1000
#endif

typedef enum {
	DCE_FAIL, DCE_OK, DCE_ERROR
} COMMON_AT_RESULT;


typedef enum {
	SIM_FAIL, SIM_READY, SIM_PIN_REQUIRED, SIM_PUK_REQUIRED,
	SIM_PIN2_REQUIRED, SIM_PUK2_REQUIRED,
	DCE_SIM_REQUIRED, DCE_PUK_REQUIRED
} SIMCARD_STATE;


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
};

#endif /* SIM900AT_H_ */
