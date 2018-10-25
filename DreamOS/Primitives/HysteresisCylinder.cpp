#include "HysteresisCylinder.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisCylinder::HysteresisCylinder() {
	// empty
}

HysteresisCylinder::~HysteresisCylinder() {
	// empty
}

bool HysteresisCylinder::Resolve(UIMallet *pMallet) {
	RESULT r = R_PASS;

	point ptBoxOrigin = GetPosition(true);
	point ptSphereOrigin = pMallet->GetMalletHead()->GetOrigin(true);
	ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation(true))) * (ptSphereOrigin - GetOrigin(true)));

	float distance = vector(ptSphereOrigin).magnitude();
	if (m_currentState == ON) {
		return distance + pMallet->GetRadius() > m_onDistance;
	}
	else {
		return distance - pMallet->GetRadius() < m_offDistance;
	}

Error:
	return false;
}
