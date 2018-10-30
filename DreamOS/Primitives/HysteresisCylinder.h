#ifndef HYSTERESIS_CYLINDER_H_
#define HYSTERESIS_CYLINDER_H_

#include "HysteresisObj.h"

class HysteresisCylinder : public HysteresisObj {

public:
	HysteresisCylinder(float onThreshold, float offThreshold);
	HysteresisCylinder();
	~HysteresisCylinder();

public:
	virtual bool Resolve(VirtualObj *pObj) override;
};

#endif // ! HYSTERESIS_CYLINDER_H_
