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
	 * Create GPRS connection for HTTP AT commands
	 */
	BEARER_PARAMETER_DETAILS bearerDetails;
	int bearerId = 1;

	//setup connection
	while(true) {
		//connection type
		bearerDetails.bearerProfileID = bearerId;
		bearerDetails.paramName = BEARER_PARAM_CONTYPE;
		//bearerDetails.paramValue = BEARER_PARAM_CONTYPE_GPRS;

		r = atProcessor->setIPBearerParameters(bearerDetails);
		if(r != DCE_OK) {
			return -1;
		}

		//AP name
		bearerDetails.bearerProfileID = bearerId;
		bearerDetails.paramName = BEARER_PARAM_APN;
		//bearerDetails.paramValue = "internet";

		r = atProcessor->setIPBearerParameters(bearerDetails);
		if(r != DCE_OK) {
			return -1;
		}

		break;
	}

	//open connection
	r = atProcessor->openIPBearer(bearerId);

	//check connection state
	BEARER_STATUS bearerStatus;
	int counter = 0;
	while(bearerStatus.mode != BEARER_CONNECTED && counter < 100) {
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

	HTTPConfig httpConfig = HTTPConfig(1, "www.ya.ru");
	atProcessor->initialiseHTTPContext(httpConfig);
	if(r != DCE_OK) {
		return -1;
	}

	HTTP_ACTION_STATUS httpActionStatus;
	r = atProcessor->setCurrentAction(HTTP_ACTION_METHOD_GET, httpActionStatus);
	if(r != DCE_OK) {
		return -1;
	}

	cout << "HTTP action status ..." << " action=" << httpActionStatus.method << " HTTPCODE=" << httpActionStatus.httpResponcecCode
			<< " size=" << httpActionStatus.size << endl;

	r = atProcessor->terminateHTTP();
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

