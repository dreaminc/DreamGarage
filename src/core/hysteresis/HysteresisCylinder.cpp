#include "HysteresisCylinder.h"
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

	vector vOffset = vector(ptSphereOrigin.x(), 0.0f, ptSphereOrigin.z());

	vector velocity = pObj->GetState().GetInstantVelocity();
	velocity = (point)(inverse(RotationMatrix(GetOrientation(true))) * (velocity));

	// calculate the object's velocity projected onto the direction of its current offset
	vector vOffsetUnit = vOffset / vOffset.magnitude();
	float velocityProj = velocity.dot(vOffsetUnit);
	float advanceFrames = 40;

	float distance = vector(vOffset * (1 + velocityProj * advanceFrames)).magnitude();

	if (m_currentStates.count(pObj) == 0 || m_currentStates[pObj] == OFF) {
		return distance > m_onThreshold;
	}
	else {
		return distance > m_offThreshold;
	}

Error:
	return false;
}
