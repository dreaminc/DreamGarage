#ifndef PEER_CONNECTION_CONTROLLER_H_
#define PEER_CONNECTION_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Environment/PeerConnectionController.h
// The Peer Connection Controller

#include <string>
#include <memory>
#include <mutex>

#include "Cloud/Controller.h"

#include "json.hpp"

#include "Cloud/webrtc/WebRTCImp.h"

#include "Cloud/User/User.h"
#include "Cloud/User/TwilioNTSInformation.h"

class Websocket;
class PeerConnection;

// TODO: This is actually a UserController - so change the name of object and file
class PeerConnectionController : public Controller, public WebRTCImp::WebRTCObserver {
	friend class CloudController;
	friend class EnvironmentController;
public:
	class PeerConnectionControllerObserver {
	public:
		virtual RESULT OnSDPOfferSuccess(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnSDPAnswerSuccess(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnICECandidatesGatheringDone(PeerConnection *pPeerConnection) = 0;

		// TODO: Switch to Peer User ID
		virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) = 0;
		virtual RESULT OnNewSocketConnection(int seatPosition) = 0;
		virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnDataChannelStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(PeerConnection* pPeerConnection, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;
		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
		virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;

		virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) = 0;
		virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) = 0;

		virtual long GetUserID() = 0;
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

	//PeerConnection *CreateNewPeerConnection(long peerConnectionID, long userID, long peerUserID);	// DEADBEEF: ?
	PeerConnection* CreateNewPeerConnection(long userID, nlohmann::json jsonPeerConnection, nlohmann::json jsonOfferSocketConnection, nlohmann::json jsonAnswerSocketConnection);
	RESULT ClosePeerConnection(PeerConnection *pPeerConnection);
	RESULT DeletePeerConnection(PeerConnection *pPeerConnection);
	RESULT CloseAllPeerConnections();
	
	// TODO: This is kind of useless
	bool FindPeerConnectionByOfferUserID(long offerUserID);
	PeerConnection *GetPeerConnectionByOfferUserID(long offerUserID);

	bool FindPeerConnectionByPeerUserID(long peerUserID);
	PeerConnection *GetPeerConnectionByPeerUserID(long peerUserID);
	
	bool FindPeerConnectionByAnswerUserID(long answerUserID);
	PeerConnection *GetPeerConnectionByAnswerUserID(long answerUserID);

	bool FindPeerConnectionByID(long peerConnectionID);
	PeerConnection *GetPeerConnectionByID(long peerConnectionID);

	bool IsUserIDConnected(long peerUserID);

	RESULT HandleEnvironmentSocketRequest(std::string strMethod, nlohmann::json jsonPayload);
	RESULT HandleEnvironmentSocketResponse(std::string strMethod, nlohmann::json jsonPayload);

	// WebRTCObserver
	virtual RESULT OnWebRTCConnectionStable(long peerConnectionID) override;
	virtual RESULT OnWebRTCConnectionClosed(long peerConnectionID) override;
	virtual RESULT OnSDPOfferSuccess(long peerConnectionID) override;
	virtual RESULT OnSDPAnswerSuccess(long peerConnectionID) override;
	virtual RESULT OnICECandidatesGatheringDone(long peerConnectionID) override;
	virtual RESULT OnIceConnectionChange(long peerConnectionID, WebRTCIceConnection::state webRTCIceConnectionState) override;
	virtual RESULT OnDataChannelStringMessage(long peerConnectionID, const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, long peerConnectionID, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	virtual RESULT OnVideoFrame(long peerConnectionID, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	virtual RESULT OnRenegotiationNeeded(long peerConnectionID) override;
	virtual RESULT OnDataChannel(long peerConnectionID) override;
	virtual RESULT OnAudioChannel(long peerConnectionID) override;

	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT BroadcastDataChannelStringMessage(std::string& strMessage);
	RESULT BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	// Video
	RESULT BroadcastVideoFrame(uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);
	RESULT StartVideoStreaming(int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat);
	RESULT StopVideoStreaming();
	bool IsVideoStreamingRunning();

	// Audio
	RESULT BroadcastAudioPacket(const std::string &strAudioTrackLabel, const AudioPacket &pendingAudioPacket);
	float GetRunTimeMicAverage();

	long GetUserID();

	RESULT SetUser(User currentUser);
	RESULT SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation);

	WebRTCImpProxy* GetWebRTCControllerProxy();

private:
	RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection);
	RESULT OnNewSocketConnection(int seatPosition);
	RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection);

private:
	std::unique_ptr<WebRTCImp> m_pWebRTCImp;

	bool m_fConnected;

	bool m_fPendingMessage;
	uint64_t m_pendingMessageID;

	std::vector<PeerConnection*> m_peerConnections;
	//std::recursive_mutex m_peerConnections_mutex;
	//PeerConnection *m_pPeerConnectionCurrentHandshake;	// TODO: This is no longer needed, all connections should be self contained

	PeerConnectionControllerObserver *m_pPeerConnectionControllerObserver;
};

#endif	// ! ENVIRONMENT_CONTROLLER_H_