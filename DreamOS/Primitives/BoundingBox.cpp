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
		point ptMax = GetOrigin() + m_vHalfSize;
		point ptMin = GetOrigin() - m_vHalfSize;

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
		point ptMaxA = GetOrigin() + m_vHalfSize;
		point ptMinA = GetOrigin() - m_vHalfSize;

		point ptMaxB = const_cast<BoundingBox&>(rhs).GetOrigin() + rhs.m_vHalfSize;
		point ptMinB = const_cast<BoundingBox&>(rhs).GetOrigin() - rhs.m_vHalfSize;

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

	if (m_type == Type::AABB) {
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
	else if (m_type == Type::OBB) {
		// TODO:

		return false;
	}

	return false;
}

RESULT BoundingBox::SetMaxPointFromOrigin(point ptMax) {
	m_vHalfSize = (ptMax - GetOrigin());
	return R_PASS;
}

// http://www.willperone.net/Code/coderr.php
vector BoundingBox::GetHalfVector() {
	RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

	double width = 0.0f;
	double height = 0.0f;
	double length = 0.0f;

	for (int i = 0; i < 8; i++) {
		point pt = rotMat * GetBoxPoint((BoxPoint)(i));

		if (pt.x() > width)
			width = pt.x();

		if (pt.y() > height)
			height = pt.y();

		if (pt.z() > length)
			length = pt.z();
	}

	return vector(width, height, length);
}

double BoundingBox::GetWidth() {
	RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

	double width = 0.0f;
	for (int i = 0; i < 8; i++) {
		point pt = rotMat * GetBoxPoint((BoxPoint)(i));
		if (pt.x() > width)
			width = pt.x();
	}

	return static_cast<double>(width * 2.0f);
}

double BoundingBox::GetHeight() {
	RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

	double height = 0.0f;
	for (int i = 0; i < 8; i++) {
		point pt = rotMat * GetBoxPoint((BoxPoint)(i));
		if (pt.y() > height)
			height = pt.y();
	}

	return static_cast<double>(height * 2.0f);
}

double BoundingBox::GetLength() {
	RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

	double length = 0.0f;
	for (int i = 0; i < 8; i++) {
		point pt = rotMat * GetBoxPoint((BoxPoint)(i));
		if (pt.z() > length)
			length = pt.z();
	}

	return static_cast<double>(length * 2.0f);
}

// TODO: Why do we need to invert the point?
point BoundingBox::GetMinPoint() {
	return (GetOrigin() - GetHalfVector());
}

point BoundingBox::GetMaxPoint() {
	return (GetOrigin() + GetHalfVector());
}

point BoundingBox::GetBoxPoint(BoxPoint ptType) {
	point retPoint = point(m_vHalfSize);
	switch (ptType) {
		case BoxPoint::TOP_RIGHT_FAR: break;// nothing 
		case BoxPoint::TOP_RIGHT_NEAR: retPoint.z() *= -1; break;
		case BoxPoint::TOP_LEFT_FAR: retPoint.x() *= -1; break;
		case BoxPoint::TOP_LEFT_NEAR: retPoint.x() *= -1; retPoint.z() *= -1; break;
		case BoxPoint::BOTTOM_RIGHT_FAR: retPoint.y() *= -1; break;
		case BoxPoint::BOTTOM_RIGHT_NEAR: retPoint.y() *= -1; retPoint.z() *= -1; break;
		case BoxPoint::BOTTOM_LEFT_FAR: retPoint.y() *= -1; retPoint.z() *= -1; break;
		case BoxPoint::BOTTOM_LEFT_NEAR: retPoint.Reverse();
	}

	return retPoint;
}