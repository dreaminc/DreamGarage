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
	HMD_INVALID
} HMD_TYPE;

#include "HAL\HALImp.h"

class HMDFactory : public Factory {
public:
	static HMD* MakeHMD(HMD_TYPE type, HALImp *halimp);
};

#endif // ! PATH_MANAGER_FACTORY_H_
