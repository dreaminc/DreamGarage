#ifndef CLOUD_CONTROLLER_H_
#define CLOUD_CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#include "DreamConsole/DreamConsole.h"

// DREAM OS
// DreamOS/Cloud/CloudController.h
// The base DreamCloud controller 

#include "Controller.h"
#include "CloudImp.h"
#include "User/UserFactory.h"
#include "Environment/EnvironmentController.h"
#include <memory>
#include <thread>

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/quaternion.h"
#include "Primitives/hand.h"

class ControllerProxy;
class Message;
class UpdateHeadMessage; 
class UpdateHandMessage;
class AudioDataMessage;

class MenuControllerProxy;
class HTTPControllerProxy;

class User;
class TwilioNTSInformation;
class EnvironmentAsset;

enum class CLOUD_CONTROLLER_TYPE {
	CLOUD,
	ENVIRONMENT,
	MENU,
	USER,
	HTTP,
	INVALID
};

class CloudControllerProxy : public ControllerProxy {
public:
	//virtual CLOUD_CONTROLLER_TYPE GetControllerType() = 0;
	//virtual RESULT RequestSubMenu(std::string strScope = "", std::string strPath = "", std::string strTitle = "") = 0;
	virtual long GetUserID() = 0;
};

class CloudController : public Controller, 
						public CloudControllerProxy,
						public std::enable_shared_from_this<CloudController>, 
						public EnvironmentController::EnvironmentControllerObserver,
						public Subscriber<CmdPromptEvent> 
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
		virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDataMessage) = 0;
		virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) = 0;
		virtual RESULT OnAudioData(PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
	};
	
	class EnvironmentObserver {
	public:
		virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) = 0;
	};

	RESULT RegisterPeerConnectionObserver(PeerConnectionObserver* pPeerConnectionControllerObserver);
	RESULT RegisterEnvironmentObserver(EnvironmentObserver* pEnvironmentObserver);

private:
	PeerConnectionObserver *m_pPeerConnectionObserver = nullptr;
	EnvironmentObserver *m_pEnvironmentObserver = nullptr;
	
public:
	RESULT SendDataMessage(long userID, Message *pDataMessage);
	//RESULT SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	//RESULT SendUpdateHandMessage(long userID, hand::HandState handState);

	RESULT BroadcastDataMessage(Message *pDataMessage);
	//RESULT BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	//RESULT BroadcastUpdateHandMessage(hand::HandState handState);

public:
	CloudController();
	~CloudController();

	// CloudControllerProxy
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() override { return CLOUD_CONTROLLER_TYPE::CLOUD; }
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) override { return R_NOT_IMPLEMENTED; }

	RESULT SetCloudImp(std::unique_ptr<CloudImp> pCloudImp);

	RESULT Start();
	RESULT Stop();

	RESULT Initialize();
	RESULT InitializeUser(version ver = 1.0f);
	RESULT InitializeEnvironment(long environmentID = -1);
	RESULT CreateNewURLRequest(std::wstring& strURL);
	//RESULT LoginUser();
	RESULT LoginUser(std::string strUsername, std::string strPassword, std::string strOTK);
	RESULT Update();
	RESULT Login();

	User GetUser();
	TwilioNTSInformation GetTwilioNTSInformation();
	bool IsUserLoggedIn();
	bool IsEnvironmentConnected();

	virtual long GetUserID() override;

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
	virtual RESULT OnDataChannelStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(PeerConnection* pPeerConnection, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;
	virtual RESULT OnAudioData(PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmnetAsset) override;

	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT BroadcastDataChannelStringMessage(std::string& strMessage);
	RESULT BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

	
	// Proxy Objects
public:
	ControllerProxy* GetControllerProxy(CLOUD_CONTROLLER_TYPE controllerType);
	RESULT RegisterControllerObserver(CLOUD_CONTROLLER_TYPE controllerType, ControllerObserver *pControllerObserver);

private:
	MenuControllerProxy* GetMenuControllerProxy();
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
	RESULT ProcessingThread();
};

#endif