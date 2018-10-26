#ifndef HYSTERESIS_SPHERE_H_
#define HYSTERESIS_SPHERE_H_

#include "HysteresisObj.h"

class HysteresisSphere : public HysteresisObj {

public:
	HysteresisSphere();
	~HysteresisSphere();

public:
	virtual bool Resolve(VirtualObj *pObj) override;
};

#endif // ! HYSTERESIS_SPHERE_H_
