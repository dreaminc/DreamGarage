#ifndef CONTROLLER_PROXY_H_
#define CONTROLLER_PROXY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/ControllerProxy.h
// The Controller Proxy Object


enum class CLOUD_CONTROLLER_TYPE;
class ControllerObserver;

class ControllerProxy {
public:
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() = 0;
	virtual RESULT RegisterControllerObserver(ControllerObserver* pControllerObserver) = 0;
};

#endif // ! CONTROLLER_PROXY_H_

