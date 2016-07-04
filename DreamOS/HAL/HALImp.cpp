#include "HALImp.h"

HALImp::HALImp() :
	m_pCamera(nullptr),
	m_pHMD(nullptr)
{
	// empty stub
}

HALImp::~HALImp() {
	// empty stub
}

camera *HALImp::GetCamera() {
	return m_pCamera;
}

RESULT HALImp::UpdateCamera() {
	RESULT r = R_PASS;

	m_pCamera->UpdateCameraPosition();

	return r;
}

RESULT HALImp::SetCameraOrientation(quaternion qOrientation) {
	m_pCamera->SetOrientation(qOrientation);
	return R_PASS;
}

RESULT HALImp::SetCameraPositionDeviation(vector vDeviation) {
	m_pCamera->SetCameraPositionDeviation(vDeviation);
	return R_PASS;
}

RESULT HALImp::SetHMD(HMD *pHMD) {
	RESULT r = R_PASS;

	m_pHMD = pHMD;
	m_pCamera->SetHMD(pHMD);

Error:
	return r;
}