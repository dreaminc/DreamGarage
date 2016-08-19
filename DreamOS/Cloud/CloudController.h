#ifndef CLOUD_CONTROLLER_H_
#define CLOUD_CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/CloudController.h
// The base DreamCloud controller 

#include "Controller.h"
#include "CloudImp.h"
#include "User/UserFactory.h"
#include "Environment/EnvironmentController.h"
#include <memory>

class CloudController : public Controller, public std::enable_shared_from_this<CloudController> {
public:
	typedef std::function<RESULT(const std::string&)> HandleDataChannelStringMessageCallback;
	typedef std::function<RESULT(uint8_t *, int)> HandleDataChannelMessageCallback;

	RESULT RegisterDataChannelStringMessageCallback(HandleDataChannelStringMessageCallback fnHandleDataChannelStringMessageCallback);
	RESULT RegisterDataChannelMessageCallback(HandleDataChannelMessageCallback fnHandleDataChannelMessageCallback);

public:
	CloudController();
	~CloudController();

	RESULT SetCloudImp(std::unique_ptr<CloudImp> pCloudImp);

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
	RESULT OnDataChannelStringMessage(const std::string& strDataChannelMessage);
	RESULT OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

	RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage);
	RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

private:
	//UID m_uid;
	std::unique_ptr<CloudImp> m_pCloudImp;
	
	std::unique_ptr<UserController> m_pUserController;
	std::unique_ptr<EnvironmentController> m_pEnvironmentController;

private:
	HandleDataChannelStringMessageCallback m_fnHandleDataChannelStringMessageCallback;
	HandleDataChannelMessageCallback m_fnHandleDataChannelMessageCallback;
};

#endif