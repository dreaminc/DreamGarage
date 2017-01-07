#include "CollisionManifold.h"

CollisionManifold::CollisionManifold() :
	m_pDimObjA(nullptr),
	m_pDimObjB(nullptr),
	m_numContacts(0)
{
	memset(m_contactPoints, 0, sizeof(ContactPoint) * MAX_CONTACT_POINTS);
}

CollisionManifold::CollisionManifold(DimObj *pDimObjA, DimObj *pDimObjB) :
	m_pDimObjA(pDimObjA),
	m_pDimObjB(pDimObjB),
	m_numContacts(0)
{
	memset(m_contactPoints, 0, sizeof(ContactPoint) * MAX_CONTACT_POINTS);
}

RESULT CollisionManifold::AddContactPoint(point ptContact, vector vNormal, double penetrationDepth) {
	RESULT r = R_PASS;

	CB((m_numContacts < MAX_CONTACT_POINTS));
	
	m_contactPoints[m_numContacts] = {
		ptContact, vNormal, penetrationDepth
	};

	m_numContacts += 1;

Error:
	return r;
}

RESULT CollisionManifold::Clear() {
	m_numContacts = 0;
	return R_PASS;
}

int CollisionManifold::NumContacts() {
	return m_numContacts;
}