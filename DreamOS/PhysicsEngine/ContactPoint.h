#ifndef CONTACT_POINT_H_
#define CONTACT_POINT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/PhysicsEngine/ContactPoint.h
// A contact point represents a point of contact in a given collision

// Generally a collision will actually have numerous contact points given 
// objects will interpenetrate one another - the contact point is chosen 
// such that it will not affect the simulation.

// In the future, to further improve the engine we might want to explore different
// contact topologies (like lines, ellipses etc)

#include "Primitives/point.h"
#include "Primitives/vector.h"

class ContactPoint {
public:
	ContactPoint();
	ContactPoint(point ptContact, vector vNormal, double penetrationDepth);

	vector GetNormal() const;
	point GetPoint() const;

private:
	point m_ptContact;
	vector m_vNormal;
	double m_penetrationDepth;

};

#endif // !CONTACT_POINT_H_
