#ifndef CLOUD_IMP_H_
#define CLOUD_IMP_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/CloudImp.h
// The base DreamCloud Implementation intefrace

#include <string>
#include <functional>

class CloudImp {
public:
	CloudImp() {
		// empty
	}

	~CloudImp() {
		// empty
	}

	virtual RESULT Update() = 0;
	virtual RESULT CreateNewURLRequest(std::wstring& strURL) = 0;

	virtual RESULT ConnectToPeer(int peerID) = 0;
	virtual std::function<void(int msg_id, void* data)> GetUIThreadCallback() = 0;
	virtual RESULT SendMessageToPeer(int peerID, std::string& strMessage) = 0;

	virtual std::string GetSDPOfferString() { return std::string(""); };
	virtual RESULT InitializeConnection() = 0;

	// TODO: Add a handle URL request callback here

private:
	UID m_uid;
};

#endif