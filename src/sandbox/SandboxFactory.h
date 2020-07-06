#ifndef SANDBOX_FACTORY_H_
#define SANDBOX_FACTORY_H_

// DREAM OS
// DreamOS/Sandbox/SandboxFactory
// This allows creation of Sandbox without need of direct implementation

#include "Sandbox.h"

typedef enum {
	SANDBOX_APP_WIN32,
	SANDBOX_APP_OSX,
	SANDBOX_APP_LINUX,
	SANDBOX_APP_ANDROID,
	SANDBOX_APP_DREAMOS,
	SANDBOX_APP_INVALID
} SANDBOX_APP_TYPE;

class DreamOS;

class SandboxFactory {
public:
	static Sandbox* MakeSandbox(SANDBOX_APP_TYPE type, DreamOS *pDOSHandle);
};

#endif // ! SANDBOX_FACTORY_H_