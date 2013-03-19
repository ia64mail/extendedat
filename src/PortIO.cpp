/*
 * PortIO.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */

#include "PortIO.h"

using namespace std;

PortIO::PortIO() {
	open_port();
	configureUART();
}

PortIO::~PortIO() {
	if(pd != NULL) {
		close(pd);
	}
}

int PortIO::receiveUART(char * const buffer, const int size) const {
	char *buffPtr; /* Current char in buffer */
	int nBytes; /* Number of bytes read */
	int totalBytes = 0; /* Number of total bytes read */

	#ifdef LOGGING
		cout << "Receive from UART: ";
	#endif

	//receive AT response
	//try few times to ensure that all data received
	buffPtr = buffer;
	for (int tries = 0; tries < TOTAL_TRIES; tries++) {
		//read from port until 0 bytes received
		while ((nBytes = read(pd, buffPtr, buffer - buffPtr + size - 1)) > 0) {
			buffPtr += nBytes;
			totalBytes += nBytes;
		}

		//break if buffer already full
		if(buffer - buffPtr == size - 1) {
			break;
		}
	}


	/* null terminate the string and see if we got an OK response */
	*buffPtr = CHAR_TR;

	#ifdef LOGGING
			cout << "\"";
			Util::writeEncoded(buffer);
			cout << "\"";
			cout << " in " << totalBytes << " byte(s)";
	#endif

	//ensure, that all data received and buffer is empty
	bool isOverloaded = false;
	if(buffPtr - buffer == size-1) {
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

	if (buffPtr[-1] == CHAR_NL && buffPtr[-2] == CHAR_CR) {
		#ifdef LOGGING
				cout << " [OK]" << endl;
		#endif

		return totalBytes;
	}

	#ifdef LOGGING
		cout <<  endl << "... [Error] Data integrity failed!" << endl;
	#endif
	return -1;
}

int PortIO::sendUART(const char * const buffer) const {
	const char *buffPtr; /* Current char in buffer */
	int nBytes; /* Number of bytes read */
	int totalBytes = 0; /* Number of total bytes read */

	//get size of buffer string excluding last terminate key code '\0'
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

void PortIO::open_port(void) {
	#ifdef LOGGING
		cout << endl << "Opening UART port on " << COM_PORT;
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

void PortIO::configureUART() const {
	#ifdef LOGGING
		cout << endl << "Configuring UART ..." << endl;
	#endif

	bool initFlag = true;
	unsigned short size = 12;
	char * const buffer = new char[size];

	//Configure ECHO mode
	if(sendUART("ATE0\r") > 0) {
		initFlag &= (receiveUART(buffer, size) > 0);
	}

	if(initFlag && sendUART("AT+CMEE=1\r") > 0) {
		initFlag &= (receiveUART(buffer, size) > 0);
	}

	delete [] buffer;
}
