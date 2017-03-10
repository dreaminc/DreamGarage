#ifndef USER_MODEL_H_
#define USER_MODEL_H_

// TODO: Rename the file and class

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/User/User.h
// Base User object

#include "Cloud/CloudModel.h"

#include <string>
#include "Primitives/version.h"
#include <iostream>

class User {
public:
	User() :
		m_userID(-1),
		m_defaultEnvironmentID(-1),
		m_strEmail(""),
		m_strFirstName(""),
		m_strLastName(""),
		m_strScreenName(""),
		m_version(0.0f)
	{
		// empty
	}

	User(long userID, long defaultEnvironmentID, 
		std::string strEmail, std::string strScreename, 
		std::string strFirstName, std::string strLastName, 
		version userVersion
	) :
		m_userID(userID),
		m_defaultEnvironmentID(defaultEnvironmentID),
		m_strEmail(strEmail),
		m_strScreenName(strScreename),
		m_strFirstName(strFirstName),
		m_strLastName(strLastName),
		m_version(userVersion)
	{
		// empty
	}

	RESULT PrintUser() {
		
		DEBUG_LINEOUT("User %d Version %s", m_userID, m_version.GetString().c_str());
		DEBUG_LINEOUT("Default Environment %d", m_defaultEnvironmentID);
		DEBUG_LINEOUT("email: %s", m_strEmail.c_str());
		DEBUG_LINEOUT("screen name: %s", m_strScreenName.c_str());
		DEBUG_LINEOUT("first name: %s", m_strFirstName.c_str());
		DEBUG_LINEOUT("last name: %s", m_strLastName.c_str());

		return R_PASS;
	}
	
	long GetUserID() const { return m_userID; }
	long GetDefaultEnvironmentID() const { return m_defaultEnvironmentID; }

	const std::string&	GetEmail() const { return m_strEmail; }
	const std::string&	GetFirstName() const  { return m_strFirstName; }
	const std::string&	GetLastName() const  { return m_strLastName; }
	const std::string&	GetScreenName() const  { return m_strScreenName; }
	
	const std::string&	GetToken() const { return m_strToken; }

	RESULT SetToken(std::string strToken) {
		if (strToken.size() > 0) {
			m_strToken = strToken;
			return R_PASS;
		}

		return R_FAIL;
	}

	version GetVersion() { return m_version; }

	friend std::ostream& operator<<(std::ostream& os, const User& user);

private:
	long m_userID;
	long m_defaultEnvironmentID;

	std::string	m_strEmail;
	std::string	m_strFirstName;
	std::string	m_strLastName;
	std::string	m_strScreenName;

	std::string m_strToken;

	version m_version;
};

std::ostream& operator<<(std::ostream& os, const User& user);

#endif	// ! USER_H_