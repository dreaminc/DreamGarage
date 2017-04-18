#include "stereocamera.h"

#include "Primitives/viewport.h"

stereocamera::stereocamera(point ptOrigin, viewport cameraVieport) :
	camera(ptOrigin, cameraVieport)
{
	m_pupillaryDistance = (DEFAULT_PUPILLARY_DISTANCE / 1000.0f);
}

point stereocamera::GetEyePosition(EYE_TYPE eye) {
	point ptEye;

	switch (eye) {
	case EYE_LEFT: {
		ptEye = camera::GetOrigin() + (GetRightVector() * (-m_pupillaryDistance / 2.0f));
	} break;

	case EYE_RIGHT: {
		ptEye = camera::GetOrigin() + (GetRightVector() * (m_pupillaryDistance / 2.0f));
	} break;

	case EYE_MONO: {
		ptEye = camera::GetOrigin();
	} break;
	}

	ptEye.w() = 0.0f;

	return ptEye;
}

ProjectionMatrix stereocamera::GetProjectionMatrix(EYE_TYPE eye) {
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

point stereocamera::GetOrigin(bool fAbsolute) {
	point eyePos = GetEyePosition(EYE_MONO);

	if (m_pHMD != nullptr) {
		eyePos += m_pHMD->GetHeadPointOrigin();
	}

	return eyePos;
}

point stereocamera::GetPosition(bool fAbsolute) {
	return GetOrigin(fAbsolute);
}

ViewMatrix stereocamera::GetViewMatrix(EYE_TYPE eye) {
	ViewMatrix mat;

	point eyePos = GetEyePosition(eye);

	// TODO: Fix this
	if (m_pHMD != nullptr) {
		point offset = m_pHMD->GetHeadPointOrigin();
		eyePos += offset;
	}
	// View Matrix requires the opposite of the camera's world position
	eyePos.Reverse();
	quaternion q = camera::GetOrientation();
	//		q.Reverse();


	mat = ViewMatrix(eyePos, q);
	return mat;
}