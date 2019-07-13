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

	enum class type {
		XY,
		XZ,
		YZ,
		INVALID
	};

public:
	plane();

	plane(plane::type planeType);

	plane(point ptPosition, vector vNormal);

	// Distance of point from plane
	point_precision Distance(point pt);

	vector GetNormal() const;

	RESULT SetNormal(vector vNormal);

	RESULT SetPlanePosition(point ptPosition);

	point GetPosition() const;

	point_precision GetDValue() const;

	static point_precision GetDValue(point ptOrigin, vector vNormal);

// Utilities for axes
	static plane iPlane() { return plane(point(0.0f, 0.0f, 0.0f), vector::iVector(1.0f)); }
	static plane jPlane() { return plane(point(0.0f, 0.0f, 0.0f), vector::jVector(1.0f)); }
	static plane kPlane() { return plane(point(0.0f, 0.0f, 0.0f), vector::kVector(1.0f)); }

protected:
	point m_ptPosition;
	vector m_vNormal;
};

#endif // !BOUNDING_VOLUME_H_
