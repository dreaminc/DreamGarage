#ifndef BOUNDING_SPHERE_H_
#define BOUNDING_SPHERE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingSphere.h
// Bounding sphere

#include "BoundingSphere.h"
#include "point.h"
#include "vector.h"
#include "line.h"

class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere() :
		m_ptOrigin(),
		m_radius(1.0f)
	{
		// Empty
	}

	BoundingSphere(point ptOrigin, float radius) :
		m_ptOrigin(ptOrigin),
		m_radius(radius)
	{
		// Empty
	}

	bool Intersect(const BoundingSphere& rhs) {
		float distance = reinterpret_cast<vector>(rhs.m_ptOrigin - m_ptOrigin).magnitude();

		if (abs(distance) <= (rhs.m_radius + m_radius))
			return true;
		else
			return false;
	}

	bool Intersect(const point& pt) {
		float distance = reinterpret_cast<vector>(pt - m_ptOrigin).magnitude();

		if (abs(distance) <= m_radius)
			return true;
		else
			return false;
	}

	bool Intersect(cont line& ln) {

		
	}

protected:
	point m_ptOrigin;
	float m_radius;
};

#endif // !BOUNDING_VOLUME_H_
