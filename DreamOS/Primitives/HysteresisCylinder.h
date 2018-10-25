#ifndef HYSTERESIS_CYLINDER_H_
#define HYSTERESIS_CYLINDER_H_

#include "HysteresisObj.h"

class HysteresisCylinder : public HysteresisObj {

public:
	HysteresisCylinder();
	~HysteresisCylinder();

public:
	virtual bool Resolve(UIMallet *pMallet) override;
};

#endif // ! HYSTERESIS_CYLINDER_H_
