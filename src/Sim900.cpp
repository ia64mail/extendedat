//============================================================================
// Name        : Sim900.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include "PortIO.h"
#include "Sim900AT.h"

#define LOGGING

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

	delete portIO;

	return 0;
}

