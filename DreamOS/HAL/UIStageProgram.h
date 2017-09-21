#ifndef UI_STAGE_PROGRAM_H_
#define UI_STAGE_PROGRAM_H_

// Dream OS
// DreamOS/HAL/UIStageProgram.h
// UI Stage Program is the interface for UI rendering

#include "./RESULT/EHM.h"

class ViewMatrix;
class ProjectionMatrix;

class UIStageProgram {
public:
	
	// Set Matrix for clipping window
	virtual RESULT SetClippingViewMatrix(ViewMatrix matView) = 0;
	virtual RESULT SetClippingFrustrum(float left, float right, float top, float bottom, float nearPlane, float farPlane) = 0;
	virtual RESULT SetClippingFrustrum(float width, float height, float nearPlane, float farPlane, float angle) = 0;

protected:
	ViewMatrix m_clippingView;
	ProjectionMatrix m_clippingProjection;
};

#endif // ! UI_STAGE_PROGRAM_H_