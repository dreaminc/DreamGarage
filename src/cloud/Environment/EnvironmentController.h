#ifndef ENVIRONMENT_CONTROLLER_H_
#define ENVIRONMENT_CONTROLLER_H_

#include "core/ehm/EHM.h"

// Dream Cloud Environment 
// dos/src/cloud/Environment/EnvironmentController.h

// The Environment Controller

#include <string>
#include <memory>

#include "core/types/Controller.h"

#include "Environment.h"
#include "EnvironmentPeer.h"
#include "PeerConnectionController.h"
#include "cloud/Menu/MenuController.h"

#include "core/types/ControllerProxy.h"

class User;
class TwilioNTSInformation;
class Websocket;
class CloudMessage;
class EnvironmentAsset;
class EnvironmentShare;
class CameraController;

class EnvironmentControllerProxy : public ControllerProxy {
public:
	//virtual CLOUD_CONTROLLER_TYPE GetControllerType() = 0;
	virtual RESULT RequestOpenAsset(std::string strStorageProviderScope = "", std::string strPath = "", std::string strTitle = "") = 0;
	virtual RESULT RequestCloseAsset(long assetID) = 0;
	virtual RESULT RequestShareAsset(long assetID, std::string strShareType) = 0;
	virtual RESULT RequestStopSharing(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
	virtual RESULT RequestCurrentScreenShare(std::string strShareType) = 0;

	virtual RESULT RequestForm(std::string strKey) = 0;

	virtual RESULT RequestOpenCamera() = 0;
	virtual RESULT RequestCloseCamera(long assetID) = 0;
	virtual RESULT RequestShareCamera(long assetID) = 0;
};

// TODO: This is actually a UserController - so change the name of object and file
class EnvironmentController : public Controller, 
							  public PeerConnectionController::PeerConnectionControllerObserver, 
							  public EnvironmentControllerProxy {
	friend class MenuController;
	friend class CloudController;
	friend class UserController;
public:
	enum class state {
		UNINITIALIZED,
		SOCKET_INITIALIZED,
		SOCKET_CONNECTING,
		SOCKET_CONNECTED,
		CREATING_ENVIRONMENT_USER,
		ENVIRONMENT_USER_CREATED,
		ENVIRONMENT_PEER_LIST_REQUESTED,
		ENVIRONMENT_PEER_LIST_RECEIVED,
		ENVIRONMENT_CONNECTED_AND_READY,

		// Peer Connection
		// TODO: Move to Peer Connection?
		SET_SDP_OFFER,
		SET_SDP_ANSWER,
		SET_OFFER_CANDIDATES,
		SET_ANSWER_CANDIDATES,
		CREATE_ICE_CANDIDATES,

		// Menu
		MENU_API_REQUEST,

		// Assets
		ENVIRONMENT_ASSET_SHARE,
		ENVIRONMENT_STOP_SHARING,
		ENVIRONMENT_ASSET_OPEN,
		ENVIRONMENT_ASSET_CLOSE,
		ENVIRONMENT_GET_BY_SHARE_TYPE,

		// Camera
		ENVIRONMENT_CAMERA_OPEN,
		ENVIRONMENT_CAMERA_CLOSE,
		ENVIRONMENT_CAMERA_SHARE_PLACEMENT,

		// User
		USER_GET_SETTINGS,
		USER_SET_SETTINGS,

		// Forms
		FORM_SETTINGS,
		FORM_GET_FORM,

		INVALID
	};

public:
	enum class EnvironmentMethod {
		CONNECT_SOCKET,
		INVALID
	};

public:
	// TODO: Convert to a proper controller observer pattern?
	class EnvironmentControllerObserver {
	public:
		virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) = 0;
		virtual RESULT OnNewSocketConnection(int seatPosition) = 0;
		virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnDataChannelStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnDataChannelMessage(PeerConnection* pPeerConnection, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;
		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
		
		virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) = 0;
		virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) = 0;
		
		// Opening and sharing
		virtual long GetUserID() = 0;
		virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
		virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
		virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;;
		virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
		virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;

		// Virtual Camera
		virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnSendCameraPlacement() = 0;
		virtual RESULT OnStopSendingCameraPlacement() = 0;
		virtual RESULT OnReceiveCameraPlacement(long userID) = 0;
		virtual RESULT OnStopReceivingCameraPlacement() = 0;

		// Forms
		virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) = 0;
	};

	RESULT RegisterEnvironmentControllerObserver(EnvironmentControllerObserver* pEnvironmentControllerObserver);

public:
	EnvironmentController(Controller* pParentController, long environmentID);
	~EnvironmentController();

public:
	RESULT Initialize();

	RESULT ConnectToEnvironmentSocket(User user, long environmentID);
	RESULT DisconnectFromEnvironmentSocket();
	RESULT CreateEnvironmentUser(User user);	// TODO: This is deprecated
	RESULT GetEnvironmentPeerList(User user);

	// TODO: New Server Integration
	nlohmann::json CreateEnvironmentMessage(User user, PeerConnection *pPeerConnection, std::string strMethod);
	nlohmann::json CreateICECandidateEnvironmentMessage(User user, PeerConnection *pPeerConnection, WebRTCICECandidate *pICECandidate, bool fOfferer);
	RESULT SetSDPOffer(User user, PeerConnection *pPeerConnection);
	RESULT SetSDPAnswer(User user, PeerConnection *pPeerConnection);
	RESULT SetOfferCandidates(User user, PeerConnection *pPeerConnection);
	RESULT SetAnswerCandidates(User user, PeerConnection *pPeerConnection);
	RESULT CreateICECandidate(User user, WebRTCICECandidate* pICECandidate, PeerConnection *pPeerConnection, bool fOfferer);

	RESULT UpdateEnvironmentUser();
	RESULT PrintEnvironmentPeerList();
	bool HasPeerConnections();

	// EnvironmentControllerProxy
	// TODO: Note - Register Controller Observer pattern needs to be fixed here
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override;
	virtual RESULT RequestOpenAsset(std::string strStorageProviderScope = "", std::string strPath = "", std::string strTitle = "") override;
	virtual RESULT RequestCloseAsset(long assetID) override;
	virtual RESULT RequestShareAsset(long assetID, std::string strShareType) override;
	virtual RESULT RequestStopSharing(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;
	virtual RESULT RequestCurrentScreenShare(std::string strShareType) override;

	virtual RESULT RequestForm(std::string strKey) override;

	virtual RESULT RequestOpenCamera() override;
	virtual RESULT RequestCloseCamera(long assetID) override;
	virtual RESULT RequestShareCamera(long assetID) override;

	RESULT OnOpenAsset(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnCloseAsset(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnSharedAsset(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnSendAsset(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnReceiveAsset(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnStopSending(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnStopReceiving(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnGetByShareType(std::shared_ptr<CloudMessage> pCloudMessage);

	RESULT OnOpenCamera(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnCloseCamera(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnSendCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnReceiveCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnStopSendingCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage);
	RESULT OnStopReceivingCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage);

	RESULT OnGetForm(std::shared_ptr<CloudMessage> pCloudMessage);

	RESULT OnEnvironmentSocketPing(std::shared_ptr<CloudMessage> pCloudMessage);

	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) override { return R_NOT_IMPLEMENTED_WARNING; }

	long GetUserID();

	// TODO: Temporary 
	//RESULT InitializeNewPeerConnection(bool fCreateOffer, bool fAddDataChannel);

protected:
	RESULT SendEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudRequest, state newState);

private:
	RESULT SendEnvironmentSocketData(const std::string& strData, state newState);
	RESULT InitializeWebsocket(std::string& strURI);

	void HandleWebsocketMessage(const std::string& strMessage);
	void HandleWebsocketConnectionOpen();
	void HandleWebsocketConnectionClose();
	void HandleWebsocketConnectionFailed();

	RESULT ClearPeerList();
	RESULT AddNewPeer(long userID, long environmentID, const std::string& strSDPOffer);
	RESULT UpdatePeer(long userID, long environmentID, const std::string& strSDPOffer);
	bool FindPeerByUserID(long userID);
	EnvironmentPeer *GetPeerByUserID(long userID);
	
	std::string GetMethodURI(EnvironmentMethod userMethod);

	// PeerConnectionControllerObserver
	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	virtual RESULT OnNewSocketConnection(int seatPosition) override;
	virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) override;
	virtual RESULT OnDataChannelStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(PeerConnection* pPeerConnection, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	virtual RESULT OnSDPOfferSuccess(PeerConnection *pPeerConnection) override;
	virtual RESULT OnSDPAnswerSuccess(PeerConnection *pPeerConnection) override;
	virtual RESULT OnICECandidatesGatheringDone(PeerConnection *pPeerConnection) override;
	virtual RESULT OnICECandidateGathered(WebRTCICECandidate *pICECandidate, PeerConnection *pPeerConnection) override;

	virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) override;
	virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) override;

public:
	long GetEnvironmentID() { return m_environment.GetEnvironmentID(); }
	RESULT SetEnvironmentID(long environmentID) { return m_environment.SetEnvironmentID(environmentID); }
	bool IsUserIDConnected(long peerUserID);

	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT BroadcastDataChannelStringMessage(std::string& strMessage);
	RESULT BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	// Video
	RESULT BroadcastVideoFrame(const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);
	RESULT StartVideoStreaming(const std::string &strVideoTrackLabel, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat);
	RESULT StopVideoStreaming(const std::string &strVideoTrackLabel);
	bool IsVideoStreamingRunning(const std::string &strVideoTrackLabel);

	// Audio
	RESULT BroadcastAudioPacket(const std::string &strAudioTrackLabel, const AudioPacket &pendingAudioPacket);
	float GetRunTimeMicAverage();

	RESULT SetUser(User currentUser);
	RESULT SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation);

	bool IsEnvironmentSocketConnected();

	// Menu Controller Proxy
	MenuControllerProxy* GetMenuControllerProxy();

	// Environment Controller Proxy
	EnvironmentControllerProxy* GetEnvironmentControllerProxy();
	
	WebRTCImpProxy* GetWebRTCControllerProxy();

public:
	EnvironmentController::state GetState() {
		return m_state;
	}

private:
	bool m_fConnected;
	EnvironmentController::state m_state;

	bool m_fPendingMessage;
	uint64_t m_pendingMessageID;

	Environment m_environment;
	std::unique_ptr<Websocket> m_pEnvironmentWebsocket;

	std::vector<EnvironmentPeer> m_environmentPeers;

	std::unique_ptr<PeerConnectionController> m_pPeerConnectionController;
	std::unique_ptr<MenuController> m_pMenuController;

	std::unique_ptr<CameraController> m_pCameraController;

	EnvironmentControllerObserver *m_pEnvironmentControllerObserver;

	std::vector<EnvironmentShare*> m_activeShares;
};

#endif	// ! ENVIRONMENT_CONTROLLER_H_