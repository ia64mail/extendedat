/*
 * Util.h
 *
 *  Created on: Mar 19, 2013
 *      Author: ia64domain
 */
#include <iostream>
#include "Config.h"

#ifndef UTIL_H_
#define UTIL_H_

class Util {
private:
	Util();
	virtual ~Util();
public:
	static void writeEncoded(const char * const buffer, const unsigned int size = 0);
};

#endif /* UTIL_H_ */
