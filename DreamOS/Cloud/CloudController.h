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

	RESULT SetCloudImp(std::unique_ptr<CloudImp> m_pCloudImp) {
		RESULT r = R_PASS;

		m_pCloudImp = std::unique_ptr<CloudImp>(std::move(m_pCloudImp));
		CN(m_pCloudImp);

	Error:
		return r;
	}

private:
	UID m_uid;
	std::unique_ptr<CloudImp> m_pCloudImp;
};

#endif