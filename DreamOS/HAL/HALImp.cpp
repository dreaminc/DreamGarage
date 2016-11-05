#include "HALImp.h"

HALImp::HALImp() :
	m_pCamera(nullptr),
	m_pHMD(nullptr)
{
	memset(&m_HALConfiguration, 0, sizeof(HALImp::HALConfiguration));
	
	m_HALConfiguration.fRenderReferenceGeometry = true;
}

HALImp::~HALImp() {
	// empty stub
}

RESULT HALImp::SetRenderReferenceGeometry(bool fRenderReferenceGeometry) {
	m_HALConfiguration.fRenderReferenceGeometry = fRenderReferenceGeometry;
	return R_PASS;
}

bool HALImp::IsRenderReferenceGeometry() {
	return (bool)(m_HALConfiguration.fRenderReferenceGeometry);
}

camera *HALImp::GetCamera() {
	return m_pCamera;
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

//Error:
	return r;
}