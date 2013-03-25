/*
 * PortIO.h
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */
#include <iostream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include "Config.h"
#include "Util.h"

#ifndef PORTIO_H_
#define PORTIO_H_

//Total number of attempts communicate with port during async read/write
const unsigned short TOTAL_TRIES = 3;

//Size of temp buffer for cleaning port in case receiving buffer overflow
const unsigned short TEMP_BUFFER_SIZE = 10;

/**
 * Main hardware abstraction class.
 * It provide methods for operate
 * with UART port.
 */
class PortIO {
private:
	int pd;/*Port descriptor*/
	void openPort(void);
	void configurePort() const;
public:
	PortIO();
	virtual ~PortIO();

	unsigned int receiveRawFromPort(char * const buffer, const unsigned int size) const;
	int receiveFromPort(char * const buffer, const unsigned int size) const;
	int sendToPort(const char * const buffer) const;

	static void sleepPort(unsigned int seconds);
};

#endif /* PORTIO_H_ */
