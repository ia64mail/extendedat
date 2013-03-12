/*
 * Sim900AT.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */

#include "Sim900AT.h"

Sim900AT::Sim900AT(PortIO * portIO) {
	this->portIO = portIO;
}

Sim900AT::~Sim900AT() {

}

int Sim900AT::testAT(){
	char * command = new char[] {"AT\r"};
	const char responceSize = 7;
	bool resFlag = true;

	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;
	if(!resFlag) {
		return -1;
	}

	char * responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	if(!resFlag) {
		delete [] responce;
		return -1;
	}

	using namespace std;

	delete [] responce;
	return 0;
}

