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

class CloudController;

class CloudImp {
public:
	CloudImp() :
		m_pParentCloudController(nullptr)
	{
		// empty
	}

	CloudImp(CloudController *pParentCloudController) :
		m_pParentCloudController(pParentCloudController)
	{
		// empty
	}

	~CloudImp() {
		// empty
	}

	virtual RESULT Update() = 0;
	virtual RESULT CreateNewURLRequest(std::wstring& strURL) = 0;

	virtual RESULT ConnectToPeer(int peerID) = 0;
	virtual std::function<void(int msg_id, void* data)> GetUIThreadCallback() = 0;
	
	virtual RESULT SendDataChannelStringMessage(int peerID, std::string& strMessage) = 0;
	virtual RESULT SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) = 0;

	virtual std::string GetSDPOfferString() { return std::string(""); };
	virtual RESULT CreateSDPOfferAnswer(std::string strSDPOfferJSON) { return R_NOT_IMPLEMENTED; };
	virtual RESULT InitializeConnection(bool fMaster, bool fAddDataChannel) = 0;
	virtual RESULT AddIceCandidates() { return R_NOT_IMPLEMENTED; }

	// TODO: Add a handle URL request callback here

	CloudController* GetParentCloudController() { return m_pParentCloudController; }

private:
	CloudController *m_pParentCloudController;
	UID m_uid;
};

#endif