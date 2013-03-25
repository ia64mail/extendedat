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

/**
 * Helper class for service usage.
 * This is singleton with static methods.
 */
class Util {
private:
	Util();
	virtual ~Util();
public:
	static void writeEncoded(const char * const buffer, const unsigned int length = 0);
};

#endif /* UTIL_H_ */
