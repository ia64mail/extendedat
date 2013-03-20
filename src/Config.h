/*
 * Config.h
 *
 *  Created on: Mar 19, 2013
 *      Author: ia64domain
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/**
 * Declare LOGGING if need enable console logging
 */
#define LOGGING

/**
 * Mobile Equipment Error reporting level.
 * Possible values - 0 and 1
 *
 * If defined 0 - not used,
 * otherwise used in short format (XX code only)
 * \r\n+CME ERROR: (XX)\r\n
 */
#define MEE_LEVEL 1

#if MEE_LEVEL == 0
#define MEE_OFFSET 0
#else
#define MEE_OFFSET 20
#endif

/**
 * Declare PORT device
 */
#define COM_PORT "/dev/ttyUSB0"

/**
 * Control chars redefinitions
 */
#define CHAR_CR '\r'
#define CHAR_NL '\n'
#define CHAR_TR '\0'

#endif /* CONFIG_H_ */
