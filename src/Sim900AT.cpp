/*
 * Sim900AT.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: igor
 */

#include "Sim900AT.h"

Sim900AT::Sim900AT(PortIO * portIO) {
	this->portIO = portIO;
}

Sim900AT::~Sim900AT() {

}

#define MAX_ERROR_MSG 0x1000

int Sim900AT::match_regex(const char * const regex_text, const char * const to_match,
		const int n_matches, regmatch_t * const m) const {
	using namespace std;

	regex_t * rp = new regex_t;

	int status = regcomp(rp, regex_text, REG_EXTENDED | REG_NEWLINE);
	if (status != 0) {
		#ifdef LOGGING
			char * error_message = new char[MAX_ERROR_MSG];
			regerror (status, rp, error_message, MAX_ERROR_MSG);
			cout << "... [Error] Regex error compiling in \"" << regex_text << "\" " << endl;
			cout << "... caused by " << error_message << endl;
			delete [] error_message;
		#endif
		delete rp;

		return 1;
	}

	/* first occurrence of search pattern rp inside string p
	 * matching whole pattern placed into m[0] element
	 * other occurrence of search pattern part in ()
	 * placed into m[1+] elements.
	 */
	int nomatch = regexec(rp, to_match, n_matches, m, 0);

	if (nomatch) {
		#ifdef LOGGING
			char * error_message = new char[MAX_ERROR_MSG];
			regerror (status, rp, error_message, MAX_ERROR_MSG);
			cout << "... [Warning] No matches found in \"" << regex_text << "\" " << endl;
			cout << "... caused by " << error_message << endl;
			delete [] error_message;
		#endif
		delete rp;

		return nomatch;
	}

	delete rp;
	return 0;
}

int Sim900AT::testAT(){
	char * command = new char;
	command = "AT\r";
	const char responceSize = 7;
	bool resFlag = true;

	resFlag &= (portIO->sendUART(command) > 0);

	//TODO
	//delete command;
	if(!resFlag) {
		return -1;
	}

	char * responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	if(!resFlag) {
		delete [] responce;
		return -1;
	}

	regmatch_t * matches = new regmatch_t[2];
	match_regex("\r\n(.{2})\r\n", responce, 2, matches);

	delete [] responce;
	return 0;
}

