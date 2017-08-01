#ifndef PLANE_H_
#define PLANE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/plane.h
// Plane class which is represented as a position and direction

#include "point.h"
#include "vector.h"

class plane {
public:
	plane() :
		m_ptPosition(),
		m_vNormal(vector::jVector(1.0f))
	{
		// Empty
	}

	plane(point ptPosition, vector vNormal) :
		m_ptPosition(ptPosition),
		m_vNormal(vNormal)
	{
		// Empty
	}

	// Distance of point from plane
	point_precision Distance(point pt) {
		return m_vNormal.Normal().dot(m_ptPosition - pt);
	}

	vector GetNormal() {
		return m_vNormal;
	}

	point GetPosition() {
		return m_ptPosition;
	}

// Utilities for axes
	static plane iPlane() { return plane(point(0.0f, 0.0f, 0.0f), vector::iVector(1.0f)); }
	static plane jPlane() { return plane(point(0.0f, 0.0f, 0.0f), vector::jVector(1.0f)); }
	static plane kPlane() { return plane(point(0.0f, 0.0f, 0.0f), vector::kVector(1.0f)); }

protected:
	point m_ptPosition;
	vector m_vNormal;
};

#endif // !BOUNDING_VOLUME_H_
