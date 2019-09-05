#ifndef HYSTERESIS_QUAD_H_
#define HYSTERESIS_QUAD_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/hysteresis/HysteresisPlane.h

#include "HysteresisObject.h"

class HysteresisPlane : public HysteresisObject {

public:
	HysteresisPlane(float onThreshold, float offThreshold);
	HysteresisPlane();
	~HysteresisPlane();

public:
	virtual bool Resolve(VirtualObj *pObj) override;
};

#endif // ! HYSTERESIS_QUAD_H_
