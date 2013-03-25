/*
 * BearerConfig.h
 *
 *  Created on: Mar 25, 2013
 *      Author: ia64domain
 */

#ifndef BEARERCONFIG_H_
#define BEARERCONFIG_H_

#define APN_MAX_LENGHT 51 /*max length of APN parameter value (include terminate key code)*/
#define USER_MAX_LENGHT 51 /*max length of user name parameter value (include terminate key code)*/
#define PWD_MAX_LENGHT 51 /*max length of password parameter value (include terminate key code)*/
#define PNUM_MAX_LENGHT 20 /*max length of phone number parameter value (include terminate key code)*/

enum BEARER_CONFIG_CONNECTION_TYPE {
	BEARER_PARAM_CONTYPE_CSD, BEARER_PARAM_CONTYPE_GPRS
};

enum BEARER_CONFIG_CONNECTION_SPEED {
	BEARER_PARAM_CSD_SPEED_2400, BEARER_PARAM_CSD_SPEED_4800,
	BEARER_PARAM_CSD_SPEED_9600, BEARER_PARAM_CSD_SPEED_14400
};

class BearerConfig {
private:
	/**
	 * Block of configuration parameter names.
	 */
	static const char connectionTypeParamName[8];
	static const char apnNameParamName[4];
	static const char userNameParamName[5];
	static const char userPasswordParamName[4];
	static const char phoneNumberParamName[9];
	static const char connectionSpeedParamName[5];

	unsigned int bearerProfileID;
	BEARER_CONFIG_CONNECTION_TYPE connectionType;
	char * apnName[APN_MAX_LENGHT];
	char * userName[USER_MAX_LENGHT];
	char * userPassword[PWD_MAX_LENGHT];
	char * phoneNumber[PNUM_MAX_LENGHT];
	BEARER_CONFIG_CONNECTION_SPEED connectionSpeed;

public:
	BearerConfig(unsigned int bearerProfileID,
			const BEARER_CONFIG_CONNECTION_TYPE connectionType, const char * apnName[APN_MAX_LENGHT]);
	virtual ~BearerConfig();

	void setConnectionType(const BEARER_CONFIG_CONNECTION_TYPE connectionType);
	void setApnName(const apnName[APN_MAX_LENGHT]);
	void setUserName(const userName[USER_MAX_LENGHT]);
	void setUserPassword(const userPassword[PWD_MAX_LENGHT]);
	void setPhoneNumber(const phoneNumber[PNUM_MAX_LENGHT]);
	void setConnectionSpeed(const BEARER_CONFIG_CONNECTION_SPEED connectionSpeed);

	const unsigned int & getBearerProfileID() const;
	const BEARER_CONFIG_CONNECTION_TYPE & getConnectionType() const;
	const char * const getApnName() const;
	const char * const getUserName() const;
	const char * const getUserPassword() const;
	const char * const getPhoneNumber() const;
	const BEARER_CONFIG_CONNECTION_SPEED & getConnectionSpeed() const;

	const char * const getConnectionTypeParamName() const;
	const char * const getApnNameParamName() const;
	const char * const getUserNameParamName() const;
	const char * const getUserPasswordParamName() const;
	const char * const getPhoneNumberParamName() const;
	const char * const getConnectionSpeedParamName() const;
};

#endif /* BEARERCONFIG_H_ */
