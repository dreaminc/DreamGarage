#ifndef CLOUD_CONTROLLER_FACTORY_H_
#define CLOUD_CONTROLLER_FACTORY_H_

#include "RESULT/EHM.h"
#include "CloudController.h"
#include "Core/Types/Factory.h"

// DREAM OS
// DreamOS/Sandbox/PathManagerFactory.h
// Dream OS Path Manager Factory for the construction of path managers within a given
// sandbox or native implementation

typedef enum {
	CLOUD_CONTROLLER_CEF,
	CLOUD_CONTROLLER_WEBRTC,
	CLOUD_CONTROLLER_INVALID
} CLOUD_CONTROLLER_TYPE;

class CloudControllerFactory : public Factory {
public:
	static CloudController* MakeCloudController(CLOUD_CONTROLLER_TYPE type, void *pContext);
};

#endif // ! CLOUD_CONTROLLER_FACTORY_H_
