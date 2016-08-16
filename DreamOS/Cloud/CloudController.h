#ifndef CLOUD_CONTROLLER_H_
#define CLOUD_CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/CloudController.h
// The base DreamCloud controller 

#include "CloudImp.h"
#include "User/UserFactory.h"
#include "Environment/EnvironmentController.h"
#include <memory>

class CloudController {
public:
	CloudController();
	~CloudController();

	RESULT SetCloudImp(std::unique_ptr<CloudImp> pCloudImp);

	RESULT InitializeUser(version ver = 1.0f);

	RESULT InitializeEnvironment(long environmentID = -1);

	RESULT CreateNewURLRequest(std::wstring& strURL);

	RESULT LoginUser();
	
	RESULT Update();

	// TODO: This will attempt to connect to the first peer in the list, should make more robust
	// and expose the available peer list at the CloudController layer
	RESULT ConnectToPeer(int peerID);

	RESULT SendMessageToPeer(int peerID, std::string& strMessage);

	std::function<void(int msgID, void* data)> GetUIThreadCallback();

	void CallGetUIThreadCallback(int msgID, void* data);

private:
	UID m_uid;
	std::unique_ptr<CloudImp> m_pCloudImp;

	std::unique_ptr<UserController> m_pUserController;
	std::unique_ptr<EnvironmentController> m_pEnvironmentController;
};

#endif