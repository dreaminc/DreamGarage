#ifndef TWILIO_NTS_INFORMATION_H_
#define TWILIO_NTS_INFORMATION_H_

// TODO: Rename the file and class

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/User/User.h
// Base User object

#include "Cloud/CloudModel.h"

#include <string>
#include "Primitives/version.h"
#include <list>
#include <vector>
#include <algorithm>

#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>


class TwilioNTSInformation {
public:
	TwilioNTSInformation() :
		m_TTL(-1),
		m_strUsername(""),
		m_strPassword(""),
		m_timeCreated(0),
		m_timeUpdated(0)
	{
		ClearICEServerURIs();
	}

	TwilioNTSInformation(std::string strDateCreated, std::string strDateUpdated, int TTL,
						 std::string strUsername, std::string strPassword) : 
		m_TTL(TTL),
		m_strUsername(strUsername),
		m_strPassword(strPassword)
	{
		ClearICEServerURIs();

		struct std::tm tmCreated;
		std::istringstream ssCreated(strDateCreated);
		ssCreated >> std::get_time(&tmCreated, "%Y-%m-%dT%H:%M:%S");
		m_timeCreated = mktime(&tmCreated);

		struct std::tm tmUpdated;
		std::istringstream ssUpdated(strDateUpdated);
		ssUpdated >> std::get_time(&tmUpdated, "%Y-%m-%dT%H:%M:%S");
		m_timeUpdated = mktime(&tmUpdated);
	}

	~TwilioNTSInformation() {
		// empty
	}

	RESULT ClearICEServerURIs() {
		m_ICEServerURIs.clear();
		return R_PASS;
	}

	// Note that the ordering of calling this function will mean something going into the WebRTC
	// so important to make sure that the ordering received from server is ensured
	RESULT AddICEServerURI(std::string strICEServerURI) {
		RESULT r = R_PASS;

		auto it = std::find(m_ICEServerURIs.begin(), m_ICEServerURIs.end(), strICEServerURI);
		CBM((it == m_ICEServerURIs.end()), "%s already in list", strICEServerURI.c_str());

		m_ICEServerURIs.push_back(strICEServerURI);

	Error:
		return r;
	}

	RESULT Print() {
		DEBUG_LINEOUT("Twilio NTS Information TTL: %d", m_TTL);
		DEBUG_LINEOUT("Created: %s Updated %s", std::ctime(&m_timeCreated), std::ctime(&m_timeUpdated));
		DEBUG_LINEOUT("Username: %s", m_strUsername.c_str());
		DEBUG_LINEOUT("Password: %s", m_strPassword.c_str());

		int i = 0;
		for (auto &strICEServerURI : m_ICEServerURIs) {
			DEBUG_LINEOUT("%d: %s", i++, strICEServerURI.c_str());
		}

		return R_PASS;
	}

	std::string GetUsername() {
		return m_strUsername;
	}

	std::string GetPassword() {
		return m_strPassword;
	}

private:
	std::time_t m_timeCreated;
	std::time_t m_timeUpdated;
	int m_TTL;

	std::string m_strUsername;
	std::string m_strPassword;

public:
	std::list<std::string> m_ICEServerURIs;
};


#endif // !TWILIO_NTS_INFORMATION_H_