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


// TODO: Change naming to non-Twilio related
class TwilioNTSInformation {
public:
	TwilioNTSInformation() :
		m_TTL(-1),
		m_timeCreated(0),
		m_timeUpdated(0)
	{
		ClearICEServers();
	}

	TwilioNTSInformation(std::string strDateCreated, std::string strDateUpdated, int TTL) : 
		m_TTL(TTL)
	{
		ClearICEServers();

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

	RESULT ClearICEServers() {
		m_ICEServerURIs.clear();
		m_ICEServerUsernames.clear();
		m_ICEServerPasswords.clear();

		return R_PASS;
	}

	// Note that the ordering of calling this function will mean something going into the WebRTC
	// so important to make sure that the ordering received from server is ensured
	RESULT AddICEServerURI(std::string strICEServerURI, std::string strUsername, std::string strPassword) {
		RESULT r = R_PASS;

		auto it = std::find(m_ICEServerURIs.begin(), m_ICEServerURIs.end(), strICEServerURI);
		CBM((it == m_ICEServerURIs.end()), "%s already in list", strICEServerURI.c_str());

		// TODO: this is a bit flimsy but it should be ok since it all 
		// happens at the same time, and cleared at the same time
		m_ICEServerURIs.push_back(strICEServerURI);
		m_ICEServerUsernames.push_back(strUsername);
		m_ICEServerPasswords.push_back(strPassword);

	Error:
		return r;
	}

	RESULT Print() {
		DOSLOG(INFO, "Twilio NTS Information TTL: %d", m_TTL);
		DOSLOG(INFO, "Created: %s Updated %s", std::ctime(&m_timeCreated), std::ctime(&m_timeUpdated));
		
		for (int i = 0; i < m_ICEServerURIs.size(); i++) {
			DOSLOG(INFO, "%d: %s", i, m_ICEServerURIs[i].c_str());
			//DOSLOG(INFO, "%d: Username: %s", i, m_ICEServerUsernames[i].c_str());
			//DOSLOG(INFO, "%d: Password: %s", i, m_ICEServerPasswords[i].c_str());
		}

		return R_PASS;
	}

private:
	std::time_t m_timeCreated;
	std::time_t m_timeUpdated;
	int m_TTL;

	std::string m_strUsername;
	std::string m_strPassword;

public:
	std::vector<std::string> m_ICEServerURIs;
	std::vector<std::string> m_ICEServerUsernames;
	std::vector<std::string> m_ICEServerPasswords;
};


#endif // !TWILIO_NTS_INFORMATION_H_