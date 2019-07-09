#ifndef UI_STAGE_PROGRAM_H_
#define UI_STAGE_PROGRAM_H_

// Dream OS
// DreamOS/HAL/UIStageProgram.h
// UI Stage Program is the interface for UI rendering

#include "./RESULT/EHM.h"

class UIStageProgram {
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