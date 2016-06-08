#ifndef CLOUD_CONTROLLER_H_
#define CLOUD_CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/CloudController.h
// The base DreamCloud controller 

class CloudController {
public:
	CloudController() {
		// empty
	}

	~CloudController() {
		// empty
	}

private:
	UID m_uid;
};

#endif