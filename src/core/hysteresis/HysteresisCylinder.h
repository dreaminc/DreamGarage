#ifndef HYSTERESIS_CYLINDER_H_
#define HYSTERESIS_CYLINDER_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/hysteresis/HysteresisCylinder.h

#include "HysteresisObject.h"

class HysteresisCylinder : public HysteresisObject {

public:
	HysteresisCylinder(float onThreshold, float offThreshold);
	HysteresisCylinder();
	~HysteresisCylinder();

public:
	virtual bool Resolve(VirtualObj *pObj) override;
};

#endif // ! HYSTERESIS_CYLINDER_H_
