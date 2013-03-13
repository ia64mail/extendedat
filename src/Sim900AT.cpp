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

int Sim900AT::compile_regex (regex_t * rp, const char * regex_text) const {
	using namespace std;

    int status = regcomp (rp, regex_text, REG_EXTENDED|REG_NEWLINE);
    if (status != 0) {
	char error_message[MAX_ERROR_MSG];
	regerror (status, rp, error_message, MAX_ERROR_MSG);
        printf ("Regex error compiling '%s': %s\n",
                 regex_text, error_message);
        return 1;
    }
    return 0;
}

int Sim900AT::match_regex (regex_t * rp, const char * to_match) const {
	using namespace std;

    /* "P" is a pointer into the string which points to the end of the
       previous match. */
    const char * p = to_match;
    /* "N_matches" is the maximum number of matches allowed. */
    const int n_matches = 10;
    /* "M" contains the matches found. */
    regmatch_t m[n_matches];

    while (1) {
        int i = 0;

        /* first occurrence of search pattern rp inside string p
         * matching whole pattern placed into m[0] element
         * other occurrence of search pattern part in ()
         * placed into m[1+] elements.
         */
        int nomatch = regexec (rp, p, n_matches, m, 0);

        if (nomatch) {
            printf ("No more matches.\n");
            return nomatch;
        }
        for (i = 0; i < n_matches; i++) {
            int start;
            int finish;
            if (m[i].rm_so == -1) {
                break;
            }
            start = m[i].rm_so + (p - to_match);
            finish = m[i].rm_eo + (p - to_match);
            if (i == 0) {
                printf ("$& is ");
            }
            else {
                printf ("$%d is ", i);
            }
            printf ("'%.*s' (bytes %d:%d)\n", (finish - start),
                    to_match + start, start, finish);
        }
        p += m[0].rm_eo;
    }
    return 0;
}

int Sim900AT::testAT(){
	char * command = new char[] {"AT\r"};
	const char responceSize = 7;
	bool resFlag = true;

	resFlag &= (portIO->sendUART(command) > 0);

	delete [] command;
	if(!resFlag) {
		return -1;
	}

	char * responce = new char[responceSize];
	resFlag &= (portIO->receiveUART(responce, responceSize) > 0);

	if(!resFlag) {
		delete [] responce;
		return -1;
	}

	delete [] responce;
	return 0;
}

