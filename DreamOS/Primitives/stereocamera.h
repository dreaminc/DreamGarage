#ifndef STEREO_CAMERA_H_
#define STEREO_CAMERA_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/stereocamera.h
// Stereo Camera Primitive
// The stereo camera is a dual camera with it's own orientation and position

#include "camera.h"

#define DEFAULT_PUPILLARY_DISTANCE 65

enum EYE_TYPE {
	EYE_LEFT,
	EYE_RIGHT,
	EYE_MONO,
	EYE_INVALID
};

class stereocamera : public camera {
public:
	stereocamera(point ptOrigin, camera_precision FOV, int pxScreenWidth, int pxScreenHeight) :
		camera(ptOrigin, FOV, pxScreenWidth, pxScreenHeight)
	{
		m_pupillaryDistance = (DEFAULT_PUPILLARY_DISTANCE / 1000.0f);
	}

	point GetEyePosition(EYE_TYPE eye) {
		point ptEye;
		
		switch (eye) {
			case EYE_LEFT: {
				ptEye = m_ptOrigin + (GetRightVector() * (m_pupillaryDistance / 2.0f));
			} break;

			case EYE_RIGHT: {
				ptEye = m_ptOrigin + (GetRightVector() * (-m_pupillaryDistance / 2.0f));
			} break;

			case EYE_MONO: {
				ptEye = m_ptOrigin;
			} break;
		}

		ptEye.w() = 0.0f;

		return ptEye;
	}

	ViewMatrix GetViewMatrix(EYE_TYPE eye) {
		ViewMatrix mat;

		switch (eye) {
			case EYE_LEFT: {
				point leftEyePoint = m_ptOrigin + (GetRightVector() * (m_pupillaryDistance / 2.0f));
				mat = ViewMatrix(leftEyePoint, m_qRotation);
			} break;

			case EYE_RIGHT: {
				point rightEyePoint = m_ptOrigin + (GetRightVector() * (-m_pupillaryDistance / 2.0f));
				mat = ViewMatrix(rightEyePoint, m_qRotation);
			} break;

			case EYE_MONO: {
				mat = ViewMatrix(m_ptOrigin, m_qRotation);
			} break;
		}
		
		return mat;
	}

private:
	camera_precision m_pupillaryDistance;	//  Distance between eyes (in mm)
};

#endif // ! STEREO_CAMERA_H_