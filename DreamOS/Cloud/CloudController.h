#ifndef CLOUD_CONTROLLER_H_
#define CLOUD_CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/CloudController.h
// The base DreamCloud controller 

#include "CloudImp.h"
#include <memory>

class CloudController {
public:
	CloudController() :
		m_pCloudImp(nullptr)
	{
		// empty
	}

	~CloudController() {
		// empty 
	}

	RESULT SetCloudImp(std::unique_ptr<CloudImp> pCloudImp) {
		RESULT r = R_PASS;

		m_pCloudImp = std::move(pCloudImp);
		CN(m_pCloudImp);

	Error:
		return r;
	}

	RESULT CreateNewURLRequest(std::wstring& strURL) {
		RESULT r = R_PASS;

		CNM(m_pCloudImp, "Cloud Imp not initialized");
		CRM(m_pCloudImp->CreateNewURLRequest(strURL), "Failed to create CEF URL request for %S", strURL.c_str());

	Error:
		return r;
	}

	// TODO: This will attempt to connect to the first peer in the list, should make more robust
	// and expose the available peer list at the CloudController layer
	RESULT ConnectToPeer(int peerID) {
		RESULT r = R_PASS;

		CN(m_pCloudImp);
		CR(m_pCloudImp->ConnectToPeer(peerID));

	Error:
		return r;
	}

	RESULT SendMessageToPeer(int peerID, std::string& strMessage) {
		RESULT r = R_PASS;

		CN(m_pCloudImp);
		CR(m_pCloudImp->SendMessageToPeer(peerID, strMessage));

	Error:
		return r;
	}

	std::function<void(int msgID, void* data)> GetUIThreadCallback() {
		return m_pCloudImp->GetUIThreadCallback();
	}

	void CallGetUIThreadCallback(int msgID, void* data) {
		std::function<void(int msg_id, void* data)> fnUIThreadCallback;
		return fnUIThreadCallback(msgID, data);
	}

	RESULT Update() {
		RESULT r = R_PASS;

		CR(m_pCloudImp->Update());

	Error:
		return r;
	}

private:
	UID m_uid;
	std::unique_ptr<CloudImp> m_pCloudImp;
};

#endif