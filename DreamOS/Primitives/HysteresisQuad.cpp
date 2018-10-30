#include "HysteresisQuad.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisQuad::HysteresisQuad(float onThreshold, float offThreshold) : 
	HysteresisObj(onThreshold,offThreshold)
{

}

HysteresisQuad::HysteresisQuad() {
	// empty
}

HysteresisQuad::~HysteresisQuad() {
	// empty
}

bool HysteresisQuad::Resolve(VirtualObj *pObj) {
	RESULT r = R_PASS;

	point ptBoxOrigin = GetPosition(true);
	point ptSphereOrigin = pObj->GetOrigin(true);
	ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation(true))) * (ptSphereOrigin - GetOrigin(true)));

	float distance = ptSphereOrigin.y();

	if (m_currentStates.count(pObj) == 0 || m_currentStates[pObj] == OFF) {
		return distance > m_onThreshold;
	}
	else {
		return distance > m_offThreshold;
	}

Error:
	return false;
}
