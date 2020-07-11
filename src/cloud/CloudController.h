#ifndef CLOUD_CONTROLLER_H_
#define CLOUD_CONTROLLER_H_

#include "core/ehm/EHM.h"

// Dream Cloud
// dos/src/cloud/CloudController.h

// The base DreamCloud controller 

#include <memory>
#include <thread>

#include "CloudImp.h"
#include "Environment/EnvironmentController.h"

#include "core/types/UID.h"
#include "core/types/valid.h"

#include "core/types/Controller.h"
#include "core/types/ControllerProxy.h"

#include "cloud/User/UserFactory.h"

class Message;
class UpdateHeadMessage; 
class UpdateHandMessage;
class AudioDataMessage;

class MenuControllerProxy;
class HTTPControllerProxy;

class hand;
class User;
class TwilioNTSInformation;
class EnvironmentAsset;
class texture;
class point;
class vector;
class quaternion;

enum class CLOUD_CONTROLLER_TYPE {
	CLOUD,
	ENVIRONMENT,
	MENU,
	USER,
	HTTP,
	WEBRTC,
	INVALID
};

class CloudControllerProxy : public ControllerProxy {
public:
	//virtual CLOUD_CONTROLLER_TYPE GetControllerType() = 0;
	//virtual RESULT RequestSubMenu(std::string strScope = "", std::string strPath = "", std::string strTitle = "") = 0;
	virtual long GetUserID() = 0;
	virtual long GetUserAvatarID() = 0;
};

class CloudController : public Controller, 
						public CloudControllerProxy,
						public std::enable_shared_from_this<CloudController>, 
						public EnvironmentController::EnvironmentControllerObserver,
						public UserController::UserControllerObserver
{
protected:
	typedef std::function<RESULT(PeerConnection*, const std::string&)> HandleDataChannelStringMessageCallback;
	typedef std::function<RESULT(PeerConnection*, uint8_t *, int)> HandleDataChannelMessageCallback;

	RESULT RegisterDataChannelStringMessageCallback(HandleDataChannelStringMessageCallback fnHandleDataChannelStringMessageCallback);
	RESULT RegisterDataChannelMessageCallback(HandleDataChannelMessageCallback fnHandleDataChannelMessageCallback);

public:
	// TODO: Replace with proxy or observer better pattern this is getting out of control

	class PeerConnectionObserver {
	public:
		virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) = 0;
		virtual RESULT OnNewSocketConnection(int seatPosition) = 0;
		virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) = 0;
		virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDataMessage) = 0;
		virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
		virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
		virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) = 0;
		virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) = 0;
	};
	
	class EnvironmentObserver {
	public:
		virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
		virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
		virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
		virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) = 0;
		virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;
		virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) = 0;

		virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
		virtual RESULT OnSendCameraPlacement() = 0;
		virtual RESULT OnStopSendingCameraPlacement() = 0;
		virtual RESULT OnReceiveCameraPlacement(long userID) = 0;
		virtual RESULT OnStopReceivingCameraPlacement() = 0;
	};

	class UserObserver {
	public:
		virtual RESULT OnDreamVersion(version dreamVersion) = 0;
		virtual RESULT OnAPIConnectionCheck(bool fIsConnected) = 0;

		virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) = 0;
		virtual RESULT OnSetSettings() = 0;

		virtual RESULT OnLogin() = 0;
		virtual RESULT OnLogout() = 0;
		virtual RESULT OnPendLogout() = 0;
		virtual RESULT OnSwitchTeams() = 0;

		virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) = 0;
		virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) = 0;
		virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) = 0;
	};

	RESULT RegisterPeerConnectionObserver(PeerConnectionObserver* pPeerConnectionControllerObserver);
	RESULT RegisterEnvironmentObserver(EnvironmentObserver* pEnvironmentObserver);
	RESULT RegisterUserObserver(UserObserver* pUserObserver);

private:
	PeerConnectionObserver *m_pPeerConnectionObserver = nullptr;
	EnvironmentObserver *m_pEnvironmentObserver = nullptr;
	UserObserver *m_pUserObserver = nullptr;
	
public:
	RESULT SendDataMessage(long userID, Message *pDataMessage);
	RESULT BroadcastDataMessage(Message *pDataMessage);

	// Video
	RESULT StartVideoStreaming(const std::string &strVideoTrackLabel, int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat);
	RESULT StopVideoStreaming(const std::string &strVideoTrackLabel);
	bool IsVideoStreamingRunning(const std::string &strVideoTrackLabel);
	RESULT BroadcastVideoFrame(const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);
	RESULT BroadcastTextureFrame(const std::string &strVideoTrackLabel, texture *pTexture, int level, PIXEL_FORMAT pixelFormat);

	// Audio 
	RESULT BroadcastAudioPacket(const std::string &strAudioTrackLabel, const AudioPacket &pendingAudioPacket);
	float GetRunTimeMicAverage();
	RESULT SetRunTimeMicAverage(float runTimeMicAverage);

	// TODO: Generalize channels

	// TODO: Allow creation / deletion of channels ad-hoc

public:
	CloudController();
	~CloudController();

	// CloudControllerProxy
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override { return CLOUD_CONTROLLER_TYPE::CLOUD; }
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) override { return R_NOT_IMPLEMENTED_WARNING; }

	RESULT SetCloudImp(std::unique_ptr<CloudImp> pCloudImp);

	RESULT Start(bool fLogin = true);
	RESULT Start(std::string strUsername, std::string strPassword, long environmentID);
	RESULT Stop();

	virtual RESULT Initialize() override;
	RESULT InitializeUser(version ver = 1.0f);
	RESULT InitializeEnvironment(long environmentID = -1);
	RESULT CreateNewURLRequest(std::wstring& strURL);
	//RESULT LoginUser();
	RESULT LoginUser(std::string strUsername, std::string strPassword, std::string strOTK = "INVALIDONETIMEKEY");
	RESULT Update();
	RESULT Login();

	User GetUser();
	TwilioNTSInformation GetTwilioNTSInformation();
	bool IsUserLoggedIn();
	bool IsEnvironmentConnected();

	virtual long GetUserID() override;
	virtual long GetUserAvatarID() override;

	//RESULT CreateSDPOfferAnswer(std::string strSDPOfferJSON);
	//std::string GetSDPOfferString();
	//RESULT InitializeConnection(bool fMaster, bool fAddDataChannel);
	
	//RESULT AddIceCandidates();

	RESULT PrintEnvironmentPeerList();

	//std::function<void(int msgID, void* data)> GetUIThreadCallback();

	void CallGetUIThreadCallback(int msgID, void* data);

	// WebRTC Callbacks
	// TODO: Convert to observer interface (clean up)
	RESULT OnICECandidatesGatheringDone();

	// EnvironmentControllerObserver
	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	virtual RESULT OnNewSocketConnection(int seatPosition) override;
	virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) override;
	virtual RESULT OnDataChannelStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(PeerConnection* pPeerConnection, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;

	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;
	virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;
	virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;
	virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;
	virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;

	virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;
	virtual RESULT OnSendCameraPlacement() override;
	virtual RESULT OnStopSendingCameraPlacement() override;
	virtual RESULT OnReceiveCameraPlacement(long userID) override;
	virtual RESULT OnStopReceivingCameraPlacement() override;

	virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) override;
	virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) override;

	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) override;

	// UserControllerObserver
	virtual RESULT OnDreamVersion(version dreamVersion) override;
	virtual RESULT OnAPIConnectionCheck(bool fIsConnected) override;
	virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) override;
	virtual RESULT OnSetSettings() override;

	virtual RESULT OnLogin() override;
	virtual RESULT OnLogout() override;
	virtual RESULT OnPendLogout() override;
	virtual RESULT OnSwitchTeams() override;

	virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override;
	virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override;
	virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override;


	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT BroadcastDataChannelStringMessage(std::string& strMessage);
	RESULT BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);


	// Proxy Objects
public:
	ControllerProxy* GetControllerProxy(CLOUD_CONTROLLER_TYPE controllerType);
	RESULT RegisterControllerObserver(CLOUD_CONTROLLER_TYPE controllerType, ControllerObserver *pControllerObserver);

private:
	MenuControllerProxy* GetMenuControllerProxy();
	WebRTCImpProxy* GetWebRTCControllerProxy();
	UserControllerProxy* GetUserControllerProxy();
	HTTPControllerProxy* GetHTTPControllerProxy();
	EnvironmentControllerProxy* GetEnvironmentControllerProxy();

private:
	//UID m_uid;
	std::unique_ptr<CloudImp> m_pCloudImp;
	
	std::unique_ptr<UserController> m_pUserController;
	std::unique_ptr<EnvironmentController> m_pEnvironmentController;

private:
	HandleDataChannelStringMessageCallback m_fnHandleDataChannelStringMessageCallback;
	HandleDataChannelMessageCallback m_fnHandleDataChannelMessageCallback;

	std::thread	m_cloudThread;
	bool m_fRunning;
	bool m_fLoginOnStart = false;

	RESULT CloudThreadProcess();
	RESULT CloudThreadProcessParams(std::string strUsername, std::string strPassword, long environmentID);

	// TEMP:
	float m_runTimeMicAverage = 0.0f;
};

#endif