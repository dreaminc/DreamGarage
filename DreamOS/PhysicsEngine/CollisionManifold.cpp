#include "CollisionManifold.h"

/*
CollisionManifold::CollisionManifold() :
	m_pObjA(nullptr),
	m_pObjB(nullptr),
	m_numContacts(0)
{
	Clear();
}
*/

CollisionManifold::CollisionManifold(VirtualObj *pObjA, VirtualObj *pObjB) :
	m_pObjA(pObjA),
	m_pObjB(pObjB),
	m_numContacts(0)
{
	Clear();
}

RESULT CollisionManifold::AddContactPoint(point ptContact, vector vNormal, double penetrationDepth, int weight) {
	RESULT r = R_PASS;

	CB((m_numContacts < MAX_CONTACT_POINTS));

	// Ensure this contact is not near anything
	for (int i = 0; i < m_numContacts; i++) {
		if ((ptContact - m_contactPoints[i].GetPoint()).magnitude() < DREAM_EPSILON ) {
			return R_SKIPPED;
		}
	}
	
	m_contactPoints[m_numContacts] = {
		ptContact, vNormal, penetrationDepth, weight
	};

	if (std::abs(penetrationDepth) > m_maxPenetrationDepth) {
		m_maxPenetrationDepth = std::abs(penetrationDepth);
	}

	if (std::abs(penetrationDepth) < m_minPenetrationDepth) {
		m_minPenetrationDepth = std::abs(penetrationDepth);
	}

	// Average Normal happens in GetNormal

	m_numContacts += 1;

Error:
	return r;
}

RESULT CollisionManifold::Clear() {
	m_numContacts = 0;
	m_maxPenetrationDepth = 0.0f;
	memset(m_contactPoints, 0, sizeof(ContactPoint) * MAX_CONTACT_POINTS);
	return R_PASS;
}

int CollisionManifold::NumContacts() const {
	return m_numContacts;
}

double CollisionManifold::MaxPenetrationDepth() const {
	return m_maxPenetrationDepth;
}

double CollisionManifold::MinPenetrationDepth() const {
	return m_minPenetrationDepth;
}

//vector vAngularVelocityOfPointA = pObjA->GetVelocityOfPoint(manifold.GetContactPoint());

point CollisionManifold::GetContactPoint() const {
	if (m_numContacts == 1) {
		return m_contactPoints[0].GetPoint();
	}
	else if (m_numContacts > 1) {
		point ptContact = point();
		int weight = 0;

		for (int i = 0; i < m_numContacts; i++) {
			//ptContact += (point)((m_contactPoints[i].GetPoint()) * ((point_precision)(m_contactPoints[i].GetWeight()) / ((point_precision)(m_numContacts))));
			ptContact += (point)((m_contactPoints[i].GetPoint()));
		}

		ptContact = ptContact * (1.0f / (point_precision)(m_numContacts));
		//ptContact = ptContact * (1.0f / (point_precision)(weight));

		return ptContact;
	}

	return point();
}

vector CollisionManifold::GetNormal() const {
	vector vNormal = vector();

	for (int i = 0; i < m_numContacts; i++) {
		vNormal += m_contactPoints[i].GetNormal();
		//vNormal += (point)((m_contactPoints[i].GetNormal()) * ((point_precision)(m_contactPoints[i].GetWeight()) / ((point_precision)(m_numContacts))));
	}

	vNormal.Normalize();

	if (!vNormal.IsValid()) {
		int a = 5; 
	}

	return vNormal;
}

VirtualObj* CollisionManifold::GetObjectA() const {
	return m_pObjA;
}

VirtualObj* CollisionManifold::GetObjectB() const {
	return m_pObjB;
}