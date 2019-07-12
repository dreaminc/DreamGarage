#ifndef CONTROLLER_OBSERVER_H_
#define CONTROLLER_OBSERVER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/ControllerProxy.h
// The Controller Proxy Object


enum class CLOUD_CONTROLLER_TYPE;

class ControllerObserver {
public:
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() = 0;
};

#endif // ! CONTROLLER_OBSERVER_H_

