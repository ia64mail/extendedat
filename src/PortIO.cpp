/*
 * PortIO.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */

#include "PortIO.h"

using namespace std;

PortIO::PortIO() {
	//initialize port before use
	openPort();
	configurePort();
}

PortIO::~PortIO() {
	//close port
	if(!pd) {
		close(pd);
	}
}

/**
 * Open port.
 * Setup operating mode and speed.
 */
void PortIO::openPort(void) {
	#ifdef LOGGING
		cout << endl << "Opening port on " << COM_PORT;
	#endif

	//open in Read/Write mode,
	//don't make it the controlling terminal for the process,
	//ignore CDC status from terminal
	pd = open(COM_PORT, O_RDWR | O_NOCTTY | O_NDELAY);

	if (pd == -1) {
		#ifdef LOGGING
			cout << endl << "... [Error] Cannot open port!" << endl;
		#endif
		return;
	}

	//set pd flags O_APPEND, O_NONBLOCK, O_ASYNC, O_DIRECT to 0
	fcntl(pd, F_SETFL, 0);

	//get port settings
	struct termios options;
	tcgetattr(pd, &options);

	//set raw input, 1 second timeout
	options.c_cflag |= (CLOCAL | CREAD | CRTSCTS);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 10;

	//set speed 57600
    cfsetispeed(&options, B57600);
    cfsetospeed(&options, B57600);

    //No parity (8N1)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

	//set port settings
	tcsetattr(pd, TCSANOW, &options);

	#ifdef LOGGING
		cout << " successfully at " << hex << &pd << dec << " [OK]" << endl;
	#endif
}

/**
 * Configure port.
 */
void PortIO::configurePort() const {
	#ifdef LOGGING
		cout << endl << "Configuring port ..." << endl;
	#endif

	bool initFlag = true;
	unsigned short size = 12 + MEE_OFFSET;
	char * const buffer = new char[size];

	//Configure ECHO mode
	if(sendToPort("ATE0\r") > 0) {
		initFlag &= (receiveFromPort(buffer, size) > 0);
	}

	//Configure dial tone and busy signal detection mode
	if(sendToPort("ATX4\r") > 0) {
		initFlag &= (receiveFromPort(buffer, size) > 0);
	}

	//Configure mobile equipment error mode
	char commandTemplate[] = "AT+CMEE=%d\r";
	char * command = new char[sizeof(commandTemplate)];

	sprintf(command, commandTemplate, MEE_LEVEL);

	if(initFlag && sendToPort(command) > 0) {
		initFlag &= (receiveFromPort(buffer, size) > 0);
	}

	delete [] command;
	delete [] buffer;
}

/**
 * Receive raw AT response from Port.
 * Received buffer will not be completed by null terminated key code
 * and port will be unloaded only for specified size.
 *
 * @return count of received bytes
 */
unsigned int PortIO::receiveRawFromPort(char * const buffer, const unsigned int size) const {
	char *buffPtr; /*current char in buffer*/
	int nBytes; /*number of bytes read*/
	int totalBytes = 0; /*number of total bytes read*/

	#ifdef LOGGING
		cout << "Receive from UART: ";
	#endif

	//receive AT response
	//try few times to ensure that all data received
	buffPtr = buffer;
	for (int tries = 0; tries < TOTAL_TRIES; tries++) {
		//read from port until 0 bytes received
		while ((nBytes = read(pd, buffPtr, buffer - buffPtr + size)) > 0) {
			buffPtr += nBytes;
			totalBytes += nBytes;
		}

		//break if buffer already full
		if(buffer - buffPtr == size) {
			break;
		}
	}

	#ifdef LOGGING
			cout << "\"";
			Util::writeEncoded(buffer, totalBytes);
			cout << "\"";
			cout << " in " << totalBytes << " byte(s)";
			cout << " [OK]" << endl;
	#endif

	return totalBytes;
}

/**
 * Receive AT response from Port.
 * Received buffer size should provide +1 byte for null terminated key code.
 *
 * @return count of received bytes if receive was successful,
 *  -1 if not enough size for incoming buffer or data integrity fail
 */
int PortIO::receiveFromPort(char * const buffer, const unsigned int size) const {
	int totalBytes; /*number of total bytes read*/

	//read raw data from port into specified buffer,
	//reserve last char in buffer for terminated key code
	totalBytes = receiveRawFromPort(buffer, size - 1);

	// null terminate the string and see if we got an OK response
	*(buffer + totalBytes) = CHAR_TR;

	//ensure, that all data received and buffer is empty
	//otherwise abort reading operation and return -1
	if(totalBytes == size - 1) {
		bool isOverloaded = false;
		int nBytes; /*number of bytes read*/
		int tempSize = TEMP_BUFFER_SIZE;

		char * temp = new char[tempSize];
		for (int tries = 0; tries < TOTAL_TRIES; tries++) {
			while ((nBytes = read(pd, temp, tempSize)) > 0) {
				isOverloaded = true;
				totalBytes += nBytes;
			}
		}
		delete [] temp;

		if(isOverloaded) {
			#ifdef LOGGING
				cout << endl << "... [Error] Buffer overloaded! ";
				cout << "Actual size of received data is " << totalBytes << " byte(s)"<< endl;
			#endif
			return -1;
		}
	}

	//check if received buffer terminated with correct signature
	if (*(buffer + totalBytes - 1) == CHAR_NL
			&& *(buffer + totalBytes - 2) == CHAR_CR) {
		return totalBytes;
	}

	#ifdef LOGGING
		cout <<  endl << "... [Error] Data integrity failed!" << endl;
	#endif
	return -1;
}

/**
 * Send an AT command to Port.
 * Command should be assembled with final <CR> key code.
 *
 * @return count of sent bytes if send was successful,
 * or -1 if sending data has integrity problem or communication problem
 */
int PortIO::sendToPort(const char * const buffer) const {
	const char *buffPtr; /*current char in buffer*/
	int nBytes; /*number of bytes read*/
	int totalBytes = 0; /*number of total bytes read*/

	//get size of buffer string excluding last terminate key code
	int size = 0;
	buffPtr = buffer;
	while (*buffPtr != CHAR_TR) {
		size++;
		buffPtr++;
	}
	totalBytes = size;

	#ifdef LOGGING
		cout << "Send to UART: \"";
		Util::writeEncoded(buffer);
		cout << "\" in " << totalBytes << " byte(s)";
	#endif

	//check data integrity
	if(buffer == NULL
			|| *(buffer + totalBytes - 1) != CHAR_CR) {
		#ifdef LOGGING
			cout << endl << "... [Error] Data integrity failed!" << endl;
		#endif
		return -1;
	}

	//send AT command
	//try few times to ensure that all data sent
	buffPtr = buffer;
	for (int tries = 0; tries < TOTAL_TRIES; tries++) {
		while ((nBytes = write(pd, buffPtr, buffer - buffPtr + size)) > 0) {
			buffPtr += nBytes;
			size -= nBytes;

			//return if all buffer sent
			if(size == 0) {
				#ifdef LOGGING
					cout << " [OK]" << endl;
				#endif
				return totalBytes;
			}
		}
	}

	#ifdef LOGGING
		cout << endl << "... [Error] Communication failed!";
		cout << "Actual size of sent data is " << totalBytes - size << " byte(s)"<< endl;
	#endif
	return -1;
}
