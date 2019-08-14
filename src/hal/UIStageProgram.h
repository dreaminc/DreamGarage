#ifndef UI_STAGE_PROGRAM_H_
#define UI_STAGE_PROGRAM_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/UIStageProgram.h

// UI Stage Program is the interface for UI rendering

#include "core/types/DObject.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"

class UIStageProgram : public DObject {
public:

	virtual RESULT SetOriginPoint(point ptOrigin) = 0;
	virtual RESULT SetOriginDirection(vector vOrigin) = 0;
	virtual RESULT SetIsAugmented(bool fAugmented) = 0;
	virtual RESULT SetClippingThreshold(float clippingThreshold) = 0;
	virtual RESULT SetClippingRate(float clippingRate) = 0;

protected:
	point m_ptOrigin;
	vector m_vOrigin;
	bool m_fIsAugmented = false;
	float m_clippingThreshold;
	float m_clippingRate;
};

#endif // ! UI_STAGE_PROGRAM_H_