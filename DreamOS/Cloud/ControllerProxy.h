#ifndef CONTROLLER_PROXY_H_
#define CONTROLLER_PROXY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/ControllerProxy.h
// The Controller Proxy Object


enum class CLOUD_CONTROLLER_PROXY_TYPE;

class ControllerProxy {
public:
	virtual CLOUD_CONTROLLER_PROXY_TYPE GetControllerType() = 0;
};

#endif // ! CONTROLLER_PROXY_H_

