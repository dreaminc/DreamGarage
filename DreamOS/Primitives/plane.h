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
	plane() :
		m_ptPosition(),
		m_vNormal(vector::jVector(1.0f))
	{
		// Empty
	}

	plane(plane::type planeType) :
		m_ptPosition()
	{
		switch (planeType) {
			case plane::type::XY: {
				m_vNormal = vector::kVector(1.0f);
			} break;

			case plane::type::XZ: {
				m_vNormal = vector::jVector(1.0f);
			} break;

			case plane::type::YZ: {
				m_vNormal = vector::iVector(1.0f);
			} break;

			case plane::type::INVALID: {
				// empty
			}
		}
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

	vector GetNormal() const {
		return m_vNormal;
	}

	RESULT SetNormal(vector vNormal) {
		m_vNormal = vNormal.Normal();
		return R_PASS;
	}

	point GetPosition() const {
		return m_ptPosition;
	}

	point_precision GetDValue() const {
		return GetDValue(m_ptPosition, m_vNormal);
	}

	static point_precision GetDValue(point ptOrigin, vector vNormal) {
		point_precision dVal = -1.0f * (vNormal.x() * ptOrigin.x() +
										vNormal.y() * ptOrigin.y() +
										vNormal.z() * ptOrigin.z());

		return dVal;
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
