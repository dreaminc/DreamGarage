#ifndef PEER_CONNECTION_CONTROLLER_H_
#define PEER_CONNECTION_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/PeerConnectionController.h
// The Peer Connection Controller

#include <string>
#include <memory>

#include "Cloud/Controller.h"

#include "json.hpp"

#include "Cloud/webrtc/WebRTCImp.h"

class User;
class Websocket;
class PeerConnection;

// TODO: This is actually a UserController - so change the name of object and file
class PeerConnectionController : public Controller, public WebRTCImp::WebRTCObserver{
public:
	class PeerConnectionControllerObserver {
	public:
		virtual RESULT OnSDPOfferSuccess(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnSDPAnswerSuccess(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnICECandidatesGatheringDone(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnDataChannelStringMessage(const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;
	};

	RESULT RegisterPeerConnectionControllerObserver(PeerConnectionControllerObserver* pPeerConnectionControllerObserver);

public:
	enum class PeerConnectionState {
		INVALID
	};

	PeerConnectionState GetState() { return m_state; }
	PeerConnectionState m_state;

private:
	enum class PeerConnectionMethod {
		INVALID
	};

	std::string GetMethodURI(PeerConnectionMethod method);
	//nlohmann::json GetPeerConnectionJSON(long peerConnectionID);

public:
	PeerConnectionController(Controller* pParentController);
	~PeerConnectionController();

	RESULT Initialize();
	RESULT ClearPeerConnections();

	//RESULT InitializeNewPeerConnection(bool fCreateOffer, bool fAddDataChannel);

	PeerConnection *CreateNewPeerConnection(long peerConnectionID, long userID, long peerUserID);
	PeerConnection* CreateNewPeerConnection(nlohmann::json jsonPeerConnection, nlohmann::json jsonOfferSocketConnection, nlohmann::json jsonAnswerSocketConnection);

	// TODO: This is kind of useless
	bool FindPeerConnectionByOfferUserID(long userID);
	PeerConnection *GetPeerConnectionByOfferUserID(long userID);

	
	bool FindPeerConnectionByAnswerUserID(long peerUserID);
	PeerConnection *GetPeerConnectionByAnswerUserID(long peerUserID);

	bool FindPeerConnectionByID(long peerConnectionID);
	PeerConnection *GetPeerConnectionByID(long peerConnectionID);

	bool IsUserIDConnected(long peerUserID);

	RESULT HandleEnvironmentSocketRequest(std::string strMethod, nlohmann::json jsonPayload);
	RESULT HandleEnvironmentSocketResponse(std::string strMethod, nlohmann::json jsonPayload);

	// WebRTCObserver
	virtual RESULT OnWebRTCConnectionStable() override;
	virtual RESULT OnWebRTCConnectionClosed() override;
	virtual RESULT OnSDPOfferSuccess() override;
	virtual RESULT OnSDPAnswerSuccess() override;
	virtual RESULT OnICECandidatesGatheringDone() override;
	virtual RESULT OnDataChannelStringMessage(const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;

	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

private:
	std::unique_ptr<WebRTCImp> m_pWebRTCImp;

	bool m_fConnected;

	bool m_fPendingMessage;
	uint64_t m_pendingMessageID;

	std::vector<PeerConnection> m_peerConnections;
	PeerConnection *m_pPeerConnectionCurrentHandshake;

	PeerConnectionControllerObserver *m_pPeerConnectionControllerObserver;
};

#endif	// ! ENVIRONMENT_CONTROLLER_H_