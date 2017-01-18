#ifndef COLLISION_MANIFOLD_H_
#define COLLISION_MANIFOLD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/PhysicsEngine/CollisionManifold.h
// The collision manifold is a container for the data associated with a 
// given collision

#include "ContactPoint.h"

#define MAX_CONTACT_POINTS 4

class VirtualObj;

class CollisionManifold {
public:
	//CollisionManifold();
	CollisionManifold(VirtualObj *pObjA, VirtualObj *pObjB);
	RESULT AddContactPoint(point ptContact, vector vNormal, double penetrationDepth);
	RESULT Clear();

	int NumContacts() const;
	double MaxPenetrationDepth() const;
	vector GetNormal() const;
	point GetContactPoint() const;

	VirtualObj *GetObjectA() const;
	VirtualObj *GetObjectB() const;

private:
	ContactPoint m_contactPoints[MAX_CONTACT_POINTS];	// Use a static array for cache coherency 
	int m_numContacts = 0;
	double m_maxPenetrationDepth = 0.0f;

	VirtualObj *m_pObjA = nullptr;
	VirtualObj *m_pObjB = nullptr;
};

#endif	// !COLLISION_MANIFOLD_H_