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

	// Temp
	// TODO: Fix this
	ProjectionMatrix GetProjectionMatrix(EYE_TYPE eye) {
		
		projection_precision left = -m_pxScreenWidth / 2;
		projection_precision right = m_pxScreenWidth / 2;
		projection_precision top = m_pxScreenHeight / 2;
		projection_precision bottom = -m_pxScreenHeight / 2;

		ProjectionMatrix projMat = ProjectionMatrix(left, right, top, bottom, m_NearPlane, m_FarPlane);

		ProjectionMatrix projMat1 = camera::GetProjectionMatrix();
		ProjectionMatrix projMat2 = m_pHMD->GetPerspectiveFOVMatrix(eye, m_NearPlane, m_FarPlane);

		///*
		projMat1.element(0, 0) = projMat2.element(0, 0);
		//projMat.element(2, 0) = projMat2.element(2, 0);

		projMat1.element(1, 1) = projMat2.element(1, 1);
		//projMat.element(2, 1) = projMat2.element(2, 1);

		///*
		//projMat2.element(2, 2) = projMat1.element(2, 2);
		//projMat2.element(2, 3) = projMat1.element(2, 3);
		projMat2.element(3, 2) = projMat1.element(3, 2);
		//*/

		//projMat2(3, 2) = -0.2f;

		// TODO: Flip matrix stuff (everything is transposed it seems)
		projMat2.element(0, 2) = projMat2.element(2, 0);

		
		/*
		// offset?
		float val = -0.15;
		if (eye == EYE_LEFT)
			projMat2.element(0, 2) = val;
		else
			projMat2.element(0, 2) = -val;
			*/

		//return projMat1;*/
		return projMat2;
	}

	ViewMatrix GetViewMatrix(EYE_TYPE eye) {
		ViewMatrix mat;

		point eyePos = GetEyePosition(eye);

		if (m_pHMD != nullptr)
			eyePos += m_pHMD->GetHeadPointOrigin();

		switch (eye) {
			case EYE_LEFT: {
				mat = ViewMatrix(eyePos, m_qRotation);
			} break;

			case EYE_RIGHT: {
				mat = ViewMatrix(eyePos, m_qRotation);
			} break;

			case EYE_MONO: {
				mat = ViewMatrix(m_ptOrigin, m_qRotation);
			} break;
		}
			
		return mat;
	}

	RESULT SetHMD(HMD *pHMD) {
		m_pHMD = pHMD;
		return R_PASS;
	}

private:
	camera_precision m_pupillaryDistance;	//  Distance between eyes (in mm)
	HMD *m_pHMD;
};

#endif // ! STEREO_CAMERA_H_