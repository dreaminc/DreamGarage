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

	quaternion_precision phi, theta, psi;
	GetOrientation().GetEulerAngles(&phi, &theta, &psi);

	rotation_precision cosPhi = static_cast<rotation_precision>(cos(phi));
	rotation_precision sinPhi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(phi));

	rotation_precision cosTheta = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinTheta = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	rotation_precision cosPsi = static_cast<rotation_precision>(cos(psi));
	rotation_precision sinPsi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(psi));

	double width = m_vHalfSize.x() * fabs(fabs(sinPhi * sinPsi) - fabs(cosPhi * sinTheta * cosPsi)) + m_vHalfSize.y() * fabs(fabs(sinPhi * cosPsi) + fabs(cosPhi * sinTheta * sinPsi)) + m_vHalfSize.z() * fabs(cosPhi * cosTheta);
	double height = m_vHalfSize.x() * fabs(fabs(cosPhi * sinPsi) + fabs(sinPhi * sinTheta * cosPsi)) + m_vHalfSize.y() * fabs(fabs(cosPhi * cosPsi) - fabs(sinPhi * sinTheta * sinPsi)) + m_vHalfSize.z() * fabs(sinPhi * cosTheta);
	double length = m_vHalfSize.x() * fabs(cosTheta * cosPsi) + m_vHalfSize.y() * fabs(cosTheta * sinPsi) + m_vHalfSize.z() * fabs(sinTheta);

	return vector(width, height, length);
}

double BoundingBox::GetWidth() {
	quaternion_precision phi, theta, psi; 
	GetOrientation().GetEulerAngles(&phi, &theta, &psi);

	rotation_precision cosPhi = static_cast<rotation_precision>(cos(phi));
	rotation_precision sinPhi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(phi));

	rotation_precision cosTheta = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinTheta = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	rotation_precision cosPsi = static_cast<rotation_precision>(cos(psi));
	rotation_precision sinPsi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(psi));

	//double width = GetHalfVector().x() * fabs(cosTheta * cosPsi) + GetHalfVector().y() * fabs(cosTheta * sinPsi) + GetHalfVector().y() * fabs(sinTheta);
	//double width = GetHalfVector().x() * fabs((sinPhi * sinPsi) - (cosPhi * sinTheta * cosPsi)) + GetHalfVector().y() * fabs((sinPhi * cosPsi) + (cosPhi * sinTheta * sinPsi)) + GetHalfVector().y() * fabs(cosPhi * cosTheta);
	double width = m_vHalfSize.x() * fabs(fabs(sinPhi * sinPsi) - fabs(cosPhi * sinTheta * cosPsi)) + m_vHalfSize.y() * fabs(fabs(sinPhi * cosPsi) + fabs(cosPhi * sinTheta * sinPsi)) + m_vHalfSize.z() * fabs(cosPhi * cosTheta);

	return static_cast<double>(width * 2.0f);
}

double BoundingBox::GetHeight() {
	quaternion_precision phi, theta, psi;
	GetOrientation().GetEulerAngles(&phi, &theta, &psi);

	rotation_precision cosPhi = static_cast<rotation_precision>(cos(phi));
	rotation_precision sinPhi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(phi));

	rotation_precision cosTheta = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinTheta = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	rotation_precision cosPsi = static_cast<rotation_precision>(cos(psi));
	rotation_precision sinPsi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(psi));

	//double height = GetHalfVector().x() * fabs((cosPhi * sinPsi) + (sinPhi * sinTheta * cosPsi)) + GetHalfVector().y() * fabs((cosPhi * cosPsi) - (sinPhi * sinTheta * sinPsi)) + GetHalfVector().y() * fabs(-sinPhi * cosTheta);
	double height = m_vHalfSize.x() * fabs(fabs(cosPhi * sinPsi) + fabs(sinPhi * sinTheta * cosPsi)) + m_vHalfSize.y() * fabs(fabs(cosPhi * cosPsi) - fabs(sinPhi * sinTheta * sinPsi)) + m_vHalfSize.z() * fabs(sinPhi * cosTheta);

	return static_cast<double>(height * 2.0f);
}

double BoundingBox::GetLength() {
	quaternion_precision phi, theta, psi;
	GetOrientation().GetEulerAngles(&phi, &theta, &psi);

	rotation_precision cosPhi = static_cast<rotation_precision>(cos(phi));
	rotation_precision sinPhi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(phi));

	rotation_precision cosTheta = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinTheta = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	rotation_precision cosPsi = static_cast<rotation_precision>(cos(psi));
	rotation_precision sinPsi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(psi));

	//double length = GetHalfVector().x() * fabs((sinPhi * sinPsi) - fabs(cosPhi * sinTheta * cosPsi)) + GetHalfVector().y() * fabs((sinPhi * cosPsi) + (cosPhi * sinTheta * sinPsi)) + GetHalfVector().y() * fabs(cosPhi * cosTheta);
	double length = m_vHalfSize.x() * fabs(cosTheta * cosPsi) + m_vHalfSize.y() * fabs(cosTheta * sinPsi) + m_vHalfSize.z() * fabs(sinTheta);

	return static_cast<double>(length * 2.0f);
}

// TODO: Why do we need to invert the point?
point BoundingBox::GetMinPoint() {
	return (GetOrigin() - GetHalfVector());
}

point BoundingBox::GetMaxPoint() {
	return (GetOrigin() + GetHalfVector());
}