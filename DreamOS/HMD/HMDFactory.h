#ifndef HMD_FACTORY_H_
#define HMD_FACTORY_H_

#include "RESULT/EHM.h"
#include "HMD.h"
#include "Core/Types/Factory.h"

// DREAM OS
// DreamOS/HMD/HMDFactory.h
// Dream OS HMD Factory for the construction of HMD objects

typedef enum {
	HMD_OVR,
	HMD_OPENVR,
	HMD_ANY_AVAILABLE,
	HMD_INVALID
} HMD_TYPE;

#include "HAL\HALImp.h"
#include "Sandbox/SandboxApp.h"

class HMDFactory : public Factory {
public:
	//static HMD* MakeHMD(HMD_TYPE type, HALImp *halimp);
	static HMD* MakeHMD(HMD_TYPE type, SandboxApp *pParentSandbox, HALImp *halimp, int wndWidth = 0, int wndHeight = 0, bool fHMDMirror = true);
};

#endif // ! PATH_MANAGER_FACTORY_H_
