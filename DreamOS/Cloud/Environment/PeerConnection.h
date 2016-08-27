#ifndef PEER_CONNECTION_H_
#define PEER_CONNECTION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/EnvironmentPeer.h
// Environment Peer Object

#include <string>
#include <vector>

class PeerConnection {
public:
	PeerConnection(long userID, long peerUserID, long peerConnectionID) :
		m_userID(userID),
		m_peerUserID(peerUserID),
		m_peerConnectionID(peerConnectionID)
	{
		// empty
	}

	RESULT Print() {
		DEBUG_LINEOUT("User ID %d Peer ID %d Peer Connection ID %d", m_userID, m_peerUserID, m_peerConnectionID);
		DEBUG_LINEOUT("SDP Offer: %s", m_strSDPOffer.c_str());
		DEBUG_LINEOUT("SDP Answer: %s", m_strSDPAnswer.c_str());

		DEBUG_LINEOUT("User Candidates:");
		for (auto &strCandidate : m_strUserCandidates)
			DEBUG_LINEOUT(strCandidate.c_str());

		DEBUG_LINEOUT("Peer Candidates:");
		for (auto &strCandidate : m_strPeerCandidates)
			DEBUG_LINEOUT(strCandidate.c_str());

		return R_PASS;
	}

	RESULT UpdateSDPOffer(std::string strSDPOffer) {
		m_strSDPOffer = strSDPOffer;
		return R_PASS;
	}

	RESULT UpdateSDPAnswer(std::string strSDPanswer) {
		m_strSDPAnswer = strSDPanswer;
		return R_PASS;
	}

	RESULT ClearUserCandidates() {
		m_strUserCandidates.clear();
		return R_PASS;
	}

	RESULT AddUserCandidate(std::string strCandidate) {
		m_strUserCandidates.push_back(strCandidate);
		return R_PASS;
	}

	RESULT ClearPeerCandidates() {
		m_strPeerCandidates.clear();
		return R_PASS;
	}

	RESULT AddPeerCandidate(std::string strCandidate) {
		m_strPeerCandidates.push_back(strCandidate);
		return R_PASS;
	}

	long GetUserID() { return m_userID; }
	long GetPeerUserID() { return m_peerUserID; }
	long GetPeerConnectionID() { return m_peerConnectionID; }

	const std::string& GetSDPOffer() { return m_strSDPOffer; }
	const std::string& GetSDPAnswer() { return m_strSDPAnswer; }

	const std::vector<std::string> GetUserCandidates() { return m_strUserCandidates; }
	const std::vector<std::string> GetPeerCandidates() { return m_strPeerCandidates; }

private:
	long m_userID;
	long m_peerUserID;
	long m_peerConnectionID;


	std::string m_strSDPOffer;	
	std::string m_strSDPAnswer;

	std::vector<std::string> m_strUserCandidates;
	std::vector<std::string> m_strPeerCandidates;
};

#endif	// !PEER_CONNECTION_H_