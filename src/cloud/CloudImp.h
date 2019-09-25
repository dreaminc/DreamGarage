#ifndef CLOUD_IMP_H_
#define CLOUD_IMP_H_

#include "core/ehm/EHM.h"

// Dream Cloud
// dos/src/cloud/CloudImp.h

// The base DreamCloud Implementation interface

#include "core/types/UID.h"
#include "core/types/valid.h"


#include <string>
#include <functional>

#include "cloud/User/User.h"
#include "cloud/User/TwilioNTSInformation.h"

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

	virtual RESULT Update() { return R_NOT_IMPLEMENTED_WARNING; }
	virtual RESULT CreateNewURLRequest(std::wstring& strURL) { return R_NOT_IMPLEMENTED_WARNING; }

	CloudController* GetParentCloudController() { return m_pParentCloudController; }

	virtual RESULT SetUser(User currentUser) { return R_NOT_IMPLEMENTED_WARNING; };
	virtual RESULT SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation) { return R_NOT_IMPLEMENTED_WARNING; };


private:
	CloudController *m_pParentCloudController;
	UID m_uid;
};

#endif