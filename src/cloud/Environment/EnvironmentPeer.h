#ifndef ENVIRONMENT_PEER_H_
#define ENVIRONMENT_PEER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/EnvironmentPeer.h
// Environment Peer Object

#include <string>

class EnvironmentPeer {
public:
	EnvironmentPeer(long userID, long environmentID, const std::string& strSDPOffer) :
		m_userID(userID),
		m_environmentID(environmentID),
		m_strSDPOffer(strSDPOffer)
	{
		// empty
	}

	RESULT PrintEnvironmentPeer() {
		DEBUG_LINEOUT("User ID %d Environment ID %d", m_userID, m_environmentID);
		DEBUG_LINEOUT("SDP Offer: %s", m_strSDPOffer.c_str());
		return R_PASS;
	}

	RESULT UpdateSDPOffer(std::string strSDPOffer) {
		m_strSDPOffer = strSDPOffer;
		return R_PASS;
	}

	long GetUserID() { return m_userID; }
	long GetEnvironmentID() { return m_environmentID; }
	const std::string& GetSDPOffer() { return m_strSDPOffer; }

private:
	long m_userID;
	long m_environmentID;
	std::string m_strSDPOffer;	// TODO: Store as JSON?
};

#endif	// !ENVIRONMENT_PEER_H_