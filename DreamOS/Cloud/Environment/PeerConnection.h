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

	PeerConnection(nlohmann::json jsonPeerConnection, nlohmann::json jsonOfferSocketConnection, nlohmann::json jsonAnswerSocketConnection) :
		m_peerConnectionID(-1),
		m_userID(-1),
		m_peerUserID(-1)
	{
		UpdatePeerConnectionFromJSON(jsonPeerConnection);
		UpdateOfferSocketConnectionFromJSON(jsonOfferSocketConnection);
		UpdateAnswerSocketConnectionFromJSON(jsonAnswerSocketConnection);
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

	RESULT SetSDPOffer(std::string strSDPOffer) {
		m_strSDPOffer = strSDPOffer;
		return R_PASS;
	}

	RESULT SetSDPAnswer(std::string strSDPanswer) {
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
	RESULT SetUserID(long userID) { m_userID = userID; return R_PASS; }

	long GetPeerUserID() { return m_peerUserID; }
	RESULT SetPeerUserID(long peerUserID) { m_peerUserID = peerUserID; return R_PASS; }

	long GetEnvironmentID() { return m_environmentID; }
	RESULT SetEnvironmentID(long environmentID) { m_environmentID = environmentID; return R_PASS; }

	// We can't change a peer connection ID once it's been set
	// TODO: Should also be done for userID, peerUser, and environmentID
	// TODO: Create generic way to do this (more robust model logic)
	long GetPeerConnectionID() { return m_peerConnectionID; }
	RESULT SetPeerConnectionID(long peerConnectionID) { 
		RESULT r = R_PASS;

		if (m_peerConnectionID == -1) {
			m_peerConnectionID = peerConnectionID;
		}

		CBM((m_peerConnectionID == peerConnectionID), "Cannot change peer connection ID once set");

	Error:
		return r; 
	}

	long GetOfferSocketConnectionID() { return m_offerSocketConnectionID; }
	RESULT SetOfferSocketConnectionID(long offerSocketConnectionID) { m_offerSocketConnectionID = offerSocketConnectionID; return R_PASS; }

	long GetAnswerSocketConnectionID() { return m_answerSocketConnectionID; }
	RESULT SetAnswerSocketConnectionID(long answerSocketConnectionID) { m_answerSocketConnectionID = answerSocketConnectionID; return R_PASS; }

	const std::string& GetSDPOffer() { return m_strSDPOffer; }
	const std::string& GetSDPAnswer() { return m_strSDPAnswer; }

	const std::vector<std::string> GetUserCandidates() { return m_strUserCandidates; }
	const std::vector<std::string> GetPeerCandidates() { return m_strPeerCandidates; }

	RESULT UpdateOfferSocketConnectionFromJSON(nlohmann::json jsonOfferSocketConnection) {
		RESULT r = R_PASS;

		long userID = jsonOfferSocketConnection["/user"_json_pointer].get<long>();
		long environmentID = jsonOfferSocketConnection["/environment"_json_pointer].get<long>();

		CR(SetUserID(userID));
		CR(SetEnvironmentID(environmentID));

	Error:
		return r;
	}

	RESULT UpdateAnswerSocketConnectionFromJSON(nlohmann::json jsonAnswerSocketConnection) {
		RESULT r = R_PASS;

		long peerUserId = jsonAnswerSocketConnection["/user"_json_pointer].get<long>();
		long environmentID = jsonAnswerSocketConnection["/environment"_json_pointer].get<long>();

		CR(SetPeerUserID(peerUserId));
		CR(SetEnvironmentID(environmentID));

	Error:
		return r;
	}

	RESULT UpdatePeerConnectionFromJSON(nlohmann::json jsonPeerConnection) {
		RESULT r = R_PASS;

		if (jsonPeerConnection["/id"_json_pointer] != nullptr) {
			CRM(SetPeerConnectionID(jsonPeerConnection["/id"_json_pointer].get<long>()), "Peer connection ID can't be reset");
		}

		// Offer

		if (jsonPeerConnection["/offer"_json_pointer] != nullptr) {
			CRM(SetSDPOffer(jsonPeerConnection["/offer"_json_pointer].get<std::string>()), "Failed to set SDP offer string");
		}

		if (jsonPeerConnection["/offer_candidates"_json_pointer] != nullptr) {
			//CR(SetSDPOffer(jsonPeerConnection["/offer"_json_pointer].get<std::string>()), "Failed to set SDP offer string");
			// TODO:
		}

		if (jsonPeerConnection["/offer_socket_connection"_json_pointer] != nullptr) {
			CRM(SetOfferSocketConnectionID(jsonPeerConnection["/offer_socket_connection"_json_pointer].get<long>()), "Failed to set offer socket conncetion ID");
		}

		// Answer

		if (jsonPeerConnection["/answer"_json_pointer] != nullptr) {
			CRM(SetSDPAnswer(jsonPeerConnection["/answer"_json_pointer].get<std::string>()), "Failed to set SDP answer string");
		}

		if (jsonPeerConnection["/answer_candidates"_json_pointer] != nullptr) {
			//CR(SetSDPOffer(jsonPeerConnection["/offer"_json_pointer].get<std::string>()), "Failed to set SDP offer string");
			// TODO:
		}

		if (jsonPeerConnection["/answer_socket_connection"_json_pointer] != nullptr) {
			CRM(SetAnswerSocketConnectionID(jsonPeerConnection["/answer_socket_connection"_json_pointer].get<long>()), "Failed to set answer socket conncetion ID");
		}

	Error:
		return r;
	}

	nlohmann::json GetPeerConnectionJSON() {
		nlohmann::json jsonData;

		jsonData["id"] = std::to_string(m_peerConnectionID);

		// Offer
		jsonData["offer"] = m_strSDPOffer;
		jsonData["offer_candidates"] = nullptr;	// TODO: fill this out
		jsonData["offer_socket_connection"] = std::to_string(m_offerSocketConnectionID);

		// Answer
		jsonData["answer"] = m_strSDPAnswer;
		jsonData["answer_candidates"] = nullptr;	// TODO: fill this out
		jsonData["answer_socket_connection"] = std::to_string(m_answerSocketConnectionID);
	
		return jsonData;
	}

private:
	long m_userID;
	long m_peerUserID;
	long m_peerConnectionID;

	long m_environmentID;

	long m_offerSocketConnectionID;
	long m_answerSocketConnectionID;

	std::string m_strSDPOffer;	
	std::string m_strSDPAnswer;

	std::vector<std::string> m_strUserCandidates;
	std::vector<std::string> m_strPeerCandidates;
};

#endif	// !PEER_CONNECTION_H_