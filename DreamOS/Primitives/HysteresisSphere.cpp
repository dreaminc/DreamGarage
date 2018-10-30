#include "HysteresisSphere.h"
#include "UI/UIMallet.h"
#include "sphere.h"

HysteresisSphere::HysteresisSphere(float onThreshold, float offThreshold) : 
	HysteresisObj(onThreshold,offThreshold)
{

}

HysteresisSphere::HysteresisSphere() {
	// empty
}

HysteresisSphere::~HysteresisSphere() {
	// empty
}

bool HysteresisSphere::Resolve(VirtualObj *pObj) {
	RESULT r = R_PASS;

	float distance = (pObj->GetPosition(true) - GetPosition(true)).magnitude();

	if (m_currentState == OFF) {
		return distance > m_onThreshold;
	}
	else {
		return distance > m_offThreshold;
	}
	
Error:
	return false;
}
