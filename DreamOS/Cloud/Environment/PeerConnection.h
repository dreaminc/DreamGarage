#ifndef PEER_CONNECTION_H_
#define PEER_CONNECTION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/EnvironmentPeer.h
// Environment Peer Object

#include <string>
#include <vector>

// TODO: This may result in cyclic dependency so might need to move it
#include "Cloud/webrtc/WebRTCPeerConnection.h"

const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";

class PeerConnection {
public:
	PeerConnection(long userID, long peerUserID, long peerConnectionID) :
		m_offerUserID(userID),
		m_answerUserID(peerUserID),
		m_peerConnectionID(peerConnectionID),
		m_fWebRTCConnectionStable(false)
	{
		// empty
	}

	PeerConnection(nlohmann::json jsonPeerConnection, nlohmann::json jsonOfferSocketConnection, nlohmann::json jsonAnswerSocketConnection) :
		m_peerConnectionID(-1),
		m_offerUserID(-1),
		m_answerUserID(-1),
		m_fWebRTCConnectionStable(false)
	{
		UpdatePeerConnectionFromJSON(jsonPeerConnection);
		UpdateOfferSocketConnectionFromJSON(jsonOfferSocketConnection);
		UpdateAnswerSocketConnectionFromJSON(jsonAnswerSocketConnection);
	}

	RESULT Print() {
		DEBUG_LINEOUT("User ID %d Peer ID %d Peer Connection ID %d", m_offerUserID, m_answerUserID, m_peerConnectionID);
		
		DEBUG_LINEOUT("SDP Offer: %s", m_strSDPOffer.c_str());
		DEBUG_LINEOUT("%d Offer Candidates:", static_cast<int>(m_offerICECandidates.size()));
		for (auto &iceCandidate : m_offerICECandidates) {
			iceCandidate.Print();
		}
		DEBUG_LINEOUT(" ");

		DEBUG_LINEOUT("SDP Answer: %s", m_strSDPAnswer.c_str());
		DEBUG_LINEOUT("%d Answer Candidates:", static_cast<int>(m_answerICECandidates.size()));
		for (auto &iceCandidate : m_answerICECandidates) {
			iceCandidate.Print();
		}
		DEBUG_LINEOUT(" ");

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
		m_offerICECandidates.clear();
		return R_PASS;
	}

	RESULT AddUserCandidate(std::string strCandidate) {
		m_offerICECandidates.push_back(strCandidate);
		return R_PASS;
	}

	RESULT ClearPeerCandidates() {
		m_answerICECandidates.clear();
		return R_PASS;
	}

	RESULT AddPeerCandidate(std::string strCandidate) {
		m_answerICECandidates.push_back(strCandidate);
		return R_PASS;
	}

	long GetOfferUserID() { return m_offerUserID; }
	RESULT SetOfferUserID(long userID) { m_offerUserID = userID; return R_PASS; }

	long GetAnswerUserID() { return m_answerUserID; }
	RESULT SetAnswerUserID(long peerUserID) { m_answerUserID = peerUserID; return R_PASS; }

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

	const std::list<WebRTCICECandidate> GetUserCandidates() { return m_offerICECandidates; }
	const std::list<WebRTCICECandidate> GetPeerCandidates() { return m_answerICECandidates; }

	RESULT UpdateOfferSocketConnectionFromJSON(nlohmann::json jsonOfferSocketConnection) {
		RESULT r = R_PASS;

		long userID = jsonOfferSocketConnection["/user"_json_pointer].get<long>();
		long environmentID = jsonOfferSocketConnection["/environment"_json_pointer].get<long>();

		CR(SetOfferUserID(userID));
		CR(SetEnvironmentID(environmentID));

	Error:
		return r;
	}

	RESULT UpdateAnswerSocketConnectionFromJSON(nlohmann::json jsonAnswerSocketConnection) {
		RESULT r = R_PASS;

		long peerUserId = jsonAnswerSocketConnection["/user"_json_pointer].get<long>();
		long environmentID = jsonAnswerSocketConnection["/environment"_json_pointer].get<long>();

		CR(SetAnswerUserID(peerUserId));
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
			m_offerICECandidates.clear();

			for (auto &jsonICECandidate : jsonPeerConnection["/offer_candidates"_json_pointer]) {
				std::string strSDPCandidate = jsonICECandidate[kCandidateSdpName].get<std::string>();
				std::string strSDPMediaID = jsonICECandidate[kCandidateSdpMidName].get<std::string>();
				int SDPMediateLineIndex = jsonICECandidate[kCandidateSdpMlineIndexName].get<int>();
				
				WebRTCICECandidate iceCandidate(strSDPCandidate, strSDPMediaID, SDPMediateLineIndex);
				m_offerICECandidates.push_back(iceCandidate);
			}
		}

		if (jsonPeerConnection["/offer_socket_connection"_json_pointer] != nullptr) {
			CRM(SetOfferSocketConnectionID(jsonPeerConnection["/offer_socket_connection"_json_pointer].get<long>()), "Failed to set offer socket conncetion ID");
		}

		// Answer
		// TODO: Duplicated code per above

		if (jsonPeerConnection["/answer"_json_pointer] != nullptr) {
			CRM(SetSDPAnswer(jsonPeerConnection["/answer"_json_pointer].get<std::string>()), "Failed to set SDP answer string");
		}

		if (jsonPeerConnection["/answer_candidates"_json_pointer] != nullptr) {
			m_answerICECandidates.clear();

			for (auto &jsonICECandidate : jsonPeerConnection["/answer_candidates"_json_pointer]) {
				std::string strSDPCandidate = jsonICECandidate[kCandidateSdpName].get<std::string>();
				std::string strSDPMediaID = jsonICECandidate[kCandidateSdpMidName].get<std::string>();
				int SDPMediateLineIndex = jsonICECandidate[kCandidateSdpMlineIndexName].get<int>();

				WebRTCICECandidate iceCandidate(strSDPCandidate, strSDPMediaID, SDPMediateLineIndex);
				m_answerICECandidates.push_back(iceCandidate);
			}
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

		if (m_offerICECandidates.size() > 0)
			jsonData["offer_candidates"] = GetCandidatesJSON(m_offerICECandidates);
		else
			jsonData["offer_candidates"] = nullptr;

		jsonData["offer_socket_connection"] = std::to_string(m_offerSocketConnectionID);

		// Answer
		jsonData["answer"] = m_strSDPAnswer;

		if (m_answerICECandidates.size() > 0)
			jsonData["answer_candidates"] = GetCandidatesJSON(m_answerICECandidates);
		else
			jsonData["answer_candidates"] = nullptr;

		jsonData["answer_socket_connection"] = std::to_string(m_answerSocketConnectionID);
	
		return jsonData;
	}

	nlohmann::json GetCandidatesJSON(std::list<WebRTCICECandidate> iceCandidates) {
		nlohmann::json jsonData = nlohmann::json::array();

		for (auto &iceCandidate : iceCandidates) {
			nlohmann::json jsonICECandidate;

			jsonICECandidate[kCandidateSdpName] = iceCandidate.m_strSDPCandidate;
			jsonICECandidate[kCandidateSdpMidName] = iceCandidate.m_strSDPMediaID;
			jsonICECandidate[kCandidateSdpMlineIndexName] = iceCandidate.m_SDPMediateLineIndex;

			jsonData.push_back(jsonICECandidate);
		}

		return jsonData;
	}

	std::list<WebRTCICECandidate> GetOfferCandidates() { return m_offerICECandidates; }
	RESULT SetOfferCandidates(std::list<WebRTCICECandidate> iceCandidates) {
		m_offerICECandidates = iceCandidates;
		return R_PASS;
	}

	std::list<WebRTCICECandidate> GetAnswerCandidates() { return m_answerICECandidates; }
	RESULT SetAnswerCandidates(std::list<WebRTCICECandidate> iceCandidates) {
		m_answerICECandidates = iceCandidates;
		return R_PASS;
	}

	bool IsWebRTCConnectionStable() {
		return m_fWebRTCConnectionStable;
	}

	RESULT SetWebRTCConnectionStable() {
		m_fWebRTCConnectionStable = true;
		return R_PASS;
	}

	RESULT SetWebRTCConnectionClosed() {
		m_fWebRTCConnectionStable = false;
		return R_PASS;
	}

	bool IsWebRTCConnected() {
		return m_fWebRTCConnectionStable;
	}

private:
	long m_offerUserID;
	long m_answerUserID;
	long m_peerConnectionID;

	long m_environmentID;

	long m_offerSocketConnectionID;
	long m_answerSocketConnectionID;

	std::string m_strSDPOffer;	
	std::string m_strSDPAnswer;

	std::list<WebRTCICECandidate> m_offerICECandidates;
	std::list<WebRTCICECandidate> m_answerICECandidates;

	bool m_fWebRTCConnectionStable;
};

#endif	// !PEER_CONNECTION_H_