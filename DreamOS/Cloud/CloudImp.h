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

	virtual RESULT Update() { return R_NOT_IMPLEMENTED; }
	virtual RESULT CreateNewURLRequest(std::wstring& strURL) { return R_NOT_IMPLEMENTED; }

	CloudController* GetParentCloudController() { return m_pParentCloudController; }

private:
	CloudController *m_pParentCloudController;
	UID m_uid;
};

#endif