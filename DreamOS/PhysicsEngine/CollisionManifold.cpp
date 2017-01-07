#include "CollisionManifold.h"

CollisionManifold::CollisionManifold() :
	m_pDimObjA(nullptr),
	m_pDimObjB(nullptr),
	m_numContacts(0)
{
	Clear();
}

CollisionManifold::CollisionManifold(DimObj *pDimObjA, DimObj *pDimObjB) :
	m_pDimObjA(pDimObjA),
	m_pDimObjB(pDimObjB),
	m_numContacts(0)
{
	Clear();
}

RESULT CollisionManifold::AddContactPoint(point ptContact, vector vNormal, double penetrationDepth) {
	RESULT r = R_PASS;

	CB((m_numContacts < MAX_CONTACT_POINTS));
	
	m_contactPoints[m_numContacts] = {
		ptContact, vNormal, penetrationDepth
	};

	if (penetrationDepth > m_maxPenetrationDepth) {
		m_maxPenetrationDepth = penetrationDepth;
	}

	// Average Normal

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

int CollisionManifold::NumContacts() {
	return m_numContacts;
}

double CollisionManifold::MaxPenetrationDepth() {
	return m_maxPenetrationDepth;
}

vector CollisionManifold::GetNormal() {
	vector vNormal = vector();

	for (int i = 0; i < m_numContacts; i++) {
		vNormal += m_contactPoints[i].GetNormal();
	}

	vNormal.Normalize();

	return vNormal;
}