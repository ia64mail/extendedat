//============================================================================
// Name        : Sim900.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include "Config.h"
#include "PortIO.h"
#include "Sim900AT.h"

/**
 * Main entry point
 */
int main() {
	using namespace std;

	#ifdef LOGGING
		cout << endl << "Communicating..." << endl;
	#endif

	PortIO * portIO = new PortIO();
	Sim900AT * atProcessor = new Sim900AT(portIO);

	/**
	 * Initialise modem
	 */
	COMMON_AT_RESULT r;
	r= atProcessor->testAT();
	SIMCARD_STATE s = atProcessor->checkSimCardLockState();
	if(s == SIM_PIN_REQUIRED) {
		r = atProcessor->unlockSimCard("0000");
	}

	/**
	 * Test voice call
	 */
/*
	CALL_DETAILS * cd = new CALL_DETAILS[3];
	atProcessor->getListCurrentCalls(cd, 3);
	delete [] cd;

	atProcessor->startCall("0501906337", true);
	sleep(5);

	cd = new CALL_DETAILS[3];
	atProcessor->getListCurrentCalls(cd, 3);
	delete [] cd;

	atProcessor->hangUpCall();
	cd = new CALL_DETAILS[3];
	atProcessor->getListCurrentCalls(cd, 3);
	delete [] cd;
*/

	/**
	 * Test USSD
	 */
/**/
	char ussdResponce[250];
	r = atProcessor->startUSSDCall("*101#", ussdResponce);
	cout << "Balance check response: " << ussdResponce << endl;
	if(r != DCE_OK) {
		return -1;
	}
/**/
	/**
	 * Create GPRS connection for HTTP AT commands
	 */
	BEARER_PARAMETER_DETAILS bearerDetails;
	int bearerId = 1;

	//setup connection type
	bearerDetails.bearerProfileID = bearerId;
	bearerDetails.paramName = BEARER_PARAM_CONTYPE;
	char contype[50] = "GPRS";
	strcpy(bearerDetails.paramValue, contype);

	r = atProcessor->setIPBearerParameters(bearerDetails);
	if(r != DCE_OK) {
		return -1;
	}

	//setup connection AP name
	bearerDetails.bearerProfileID = bearerId;
	bearerDetails.paramName = BEARER_PARAM_APN;
	char apname[50] = "internet";
	strcpy(bearerDetails.paramValue, apname);


	r = atProcessor->setIPBearerParameters(bearerDetails);
	if(r != DCE_OK) {
		return -1;
	}

	//open connection
	sleep(5); /*small delay required!!!*/
	r = atProcessor->openIPBearer(bearerId);
	if(r != DCE_OK) {
		//return -1;
	}

	//check connection state
	BEARER_STATUS bearerStatus;
	int counter = 0;
	while(bearerStatus.mode != BEARER_CONNECTED && counter < 3) {
		r = atProcessor->getIPBearerState(bearerId, bearerStatus);

		cout << "Stating GPRS connection ..." << " bearerProfileID=" << bearerStatus.bearerProfileID
				<< " mode=" << bearerStatus.mode << " IP=" << bearerStatus.ipAddress << endl;

		if(r != DCE_OK) {
			return -1;
		}

		counter++;
	}

	/**
	 * Start HTTP session
	 */
	r = atProcessor->initialiseHTTP();
	if(r != DCE_OK) {
		return -1;
	}

	HTTPConfig oldhttpConfig = HTTPConfig();
	atProcessor->getHTTPContext(oldhttpConfig);

	HTTPConfig httpConfig = HTTPConfig(bearerId, "http://ya.ru/");
	atProcessor->updateHTTPContext(httpConfig);
	if(r != DCE_OK) {
		return -1;
	}

	HTTP_ACTION_STATUS httpActionStatus;
	unsigned char i = 0;
	do {
		r = atProcessor->setCurrentHTTPAction(HTTP_ACTION_METHOD_GET, httpActionStatus);
		if(i > 3) {
			return -1;
		}
		i++;
	} while (r != DCE_OK);

	cout << "HTTP action status ..." << " action=" << httpActionStatus.method << " HTTPCODE=" << httpActionStatus.httpResponcecCode
			<< " size=" << httpActionStatus.size << endl;

	char * readBuffer = new char[httpActionStatus.size];
	r = atProcessor->readHTTPResponse(0, httpActionStatus.size, readBuffer);
	delete [] readBuffer;
	if(r != DCE_OK) {
		return -1;
	}

	r = atProcessor->terminateHTTP();
	if(r != DCE_OK) {
		return -1;
	}

	//close connection
	r = atProcessor->closeIPBearer(bearerId);
	if(r != DCE_OK) {
		return -1;
	}

/*
	CALL_STATE cs = atProcessor->startCall("0501906337", true);
	while(cs == CALL_CONNECT_VOICE) {
		CALL_DETAILS * const details = new CALL_DETAILS[2];
		int nCalls = atProcessor->getListCurrentCalls(details, 2);
		if(nCalls < 0) {
			//TODO missing delete
			break;
		}
		delete details;
	}
*/

	delete portIO;

	return 0;
}

