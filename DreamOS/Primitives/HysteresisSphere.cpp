#include "HysteresisSphere.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisSphere::HysteresisSphere() {
	// empty
}

HysteresisSphere::~HysteresisSphere() {
	// empty
}

bool HysteresisSphere::Resolve(VirtualObj *pObj) {
	RESULT r = R_PASS;

	float distance = (pObj->GetPosition(true) - GetPosition(true)).magnitude();

	if (m_currentState == ON) {
		return distance > m_onThreshold;
	}
	else {
		return distance < m_offThreshold;
	}
	
Error:
	return false;
}
