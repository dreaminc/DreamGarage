#ifndef CONTROLLER_OBSERVER_H_
#define CONTROLLER_OBSERVER_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/types/ControllerProxy.h

// The Controller Proxy Object

enum class CLOUD_CONTROLLER_TYPE;

class ControllerObserver {
public:
	virtual CLOUD_CONTROLLER_TYPE GetControllerType() = 0;
};

#endif // ! CONTROLLER_OBSERVER_H_

