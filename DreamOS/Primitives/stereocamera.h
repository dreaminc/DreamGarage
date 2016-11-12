#ifndef STEREO_CAMERA_H_
#define STEREO_CAMERA_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/stereocamera.h
// Stereo Camera Primitive
// The stereo camera is a dual camera with it's own orientation and position

#include "camera.h"

#define DEFAULT_PUPILLARY_DISTANCE 55

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
				ptEye = m_ptOrigin + (GetRightVector() * (-m_pupillaryDistance / 2.0f));
			} break;

			case EYE_RIGHT: {
				ptEye = m_ptOrigin + (GetRightVector() * (m_pupillaryDistance / 2.0f));
			} break;

			case EYE_MONO: {
				ptEye = m_ptOrigin;
			} break;
		}

		ptEye.w() = 0.0f;

		return ptEye;
	}

	ProjectionMatrix GetProjectionMatrix(EYE_TYPE eye) {
		ProjectionMatrix projMat;

		if (m_pHMD != nullptr) {
			projMat = m_pHMD->GetPerspectiveFOVMatrix(eye, m_NearPlane, m_FarPlane);
			//projMat.element(0, 2) = projMat.element(2, 0);
			//projMat.element(1, 2) = projMat.element(2, 1);
		}
		else {
			projMat = camera::GetProjectionMatrix();
		}

		return projMat;
	}

	virtual point GetOrigin() override {
		point eyePos = GetEyePosition(EYE_MONO);

		if (m_pHMD != nullptr) {
			eyePos += m_pHMD->GetHeadPointOrigin();
		}

		return eyePos;
	}

	virtual point GetPosition() override {
		point eyePos = GetEyePosition(EYE_MONO);

		if (m_pHMD != nullptr) {
			eyePos += m_pHMD->GetHeadPointOrigin();
		}

		return eyePos;
	}

	ViewMatrix GetViewMatrix(EYE_TYPE eye) {
		ViewMatrix mat;

		point eyePos = GetEyePosition(eye);

		// TODO: Fix this
		if (m_pHMD != nullptr) {
			point offset = m_pHMD->GetHeadPointOrigin();
			eyePos += offset;
		}
		// View Matrix requires the opposite of the camera's world position
		eyePos.Reverse();
		quaternion q = m_qRotation;
//		q.Reverse();


		mat = ViewMatrix(eyePos, q);
		return mat;
	}

private:
	camera_precision m_pupillaryDistance;	//  Distance between eyes (in mm)
};

#endif // ! STEREO_CAMERA_H_