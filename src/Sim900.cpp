//============================================================================
// Name        : Sim900.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define DEBUG
#define COM_PORT "/dev/ttyUSB0"

//Total number of attempts communicate with port
const unsigned short TOTAL_TRIES = 3;

//Size of temp buffer for cleaning port in case receiving buffer overflow
const unsigned short TEMP_BUFFER_SIZE = 10;

//Control chars redefinitions
const char CHAR_CR = '\r';
const char CHAR_NL = '\n';
const char CHAR_TR = '\0';

using namespace std;

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */
int open_port(void) {
	cout << "Opening port ..." << endl;

	int fd; /* File descriptor for the port */

	fd = open(COM_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		perror("open_port: Unable to open ");
	} else {
		/*
		 * Start
		 */
		fcntl(fd, F_SETFL, 0);

		/**
		 * Setup
		 */
		struct termios options;

		/*
		 * Get the current options for the port...
		 */

		tcgetattr(fd, &options);

		/* set raw input, 1 second timeout */
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		options.c_oflag &= ~OPOST;
		options.c_cc[VMIN] = 0;
		options.c_cc[VTIME] = 10;

		/*
		 * Set the baud rates to 19200...


		 cfsetispeed(&options, B19200);
		 cfsetospeed(&options, B19200);


		 * Enable the receiver and set local mode...


		 options.c_cflag |= (CLOCAL | CREAD);


		 * No parity (8N1):


		 options.c_cflag &= ~PARENB;
		 options.c_cflag &= ~CSTOPB;
		 options.c_cflag &= ~CSIZE;
		 options.c_cflag |= CS8;


		 * Hardware flow control

		 options.c_cflag |= CRTSCTS;
		 */
		/*
		 * Set the new options for the port...
		 */

		tcsetattr(fd, TCSANOW, &options);
	}

	cout << "successfully at " << hex << fd << dec << endl;

	return (fd);
}

/*
 * O - 0 = MODEM ok, -1 = MODEM bad
 * I - Serial port file
 */
int init_modem(int fd) {
	cout << "Initializing ..." << endl;

	char buffer[255]; /* Input buffer */
	char *bufptr; /* Current char in buffer */
	int nbytes; /* Number of bytes read */
	int tries; /* Number of tries so far */

	for (tries = 0; tries < 3; tries++) {
		/* send an AT command followed by a CR */
		if (write(fd, "AT\r", 3) < 3)
			continue;

		/* read characters into our string buffer until we get a CR or NL */
		bufptr = buffer;
		while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1))
				> 0) {
			bufptr += nbytes;
			if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
				break;
		}

		/* null terminate the string and see if we got an OK response */
		*bufptr = '\0';

		if (strncmp(buffer, "OK", 2) == 0) {
			cout << "initialized successfully" << endl;
			return (0);
		}
	}

	cout << "ERROR! Don't responding" << endl;

	return (-1);
}

/**
 * Write buffer to output. Control chars will be encoded.
 */
void writeEncoded(const char * const buffer) {
	const char * buffPtr = buffer;
	while (*buffPtr != CHAR_TR) {
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
	}
}

/**
 * Send an AT command to UART port.
 * Command should be assembled with final <CR> key code.
 *
 * @param fd UART port descriptor
 *
 * @return count of sent bytes if send was successful,
 * or -1 if sending data has integrity problem
 */
int sendUART(const int &fd, const char * const buffer) {
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

	#ifdef DEBUG
		cout << "Send to UART: \"";
		writeEncoded(buffer);
		cout << "\" in " << totalBytes << " byte(s)";
	#endif

	//check data integrity
	if(buffer == NULL
			|| *(buffer + totalBytes - 1) != CHAR_CR) {
		#ifdef DEBUG
			cout << endl << "... [Error] Data integrity failed!" << endl;
		#endif
		return -1;
	}

	//send AT command
	//try few times to ensure that all data sent
	buffPtr = buffer;
	for (int tries = 0; tries < TOTAL_TRIES; tries++) {
		while ((nBytes = write(fd, buffPtr, buffer - buffPtr + size)) > 0) {
			buffPtr += nBytes;
			size -= nBytes;

			//return if all buffer sent
			if(size == 0) {
				#ifdef DEBUG
					cout << " [OK]" << endl;
				#endif
				return totalBytes;
			}
		}
	}

	#ifdef DEBUG
		cout << endl << "... [Error] Communication failed!";
		cout << "Actual size of sent data is " << totalBytes - size << " byte(s)"<< endl;
	#endif
	return -1;
}

/**
 * Receive AT response from UART port.
 * Size of buffer should include +1 byte for null terminated key code.
 *
 * @return count of received bytes if receive was successful,
 *  -1 if not enough size for incoming buffer
 */
int receiveUART(const int &fd, char * const buffer, const int size) {
	char *buffPtr; /* Current char in buffer */
	int nBytes; /* Number of bytes read */
	int totalBytes = 0; /* Number of total bytes read */

	#ifdef DEBUG
		cout << "Receive from UART: ";
	#endif

	//receive AT response
	//try few times to ensure that all data received
	buffPtr = buffer;
	for (int tries = 0; tries < TOTAL_TRIES; tries++) {
		//read from port until 0 bytes received
		while ((nBytes = read(fd, buffPtr, buffer - buffPtr + size - 1)) > 0) {
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

	#ifdef DEBUG
			cout << "\"";
			writeEncoded(buffer);
			cout << "\"";
			cout << " in " << totalBytes << " byte(s)";
	#endif

	//ensure, that all data received and buffer is empty
	bool isOverloaded = false;
	if(buffPtr - buffer == size-1) {
		int tempSize = 10;
		char * temp = new char[tempSize];
		for (int tries = 0; tries < TOTAL_TRIES; tries++) {
			while ((nBytes = read(fd, temp, tempSize)) > 0) {
				isOverloaded = true;
				totalBytes += nBytes;
			}
		}

		if(isOverloaded) {
			#ifdef DEBUG
				cout << endl << "... [Error] Buffer overloaded! ";
				cout << "Actual size of received data is " << totalBytes << " byte(s)"<< endl;
			#endif
			return -1;
		}
	}

	if (buffPtr[-1] == CHAR_NL && buffPtr[-2] == CHAR_CR) {
		#ifdef DEBUG
				cout << " [OK]" << endl;
		#endif

		return totalBytes;
	}

	#ifdef DEBUG
		cout <<  endl << "... [Error] Data integrity failed!" << endl;
	#endif
	return -1;
}

void configureUART(const int &fd){
	#ifdef DEBUG
		cout << endl << "Configuring UART ..." << endl;
	#endif

	bool initFlag = true;

	char * buffer;
	unsigned short size;

	//Configure ECHO mode
	if(sendUART(fd, "ATE0\r") > 0) {
		size = 10;
		buffer = new char[size];
		initFlag &= (receiveUART(fd, buffer, size) > 0);
		delete buffer;
	}
}

/**
 * Main entry point
 */
int main() {
	int fd = open_port();


	configureUART(fd);

	char * buffer;
	#ifdef DEBUG
		cout << endl << "Communicating..." << endl;
	#endif

	buffer = new char[400];
	sendUART(fd, "AT&V\r");
	receiveUART(fd, buffer, 400);
	delete buffer;

	buffer = new char[30];
	sendUART(fd, "AT\r");
	receiveUART(fd, buffer, 30);
	delete buffer;

	return 0;
}

