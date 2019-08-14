#ifndef HMD_FACTORY_H_
#define HMD_FACTORY_H_

#include "core/ehm/EHM.h"

// Dream HMD
// dos/src/hmd/HMDFactory.h

// Dream OS HMD Factory for the construction of HMD objects

#include "HMD.h"

#include "core/types/Factory.h"

#include "hal/HALImp.h"

typedef enum {
	HMD_OVR,
	HMD_OPENVR,
	HMD_ANY_AVAILABLE,
	HMD_INVALID
} HMD_TYPE;

class Sandbox;

class HMDFactory : public Factory {
public:
	//static HMD* MakeHMD(HMD_TYPE type, HALImp *halimp);
	static HMD* MakeHMD(HMD_TYPE type, Sandbox *pParentSandbox, HALImp *halimp, int wndWidth = 0, int wndHeight = 0, bool fHMDMirror = true);
};

#endif // ! PATH_MANAGER_FACTORY_H_
