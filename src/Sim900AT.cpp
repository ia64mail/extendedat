/*
 * Sim900AT.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */

#include "Sim900AT.h"

Sim900AT::Sim900AT(PortIO * portIO) {
	this->portIO = portIO;
	this->lastCMEError = 0;
}

Sim900AT::~Sim900AT() {

}

int Sim900AT::match_regex(const char * const regex_text, const char * const to_match,
		const int n_matches, regmatch_t * const matches) const {

	using namespace std;
	regex_t * regexPointer = new regex_t;

	//compile regex text into regex_t structure
	int status = regcomp(regexPointer, regex_text, REG_EXTENDED | REG_NEWLINE | REG_ICASE);

	if (status != 0) {
		#ifdef LOGGING
			char * error_message = new char[MAX_ERROR_MSG];
			regerror (status, regexPointer, error_message, MAX_ERROR_MSG);
			cout << "... [Error] Regex error compiling in \"" << regex_text << "\" " << endl;
			cout << "... caused by " << error_message << endl;
			delete [] error_message;
		#endif
		delete regexPointer;

		return 1;
	}

	/* search regex_text pattern into to_match;
	 * first occurrence of search pattern rp inside string to_match
	 * matching whole pattern placed into matches[0] element
	 * other occurrence of search pattern part in ()
	 * placed into matches[1+] elements.
	 */
	int nomatch = regexec(regexPointer, to_match, n_matches, matches, 0);

	if (nomatch != 0) {
		#ifdef LOGGING
			char * error_message = new char[MAX_ERROR_MSG];
			regerror (status, regexPointer, error_message, MAX_ERROR_MSG);
			cout << "... [Warning] No matches found in \"" << regex_text << "\" " << endl;
			cout << "... caused by " << error_message << endl;
			delete [] error_message;
		#endif
		delete regexPointer;

		return nomatch;
	}

	delete regexPointer;
	return 0;
}

void Sim900AT::resetLastMobileEquipmentErrorStatus() {
		lastCMEError = 0;
}

void Sim900AT::updateLastMobileEquipmentErrorStatus(const char * const responce) {
	char n_matches = 2;
	regmatch_t * matches = new regmatch_t[n_matches];

	if (match_regex("^\r\n\\+CME\\sERROR:\\s([[:digit:]]+)\r\n$", responce, n_matches, matches) == 0) {
		lastCMEError = strtol(responce + matches[1].rm_so, NULL, 10);
	}

	delete [] matches;
}

unsigned short Sim900AT::getLastMobileEquipmentErrorStatus() const {
	return lastCMEError;
}

/**
 * Send simple AT command and ensure,
 * that response is OK
 */
COMMON_AT_RESULT Sim900AT::testAT(){
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	char * command = "AT\r";
	const char responceSize = 7;
	bool resFlag = true;

	resFlag &= (portIO->sendUART(command) > 0);

	//send fail
	if(!resFlag) {
		return dceResult;
	}

	char * const responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	//receive fail
	if(!resFlag) {
		delete [] responce;
		return dceResult;
	}

	char n_matches = 2;
	regmatch_t * matches = new regmatch_t[n_matches];
	resFlag &= (match_regex("^\r\n(\\w{2,5})\r\n$", responce, n_matches, matches) == 0);

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		delete [] matches;
		return dceResult;
	}

	while(1) {
		if(strncmp("OK", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = DCE_OK;
			break;
		}

		if(strncmp("ERROR", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = DCE_ERROR;
			break;
		}

		break;
	}

	delete [] responce;
	delete [] matches;
	return dceResult;
}

SIMCARD_STATE Sim900AT::checkSimCardLockState(){
	resetLastMobileEquipmentErrorStatus();
	SIMCARD_STATE dceResult = SIM_FAIL;

	char * command = "AT+CPIN?\r";
	const char responceSize = 25;
	bool resFlag = true;

	resFlag &= (portIO->sendUART(command) > 0);

	//send fail
	if(!resFlag) {
		return dceResult;
	}

	char * const responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	//receive fail
	if(!resFlag) {
		delete [] responce;
		return dceResult;
	}

	char n_matches = 2;
	regmatch_t * matches = new regmatch_t[n_matches];
	resFlag &= (match_regex("^\r\n\\+CPIN:\\s([[:alnum:][:space:]]{5,10})\r\n\r\nOK\r\n$", responce, n_matches, matches) == 0);

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		delete [] matches;
		return dceResult;
	}

	while(1) {
		if(strncmp("READY", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = SIM_READY;
			break;
		}

		if(strncmp("SIM PIN", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = SIM_PIN_REQUIRED;
			break;
		}

		if(strncmp("SIM PUK", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = SIM_PUK_REQUIRED;
			break;
		}

		if(strncmp("PH_SIM PIN", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = DCE_SIM_REQUIRED;
			break;
		}

		if(strncmp("PH_SIM PUK", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = DCE_PUK_REQUIRED;
			break;
		}

		if(strncmp("SIM PIN2", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = SIM_PIN2_REQUIRED;
			break;
		}

		if(strncmp("SIM PUK2", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = SIM_PUK2_REQUIRED;
			break;
		}

		break;
	}

	delete [] responce;
	delete [] matches;
	return dceResult;
}

COMMON_AT_RESULT Sim900AT::unlockSimCard(const char * const password) {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	char commandTemplate[] = "AT+CPIN=%s\r";
	char * command = new char[sizeof(commandTemplate) + 4];
	const char responceSize = 25;
	bool resFlag = true;

	sprintf(command, commandTemplate, password);
	resFlag &= (portIO->sendUART(command) > 0);

	//send fail
	if(!resFlag) {
		return dceResult;
	}

	char * const responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	//receive fail
	if(!resFlag) {
		delete [] responce;
		return dceResult;
	}

	char n_matches = 2;
	regmatch_t * matches = new regmatch_t[n_matches];
	resFlag &= (match_regex("^\r\n(\\w{2})\r\n$", responce, n_matches, matches) == 0);

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		delete [] matches;
		return dceResult;
	}

	while(1) {
		if(strncmp("OK", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			dceResult = DCE_OK;
			break;
		}

		break;
	}

	delete [] responce;
	delete [] matches;
	return dceResult;
}

