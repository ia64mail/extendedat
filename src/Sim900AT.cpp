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
			cout << "... [Warning] No matches found in \"";
			Util::writeEncoded(regex_text);
			cout  << "\" " << endl;
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

	const char * command = "AT\r";
	const char responceSize = 7 + MEE_OFFSET;
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

/**
 *
 */
SIMCARD_STATE Sim900AT::checkSimCardLockState(){
	resetLastMobileEquipmentErrorStatus();
	SIMCARD_STATE dceResult = SIM_FAIL;

	const char * command = "AT+CPIN?\r";
	const char responceSize = 25 + MEE_OFFSET;
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

/**
 *
 */
COMMON_AT_RESULT Sim900AT::unlockSimCard(const char * const password) {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	char commandTemplate[] = "AT+CPIN=%s\r";
	char * command = new char[sizeof(commandTemplate) + 4];
	const char responceSize = 7  + MEE_OFFSET;
	bool resFlag = true;

	sprintf(command, commandTemplate, password);
	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;

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

/**
 *
 */
CALL_STATE Sim900AT::startCall(const char * const phoneNumber, const bool isVoice) {
	resetLastMobileEquipmentErrorStatus();
	CALL_STATE callResult = CALL_FAIL;

	char semicolon = ';';
	if(!isVoice) {
		semicolon = ' ';
	}

	char commandTemplate[] = "ATD%s%c\r";
	char * command = new char[sizeof(commandTemplate) + 14];
	const char responceSize = 16  + MEE_OFFSET;
	bool resFlag = true;

	sprintf(command, commandTemplate, phoneNumber, semicolon);
	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;

	//send fail
	if(!resFlag) {
		return callResult;
	}

	char * const responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	//receive fail
	if(!resFlag) {
		delete [] responce;
		return callResult;
	}

	char n_matches = 2;
	regmatch_t * matches = new regmatch_t[n_matches];
	resFlag &= (match_regex("^\r\n([[:alnum:][:space:]]{2,11})\r\n$", responce, n_matches, matches) == 0);

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		delete [] matches;
		return callResult;
	}

	while(1) {
		if(strncmp("OK", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			callResult = CALL_CONNECT_VOICE;
			break;
		}

		if(strncmp("NO DIALTONE", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			callResult = CALL_NO_DIALTONE;
			break;
		}

		if(strncmp("BUSY", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			callResult = CALL_BUSY;
			break;
		}

		if(strncmp("NO CARRIER", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			callResult = CALL_NO_CARRIER;
			break;
		}

		if(strncmp("NO ANSWER", responce + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so) == 0) {
			callResult = CALL_NO_ANSWER;
			break;
		}

		break;
	}

	delete [] responce;
	delete [] matches;
	return callResult;
}

/**
 *
 */
int Sim900AT::getListCurrentCalls(CALL_DETAILS * const details, const int &size) {
	resetLastMobileEquipmentErrorStatus();

	const char * command = "AT+CLCC\r";
	const int responceSize = (sizeof(CALL_DETAILS) + 15) * size + 6 + MEE_OFFSET;
	bool resFlag = true;

	resFlag &= (portIO->sendUART(command) > 0);

	//send fail
	if(!resFlag) {
		return -1;
	}

	char * const responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	//receive fail
	if(!resFlag) {
		delete [] responce;
		return -1;
	}

	char n_matches = 10;
	const char * regex_text = "\r\n\\+CLCC:\\s([[:digit:]]+),([[:digit:]]+),([[:digit:]]+),([[:digit:]]+),([[:digit:]]+)"
			"(,\"([[:alnum:]]+)\",([[:digit:]]+),\"([[:digit:]]*)\")?\r\n";
	const char * responcePointer = responce;
	int i = 0;
	bool hasCallRecord = true;
	while(hasCallRecord) {
		regmatch_t * matches = new regmatch_t[n_matches];
		hasCallRecord &= (match_regex(regex_text, responcePointer, n_matches, matches) == 0);

		//if has record about call state and has space for record info, parse it
		if(hasCallRecord && i < size) {
			details[i].callID = strtol(responce + matches[1].rm_so, NULL, 10);

			int intVal = strtol(responce + matches[2].rm_so, NULL, 10);
			switch(intVal) {
			case 0:
				details[i].direction = CALLDIRECTION_OUT;
				break;
			case 1:
				details[i].direction = CALLDIRECTION_IN;
				break;
			}

			intVal = strtol(responce + matches[3].rm_so, NULL, 10);
			switch(intVal) {
			case 0:
				details[i].status = CALLSTATUS_ACTIVE;
				break;
			case 1:
				details[i].status = CALLSTATUS_HELD;
				break;
			case 2:
				details[i].status = CALLSTATUS_DIALING;
				break;
			case 3:
				details[i].status = CALLSTATUS_ALERTING;
				break;
			case 4:
				details[i].status = CALLSTATUS_INCOMING;
				break;
			case 5:
				details[i].status = CALLSTATUS_WAITING;
				break;
			case 6:
				details[i].status = CALLSTATUS_DISCONNECT;
				break;
			}

			intVal = strtol(responce + matches[4].rm_so, NULL, 10);
			switch(intVal) {
			case 0:
				details[i].mode = CALLMODE_VOICE;
				break;
			case 1:
				details[i].mode = CALLMODE_DATA;
				break;
			case 2:
				details[i].mode = CALLMODE_FAX;
				break;
			}

			details[i].isConference = strtol(responce + matches[5].rm_so, NULL, 10);

			int callNumberSize = matches[7].rm_eo - matches[7].rm_so;
			if(callNumberSize > 0) {
				strncpy(details[i].callOriginalNumber,responce + matches[7].rm_so, callNumberSize);
				details[i].callOriginalNumber[callNumberSize + 1] = CHAR_TR;

				int callNumberTypeVal = strtol(responce + matches[8].rm_so, NULL, 10);
				switch(callNumberTypeVal) {
				case 161:
					details[i].callNumberType = CALLEDPARTY_NATIOANL;
					break;
				case 145:
					details[i].callNumberType = CALLEDPARTY_INTERNATIONAL;
					break;
				case 177:
					details[i].callNumberType = CALLEDPARTY_NETWORK_SPECIFIC;
					break;
				case 193:
					details[i].callNumberType = CALLEDPARTY_DEDICATED;
					break;
				case 129:
					details[i].callNumberType = CALLEDPARTY_UNKNOWN;
					break;
				default:
					details[i].callNumberType = CALLEDPARTY_UNKNOWN;
					break;
				}

				details[i].adressBookId = strtol(responce + matches[9].rm_so, NULL, 10);
			}
		}

		//if has record about call state, move pointer to try find next record in response
		if(hasCallRecord) {
			responcePointer = responcePointer + matches[0].rm_eo;
			i++;
		}

		delete [] matches;
	}

	//any call state record found
	if(i==0) {
		char n_matches = 2;
		regmatch_t * matches = new regmatch_t[n_matches];
		resFlag &= (match_regex("^\r\nOK\r\n$", responce, n_matches, matches) == 0);
		delete [] matches;
	}

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		return -1;
	}

	delete [] responce;
	return i;
}

/**
 *
 */
COMMON_AT_RESULT  Sim900AT::hangUpCall(const HANGUP_MODE mode) {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	char commandTemplate[] = "ATH%u\r";
	char * command = new char[sizeof(commandTemplate)];
	const char responceSize = 7 + MEE_OFFSET;
	bool resFlag = true;

	int modeVal;
	switch(mode) {
	case HANGUP_ALL: modeVal = 0;
	break;
	case HANGUP_ALL_AND_CLEAN:
		modeVal = 1;
		break;
	case HANGUP_CSD_CALLS:
		modeVal = 2;
		break;
	case HANGUP_GPRS_CALLS:
		modeVal = 3;
		break;
	case HANGUP_CS_CALLS_EXCLUDE_ONHOLD:
		modeVal = 4;
		break;
	case HANGUP_ONHOLD_CALLS:
		modeVal = 5;
		break;
	default:
		modeVal = 0;
		break;
	}

	sprintf(command, commandTemplate, modeVal);
	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;

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

COMMON_AT_RESULT Sim900AT::definePaketDataProtocolContextProfile(const PDP_CONTEXT_DETAILS &details) {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	const char * commandTemplate = "AT+CGDCONT=%u,\"IP\",%s,%s,%u,%u\r";
	if(details.ipAddress == PDP_CONTEXT_AUTO_IP) {
		commandTemplate = "AT+CGDCONT=%u,\"IP\",%s\r";
	} else if (details.dataCompresionLevel == PDP_CONTEXT_AUTO_COMPRESSION_LEVEL) {
		commandTemplate = "AT+CGDCONT=%u,\"IP\",%s,%s\r";
	} else if (details.headerCompresionLevel == PDP_CONTEXT_AUTO_COMPRESSION_LEVEL) {
		commandTemplate = "AT+CGDCONT=%u,\"IP\",%s,%s,%u\r";
	}

	char * command = new char[25 + sizeof(details)];
	const char responceSize = 7 + MEE_OFFSET;
	bool resFlag = true;

	sprintf(command, commandTemplate, details.contextProfileID, details.accessPointName,
			details.ipAddress, details.dataCompresionLevel, details.headerCompresionLevel);
	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;

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

COMMON_AT_RESULT Sim900AT::setIPBearerParameters(const BEARER_PARAMETER_DETAILS &details) {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	char commandTemplate[] = "AT+SAPBR=3,%u,%s,%s\r";
	char * command = new char[sizeof(commandTemplate) + sizeof(details)];
	const char responceSize = 20 + MEE_OFFSET;
	bool resFlag = true;

	const char * paramNameVal;
	switch(details.paramName) {
	case BEARER_PARAM_CONTYPE:
		paramNameVal = "CONTYPE";
		break;
	case BEARER_PARAM_APN:
		paramNameVal = "APN";
		break;
	case BEARER_PARAM_USER:
		paramNameVal = "USER";
		break;
	case BEARER_PARAM_PWD:
		paramNameVal = "PWD";
		break;
	case BEARER_PARAM_PHONENUM:
		paramNameVal = "PHONENUM";
		break;
	case BEARER_PARAM_RATE:
		paramNameVal = "RATE";
		break;
	}

	sprintf(command, commandTemplate, details.bearerProfileID, paramNameVal, details.paramValue);
	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;

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

COMMON_AT_RESULT Sim900AT::changeStateIPBearer(const unsigned int &bearerProfileID, const unsigned int changeStateCode) {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	char commandTemplate[] = "AT+SAPBR=%u,%u\r";
	char * command = new char[sizeof(commandTemplate) + 2];
	const char responceSize = 20 + MEE_OFFSET;
	bool resFlag = true;

	sprintf(command, commandTemplate, changeStateCode, bearerProfileID);
	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;

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

COMMON_AT_RESULT Sim900AT::openIPBearer(const unsigned int &bearerProfileID) {
	return changeStateIPBearer(bearerProfileID, 1);
}

COMMON_AT_RESULT Sim900AT::closeIPBearer(const unsigned int &bearerProfileID) {
	return changeStateIPBearer(bearerProfileID, 0);
}

COMMON_AT_RESULT Sim900AT::getIPBearerState(const unsigned int &bearerProfileID, BEARER_STATUS &status){
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	char commandTemplate[] = "AT+SAPBR=2,%u\r";
	char * command = new char[sizeof(commandTemplate) + 2];
	const char responceSize = 30 + MEE_OFFSET;
	bool resFlag = true;

	sprintf(command, commandTemplate, bearerProfileID);
	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;

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

	char n_matches = 4;
	regmatch_t * matches = new regmatch_t[n_matches];
	resFlag &= (match_regex("^\r\n\\+SAPBR:\\s([[:digit:]]+),([[:digit:]]{1}),\"([[:digit:][:d:]]{7,15})\"\r\nOK\r\n$", responce, n_matches, matches) == 0);

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		delete [] matches;
		return dceResult;
	}

	status.bearerProfileID = strtol(responce + matches[1].rm_so, NULL, 10);

	int modeVal = strtol(responce + matches[2].rm_so, NULL, 10);
	switch(modeVal) {
	case 0:
		status.mode = BEARER_CONNECTING;
		break;
	case 1:
		status.mode = BEARER_CONNECTED;
		break;
	case 2:
		status.mode = BEARER_CLOSING;
		break;
	case 3:
		status.mode = BEARER_CLOSED;
		break;
	}

	char ipSize = matches[3].rm_eo - matches[3].rm_so;
	strncpy(status.ipAddress, responce + matches[3].rm_so, ipSize);
	status.ipAddress[ipSize + 1] = CHAR_TR;

	delete [] responce;
	delete [] matches;
	return dceResult;
}

COMMON_AT_RESULT Sim900AT::initialiseHTTP(){
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	const char * command = "AT+HTTPINIT\r";
	const char responceSize = 7 + MEE_OFFSET;
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

	char n_matches = 1;
	regmatch_t * matches = new regmatch_t[n_matches];
	resFlag &= (match_regex("^\r\nOK\r\n$", responce, n_matches, matches) == 0);

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		delete [] matches;
		return dceResult;
	}

	dceResult = DCE_OK;

	delete [] responce;
	delete [] matches;
	return dceResult;
}

COMMON_AT_RESULT Sim900AT::terminateHTTP() {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	const char * command = "AT+HTTPTERM\r";
	const char responceSize = 7 + MEE_OFFSET;
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

	char n_matches = 1;
	regmatch_t * matches = new regmatch_t[n_matches];
	resFlag &= (match_regex("^\r\nOK\r\n$", responce, n_matches, matches) == 0);

	//answer decode fail
	if(!resFlag) {
		updateLastMobileEquipmentErrorStatus(responce);

		delete [] responce;
		delete [] matches;
		return dceResult;
	}

	dceResult = DCE_OK;

	delete [] responce;
	delete [] matches;
	return dceResult;
}

COMMON_AT_RESULT Sim900AT::configureHTTP(const HTTPConfig &config) {
	resetLastMobileEquipmentErrorStatus();
	COMMON_AT_RESULT dceResult = DCE_FAIL;

	return dceResult;
}
