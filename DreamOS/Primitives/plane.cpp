#include "plane.h"

#include "point.h"
#include "vector.h"

plane::plane() :
	m_ptPosition(),
	m_vNormal(vector::jVector(1.0f))
{
	// Empty
}

plane::plane(plane::type planeType) :
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

plane::plane(point ptPosition, vector vNormal) :
	m_ptPosition(ptPosition),
	m_vNormal(vNormal)
{
	// Empty
}

// Distance of point from plane
point_precision plane::Distance(point pt) {
	return m_vNormal.Normal().dot(m_ptPosition - pt);
}

vector plane::GetNormal() const {
	return m_vNormal;
}

RESULT plane::SetNormal(vector vNormal) {
	m_vNormal = vNormal.Normal();
	return R_PASS;
}

RESULT plane::SetPlanePosition(point ptPosition) {
	m_ptPosition = ptPosition;
	return R_PASS;
}

point plane::GetPosition() const {
	return m_ptPosition;
}

point_precision plane::GetDValue() const {
	return GetDValue(m_ptPosition, m_vNormal);
}

point_precision plane::GetDValue(point ptOrigin, vector vNormal) {
	point_precision dVal = -1.0f * (vNormal.x() * ptOrigin.x() +
		vNormal.y() * ptOrigin.y() +
		vNormal.z() * ptOrigin.z());

	//point_precision dVal = -1.0f * vector(ptOrigin).dot(vNormal);

	return dVal;
}