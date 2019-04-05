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

	// TODO: this should be handled in the ObjectState in the VirtualObj
	//vector velocity = pObj->GetState().GetVelocity();
	//vector velocity = pObj->GetState().GetInstantVelocity();
	//velocity = vector(velocity.x(), 0.0f, velocity.z());

	// test
	//auto pDimObj = dynamic_cast<DimObj*>(pObj);
	//auto pHand = pDimObj->GetParent();

	vector vOffset = vector(ptSphereOrigin.x(), 0.0f, ptSphereOrigin.z());

	if (m_offsets.count(pObj) == 0) {
		m_offsets[pObj] = vOffset;
	}
	vector velocity = vOffset - m_offsets[pObj];
	m_offsets[pObj] = vOffset;

	// calculate the object's velocity projected onto the direction of its current offset
	vector vOffsetUnit = vOffset / vOffset.magnitude();
	float velocityProj = velocity.dot(vOffsetUnit);
	float advanceFrames = 40;

	float distance = vector(vOffset * (1 + velocityProj * advanceFrames)).magnitude();
	//float distance = vOffset.magnitude();

	if (m_currentStates.count(pObj) == 0 || m_currentStates[pObj] == OFF) {
		return distance > m_onThreshold;
	}
	else {
		return distance > m_offThreshold;
	}

Error:
	return false;
}
