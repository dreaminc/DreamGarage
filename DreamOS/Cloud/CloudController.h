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

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/quaternion.h"
#include "Primitives/hand.h"

class Message;
class UpdateHeadMessage; 
class UpdateHandMessage;

typedef std::function<RESULT(long, Message*)> HandleDataMessageCallback;
typedef std::function<RESULT(long, UpdateHeadMessage*)> HandleHeadUpdateMessageCallback;
typedef std::function<RESULT(long, UpdateHandMessage*)> HandleHandUpdateMessageCallback;

class CloudController : public Controller, public std::enable_shared_from_this<CloudController>, public EnvironmentController::EnvironmentControllerObserver,
						public Subscriber<CmdPromptEvent> {
protected:
	typedef std::function<RESULT(const std::string&)> HandleDataChannelStringMessageCallback;
	typedef std::function<RESULT(uint8_t *, int)> HandleDataChannelMessageCallback;

	RESULT RegisterDataChannelStringMessageCallback(HandleDataChannelStringMessageCallback fnHandleDataChannelStringMessageCallback);
	RESULT RegisterDataChannelMessageCallback(HandleDataChannelMessageCallback fnHandleDataChannelMessageCallback);

public:
	RESULT RegisterDataMessageCallback(HandleDataMessageCallback fnHandleDataMessageCallback);
	RESULT RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback);
	RESULT RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback);

	RESULT SendDataMessage(long userID, Message *pDataMessage);
	RESULT SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT SendUpdateHandMessage(long userID, hand::HandState handState);

public:
	CloudController();
	~CloudController();

	RESULT SetCloudImp(std::unique_ptr<CloudImp> pCloudImp);

	RESULT Initialize();
	RESULT InitializeUser(version ver = 1.0f);
	RESULT InitializeEnvironment(long environmentID = -1);
	RESULT CreateNewURLRequest(std::wstring& strURL);
	RESULT LoginUser();
	RESULT Update();

	RESULT CreateSDPOfferAnswer(std::string strSDPOfferJSON);
	std::string GetSDPOfferString();
	RESULT InitializeConnection(bool fMaster, bool fAddDataChannel);
	RESULT AddIceCandidates();

	// TODO: This will attempt to connect to the first peer in the list, should make more robust
	// and expose the available peer list at the CloudController layer
	// TODO: CLEAN UP MIGHT NOT DO STUFF ANYMORE
	RESULT ConnectToPeer(int peerID);
	RESULT PrintEnvironmentPeerList();

	std::function<void(int msgID, void* data)> GetUIThreadCallback();

	void CallGetUIThreadCallback(int msgID, void* data);

	// WebRTC Callbacks
	// TODO: Convert to observer interface (clean up)
	RESULT OnICECandidatesGatheringDone();
	virtual RESULT OnDataChannelStringMessage(const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;

	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

private:
	//UID m_uid;
	std::unique_ptr<CloudImp> m_pCloudImp;
	
	std::unique_ptr<UserController> m_pUserController;
	std::unique_ptr<EnvironmentController> m_pEnvironmentController;

private:
	HandleDataChannelStringMessageCallback m_fnHandleDataChannelStringMessageCallback;
	HandleDataChannelMessageCallback m_fnHandleDataChannelMessageCallback;

	HandleDataMessageCallback m_fnHandleDataMessageCallback;
	HandleHeadUpdateMessageCallback m_fnHandleHeadUpdateMessageCallback;
	HandleHandUpdateMessageCallback m_fnHandleHandUpdateMessageCallback;
};

#endif