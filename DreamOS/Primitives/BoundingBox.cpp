#include "BoundingBox.h"
#include "BoundingSphere.h"

BoundingBox::BoundingBox(BoundingBox::Type type) :
	BoundingVolume(),
	m_type(type),
	m_vHalfSize(vector(1.0f, 1.0f, 1.0f))
{
	// Empty
}

BoundingBox::BoundingBox(BoundingBox::Type type, point ptOrigin, vector vHalfSize) :
	BoundingVolume(ptOrigin),
	m_type(type),
	m_vHalfSize(vHalfSize)
{
	// Empty
}

bool BoundingBox::Intersect(const BoundingSphere& rhs) {
	if (m_type == Type::AABB) {
		point ptMax = m_ptOrigin + m_vHalfSize;
		point ptMin = m_ptOrigin - m_vHalfSize;

		point ptClosestPoint = point::min(point::max(static_cast<BoundingSphere>(rhs).GetOrigin(), ptMin), ptMax);
		double distanceSquared = pow((ptClosestPoint - static_cast<BoundingSphere>(rhs).GetOrigin()).magnitude(), 2.0f);

		if (distanceSquared < pow(static_cast<BoundingSphere>(rhs).GetRadius(), 2.0f))
			return true;
		else
			return false;
	}
	else if (m_type == Type::OBB) {
		// TODO:

		return false;
	}

	return false;
}

bool BoundingBox::Intersect(const BoundingBox& rhs) {
	if (m_type == Type::AABB) {
		point ptMaxA = m_ptOrigin + m_vHalfSize;
		point ptMinA = m_ptOrigin - m_vHalfSize;

		point ptMaxB = rhs.m_ptOrigin + rhs.m_vHalfSize;
		point ptMinB = rhs.m_ptOrigin - rhs.m_vHalfSize;

		if ((ptMaxA > ptMinB) && (ptMaxB > ptMinA))
			return true;
		else
			return false;
	}
	else if (m_type == Type::OBB) {
		// TODO:

		return false;
	}

	return false;
}

//bool Intersect(const point& pt) {
bool BoundingBox::Intersect(point& pt) {
	point ptMax = m_ptOrigin + m_vHalfSize;
	point ptMin = m_ptOrigin - m_vHalfSize;

	if ((pt > ptMin) && (pt < ptMax))
		return true;
	else
		return false;
}

RESULT BoundingBox::SetMaxPointFromOrigin(point ptMax) {
	m_vHalfSize = (ptMax - m_ptOrigin);
	return R_PASS;
}