#include "HysteresisSphere.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisSphere::HysteresisSphere() {
	// empty
}

HysteresisSphere::~HysteresisSphere() {
	// empty
}

bool HysteresisSphere::Resolve(UIMallet *pMallet) {
	RESULT r = R_PASS;

	float distance = (pMallet->GetMalletHead()->GetPosition(true) - GetPosition(true)).magnitude();

	if (m_currentState == ON) {
		return distance + pMallet->GetRadius() > m_onDistance;
	}
	else {
		return distance - pMallet->GetRadius() < m_offDistance;
	}
	
Error:
	return false;
}
