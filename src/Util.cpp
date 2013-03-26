/*
 * Util.cpp
 *
 *  Created on: Mar 19, 2013
 *      Author: ia64domain
 */

#include "Util.h"

Util::Util() {
}

Util::~Util() {
}

/**
 * Write buffer to standard output. Control chars will be encoded.
 *
 * If parameter length is specified (great then zero), then to output
 * will be sent specified count of chars if terminated '\0' key code
 * will not occur first.
 */
void Util::writeEncoded(const char * const buffer, const unsigned int length) {
	using namespace std;

	const char * buffPtr = buffer; /*tracking service pointer*/
	unsigned int i = 0; /*printed chars counter*/

	while (*buffPtr != CHAR_TR || (length > 0 && i < length)) {
		if(iscntrl(*buffPtr)) {
			//special char detected
			switch(*buffPtr) {
			case CHAR_NL :
				cout << "\\n";
				break;
			case CHAR_CR :
				cout << "\\r";
				break;
			default:
				cout << '\\' << int(*buffPtr);
				break;
			}
		} else {
			//ordinary char detected
			cout << *buffPtr;
		}

		buffPtr++;
		i++;
	}
}

