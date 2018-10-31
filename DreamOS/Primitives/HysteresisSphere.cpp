#include "HysteresisSphere.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisSphere::HysteresisSphere(float onThreshold, float offThreshold) : 
	HysteresisObject(onThreshold,offThreshold)
{

}

HysteresisSphere::HysteresisSphere() {
	// empty
}

HysteresisSphere::~HysteresisSphere() {
	// empty
}

inline bool HysteresisSphere::Resolve(VirtualObj *pObj) {
	RESULT r = R_PASS;

	float distance = (pObj->GetPosition(true) - GetPosition(true)).magnitude();

	if (m_currentStates.count(pObj) == 0 || m_currentStates[pObj] == OFF) {
		return distance > m_onThreshold;
	}
	else {
		return distance > m_offThreshold;
	}
	
Error:
	return false;
}
