#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/viewport.h
// Viewport Primitive
// Basic viewport primitive

#define DEFAULT_VIEWPORT_FOV_ANGLE 60.0f
#define DEFAULT_VIEWPORT_WIDTH 100
#define DEFAULT_VIEWPORT_HEIGHT 100

class viewport {
public:
	viewport(int pxWidth = DEFAULT_VIEWPORT_WIDTH, 
			 int pxHeight = DEFAULT_VIEWPORT_HEIGHT, 
			 float FOVAngle = DEFAULT_VIEWPORT_FOV_ANGLE);

	int Width() const;
	int Height() const;
	float FOVAngle();

	RESULT ResizeViewport(int width, int height);
	RESULT ResizeViewport(int width, int height, float FOVAngle);

protected:
	int m_pxViewWidth;
	int m_pxViewHeight;
	float m_FOVAngle;
};

#endif // ! VIEWPORT_H_
