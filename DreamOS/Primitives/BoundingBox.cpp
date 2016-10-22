#include "BoundingBox.h"
#include "BoundingSphere.h"
#include <algorithm>

#include "VirtualObj.h"

BoundingBox::BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type) :
	BoundingVolume(pParentObject),
	m_type(type),
	m_vHalfSize(vector(1.0f, 1.0f, 1.0f))
{
	// Empty
}

BoundingBox::BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type, point ptOrigin, vector vHalfSize) :
	BoundingVolume(pParentObject, ptOrigin),
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
	point ptMin = GetMinPoint();
	point ptMax = GetMaxPoint();

	if ((pt > ptMin) && (pt < ptMax)) {
		return true;
	}
	else {
		return false;
	}
}

// https://tavianator.com/fast-branchless-raybounding-box-intersections/
bool BoundingBox::Intersect(ray& r) {
	double tmin = -INFINITY, tmax = INFINITY;
	
	point ptMin = GetMinPoint();
	point ptMax = GetMaxPoint();

	for (int i = 0; i < 3; i++) {
		double t1 = (ptMin(i) - r.ptOrigin()(i)) / r.vDirection()(i);
		double t2 = (ptMax(i) - r.ptOrigin()(i)) / r.vDirection()(i);

		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));
	}

	return (tmax >= tmin);
}

RESULT BoundingBox::SetMaxPointFromOrigin(point ptMax) {
	m_vHalfSize = (ptMax - m_ptOrigin);
	return R_PASS;
}

double BoundingBox::GetWidth() {
	return static_cast<double>(m_vHalfSize.x() * 2.0f);
}

double BoundingBox::GetHeight() {
	return static_cast<double>(m_vHalfSize.y() * 2.0f);
}

double BoundingBox::GetLength() {
	return static_cast<double>(m_vHalfSize.z() * 2.0f);
}

// TODO: Why do we need to invert the point?
point BoundingBox::GetMinPoint() {
	return ((-1.0f * m_pParent->GetOrigin() - m_ptOrigin) - m_vHalfSize);
}

point BoundingBox::GetMaxPoint() {
	return ((-1.0f * m_pParent->GetOrigin() - m_ptOrigin) + m_vHalfSize);
}