//============================================================================
// Name        : Sim900.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include "PortIO.h"

#define LOGGING

/**
 * Main entry point
 */
int main() {
	using namespace std;

	PortIO * portIO = new PortIO();

	char * buffer;
	#ifdef LOGGING
		cout << endl << "Communicating..." << endl;
	#endif

	buffer = new char[330];
	portIO->sendUART("AT&V\r");
	portIO->receiveUART(buffer, 330);
	delete [] buffer;

	buffer = new char[10];
	portIO->sendUART("AT\r");
	portIO->receiveUART(buffer, 10);
	delete [] buffer;

	delete portIO;

	return 0;
}

