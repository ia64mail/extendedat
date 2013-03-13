/*
 * Sim900AT.h
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */
#include <iostream>
#include <regex.h>
#include "PortIO.h"

#ifndef SIM900AT_H_
#define SIM900AT_H_

#define LOGGING

class Sim900AT {
private:
	PortIO * portIO;

	/**
	 * Find matches into string
	 */
	int match_regex(const char * const regex_text, const char * const to_match,
			const int n_matches, regmatch_t * const m) const;
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
