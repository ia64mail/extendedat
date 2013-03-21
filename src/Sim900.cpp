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

	COMMON_AT_RESULT r = atProcessor->testAT();
	SIMCARD_STATE s = atProcessor->checkSimCardLockState();
	if(s == SIM_PIN_REQUIRED) {
		r = atProcessor->unlockSimCard("0000");
	}
	CALL_STATE cs = atProcessor->startVoiceCall("0501906337");

	while(cs == CALL_CONNECT_VOICE) {
		CALL_DETAILS * const details = new CALL_DETAILS[2];
		int nCalls = atProcessor->getListCurrentCalls(details, 2);
		if(nCalls < 0) {
			//TODO missing delete
			break;
		}
		delete details;
	}

	delete portIO;

	return 0;
}

