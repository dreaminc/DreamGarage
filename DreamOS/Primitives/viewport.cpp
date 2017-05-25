#include "viewport.h"

viewport::viewport(int pxWidth, int pxHeight, float FOVAngle) :
	m_pxViewWidth(pxWidth),
	m_pxViewHeight(pxHeight),
	m_FOVAngle(FOVAngle)
{
	// empty
}

int viewport::Width() const {
	return m_pxViewWidth;
}

int viewport::Height() const {
	return m_pxViewHeight;
}

float viewport::FOVAngle() {
	return m_FOVAngle;
}

RESULT viewport::ResizeViewport(int width, int height) {
	m_pxViewWidth = width;
	m_pxViewHeight = height;

	return R_PASS;
}

RESULT viewport::ResizeViewport(int width, int height, float FOVAngle) {
	m_pxViewWidth = width;
	m_pxViewHeight = height;
	m_FOVAngle = FOVAngle;

	return R_PASS;
}