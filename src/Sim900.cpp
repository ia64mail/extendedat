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

#define COM_PORT "/dev/ttyUSB0"

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

/*
 * Send test commands to modem
 */
int send_test_commands() {
	cout << "Test commands ..." << endl;

	return (-1);
}


/**
 * Send an AT command to UART port.
 * Command should be assembled with final <CR> key code.
 *
 * @param fd UART open port descriptor
 *
 * @return 0 if send was successful,
 * -1 if sending data has integrity problem, 1 if send was error
 */
int sendUART(const int &fd, const char * const data, const int size) {
	#ifdef Debug
		cout << "Send to UART :" << data << " ";
	#endif

	//check data integrity
	if(data == NULL
			|| *(data + size - 1) != '\0'
					|| *(data + size - 2) != '\n') {

		#ifdef Debug
				cout << "[Error! Data integrity failed!]" << endl;
		#endif

		return -1;
	}

	//send AT command
	for (int tries = 0; tries < 3; tries++) {
		if (write(fd, data, size) < size) {
			continue;
		} else {
			#ifdef Debug
					cout << "[OK]" << endl;
			#endif

			return 0;
		}
	}

	#ifdef Debug
			cout << "[Error! Communication failed!]" << endl;
	#endif
	return 1;
}

/**
 * Receive AT response from UART port.
 * Size of buffer should include +1 byte for null terminated key code.
 *
 * @return 0 if receive was successful, -1 if not enough size for incoming buffer
 */
int receiveUART(const int &fd, char * const buffer, const int size) {
	char *bufptr; /* Current char in buffer */
	int nbytes; /* Number of bytes read */

	#ifdef Debug
		cout << "Receive from UART :";
	#endif

	//receive AT response
	bufptr = buffer;
	while ((nbytes = read(fd, bufptr, buffer - bufptr + size - 1)) > 0) {
		bufptr += nbytes;
		if (bufptr[-1] == '\n' || bufptr[-1] == '\r') {
			break;
		}
	}

	if (bufptr[-1] == '\n' || bufptr[-1] == '\r') {
		/* null terminate the string and see if we got an OK response */
		*bufptr = '\0';

		#ifdef Debug
			cout << bufptr << " "<<"[OK]" << endl;
		#endif

		return 0;
	}

	#ifdef Debug
			cout << bufptr << " " << "[Error! Buffer overloaded]" << endl;
	#endif
	return -1;
}

int main() {
	#ifdef Debug
	cout << "Starting..." << endl;
	#endif

	int fd = open_port();

	sendUART(fd, "AT\n", 3);

	char buffer[3];
	receiveUART(fd, buffer, 3);

	return 0;
}

