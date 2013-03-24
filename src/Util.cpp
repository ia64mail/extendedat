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
 * Write buffer to output. Control chars will be encoded.
 */
void Util::writeEncoded(const char * const buffer, const unsigned int size) {
	using namespace std;

	const char * buffPtr = buffer;
	unsigned short i = 0;
	while (*buffPtr != CHAR_TR || (size > 0 && i < size)) {
		if(iscntrl(*buffPtr)) {
			switch(*buffPtr) {
				case CHAR_NL : cout << "\\n";
				break;
				case CHAR_CR : cout << "\\r";
				break;
				default: cout << '\\' << int(*buffPtr);
				break;
			}
		} else {
			cout << *buffPtr;
		}

		buffPtr++;
		i++;
	}

	if(size != 0) {
		cout << "\0";
	}
}

