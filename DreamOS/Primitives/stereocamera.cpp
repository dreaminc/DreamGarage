#include "stereocamera.h"

#include "Primitives/viewport.h"

stereocamera::stereocamera(point ptOrigin, viewport cameraVieport) :
	camera(ptOrigin, cameraVieport),
	m_eye(EYE_MONO)
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

	default:
	case EYE_MONO: {
		ptEye = camera::GetOrigin();
	} break;
	}

	ptEye.w() = 0.0f;

	return ptEye;
}

ProjectionMatrix stereocamera::GetProjectionMatrix(EYE_TYPE eye) {
	//ProjectionMatrix projMat;

	if (m_pHMD != nullptr) {
		if (m_fProjEyeInit[eye] == false) {
			m_projEye[eye] = m_pHMD->GetPerspectiveFOVMatrix(eye, m_NearPlane, m_FarPlane);
			m_fProjEyeInit[eye] = true;
		}

		return m_projEye[eye];
	}
	else {
		return camera::GetProjectionMatrix();
	}

	//return projMat;
}

point stereocamera::GetOrigin(bool fAbsolute) {
	point eyePos = GetEyePosition(EYE_MONO);

	if (m_pHMD != nullptr) {
		eyePos += m_pHMD->GetHeadPointOrigin();
	}

	return eyePos;
}

RESULT stereocamera::SetHMDAdjustedPosition(point ptPosition) {
	RESULT r = R_PASS;

	if (m_pHMD != nullptr) {
		point ptHMDOffset = m_pHMD->GetHeadPointOrigin();
		point ptAdjustedPosition = ptPosition - ptHMDOffset;

		SetPosition(ptAdjustedPosition);
	}
	else {
		SetPosition(ptPosition);
	}

//Error:
	return r;
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

	mat = ViewMatrix(eyePos, q);
	return mat;
}

EYE_TYPE stereocamera::GetCameraEye() {
	return m_eye;
}

RESULT stereocamera::SetCameraEye(EYE_TYPE eye) {
	m_eye = eye;
	return R_PASS;
}