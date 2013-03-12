/*
 * Sim900AT.h
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */
#include <iostream>
#include <regex>

#ifndef SIM900AT_H_
#define SIM900AT_H_

class Sim900AT {
private:
	PortIO * portIO;
public:
	Sim900AT(PortIO * portIO);
	virtual ~Sim900AT();

	/**
	 * Send simple AT command and ensure,
	 * that response is OK
	 *
	 * @return 0 if command passed
	 */
	int testAT();
};

#endif /* SIM900AT_H_ */
