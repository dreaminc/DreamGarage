#ifndef HYSTERESIS_QUAD_H_
#define HYSTERESIS_QUAD_H_

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
