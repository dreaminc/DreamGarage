#include "HysteresisCylinder.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisCylinder::HysteresisCylinder() {
	// empty
}

HysteresisCylinder::~HysteresisCylinder() {
	// empty
}

bool HysteresisCylinder::Resolve(VirtualObj *pObj) {
	RESULT r = R_PASS;

	point ptBoxOrigin = GetPosition(true);
	point ptSphereOrigin = pObj->GetOrigin(true);
	ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation(true))) * (ptSphereOrigin - GetOrigin(true)));

	float distance = vector(ptSphereOrigin).magnitude();
	if (m_currentState == ON) {
		return distance > m_onThreshold;
	}
	else {
		return distance < m_offThreshold;
	}

Error:
	return false;
}
