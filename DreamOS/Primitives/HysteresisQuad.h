#ifndef HYSTERESIS_QUAD_H_
#define HYSTERESIS_QUAD_H_

#include "HysteresisObj.h"

class HysteresisQuad : public HysteresisObj {

public:
	HysteresisQuad();
	~HysteresisQuad();

public:
	virtual RESULT Update(UIMallet *pMallet) override;
};

#endif // ! HYSTERESIS_QUAD_H_
