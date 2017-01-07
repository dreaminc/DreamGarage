#ifndef COLLISION_MANIFOLD_H_
#define COLLISION_MANIFOLD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/PhysicsEngine/CollisionManifold.h
// The collision manifold is a container for the data associated with a 
// given collision

#include "ContactPoint.h"

#define MAX_CONTACT_POINTS 4

class DimObj;

class CollisionManifold {
public:
	CollisionManifold();
	CollisionManifold(DimObj *pDimObjA, DimObj *pDimObjB);
	RESULT AddContactPoint(point ptContact, vector vNormal, double penetrationDepth);
	RESULT Clear();

	int NumContacts();

private:
	ContactPoint m_contactPoints[MAX_CONTACT_POINTS];	// Use a static array for cache coherency 
	int m_numContacts = 0;

	DimObj *m_pDimObjA = nullptr;
	DimObj *m_pDimObjB = nullptr;
};

#endif	// !COLLISION_MANIFOLD_H_