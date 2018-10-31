#include "HysteresisCylinder.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisCylinder::HysteresisCylinder(float onThreshold, float offThreshold) : 
	HysteresisObject(onThreshold,offThreshold)
{

}

HysteresisCylinder::HysteresisCylinder() {
	// empty
}

HysteresisCylinder::~HysteresisCylinder() {
	// empty
}

inline bool HysteresisCylinder::Resolve(VirtualObj *pObj) {
	RESULT r = R_PASS;

	point ptBoxOrigin = GetPosition(true);
	point ptSphereOrigin = pObj->GetOrigin(true);
	ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation(true))) * (ptSphereOrigin - GetOrigin(true)));

	float distance = vector(ptSphereOrigin.x(), 0.0f, ptSphereOrigin.z()).magnitude();
	if (m_currentStates.count(pObj) == 0 || m_currentStates[pObj] == OFF) {
		return distance > m_onThreshold;
	}
	else {
		return distance > m_offThreshold;
	}

Error:
	return false;
}
