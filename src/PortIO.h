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

//Total number of attempts communicate with port
const unsigned short TOTAL_TRIES = 3;

//Size of temp buffer for cleaning port in case receiving buffer overflow
const unsigned short TEMP_BUFFER_SIZE = 10;

class PortIO {
private:
	int pd; /*port descriptor*/

	/**
	 * Open port.
	 */
	void open_port(void);

	/**
	 * Configure port before use.
	 */
	void configureUART() const;
public:
	PortIO();
	virtual ~PortIO();

	/**
	 * Receive AT response from UART port.
	 * Size of buffer should include +1 byte for null terminated key code.
	 *
	 * @return count of received bytes if receive was successful,
	 *  -1 if not enough size for incoming buffer
	 */
	int receiveUART(char * const buffer, const int size) const;

	/**
	 * Receive raw AT response from UART port.
	 * Buffer will not terminated by null terminated key code
	 * and UART will be unloaded only for specified size
	 *
	 * @return count of received bytes
	 */
	int receiveRawUART(char * const buffer, const int size) const;

	/**
	 * Send an AT command to UART port.
	 * Command should be assembled with final <CR> key code.
	 *
	 * @return count of sent bytes if send was successful,
	 * or -1 if sending data has integrity problem
	 */
	int sendUART(const char * const buffer) const;
};

#endif /* PORTIO_H_ */
