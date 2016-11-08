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
		point ptMax = GetOrigin() + GetHalfVector();
		point ptMin = GetOrigin() - GetHalfVector();

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
		point ptMaxA = GetOrigin() + GetHalfVector();
		point ptMinA = GetOrigin() - GetHalfVector();

		point ptMaxB = const_cast<BoundingBox&>(rhs).GetOrigin() + static_cast<BoundingBox>(rhs).GetHalfVector();
		point ptMinB = const_cast<BoundingBox&>(rhs).GetOrigin() - static_cast<BoundingBox>(rhs).GetHalfVector();

		if ((ptMaxA > ptMinB) && (ptMaxB > ptMinA))
			return true;
		else
			return false;
	}
	else if (m_type == Type::OBB) {
		for (int i = 0; i < 3; i++) {
			// Self Box Axes
			if (!OverlapOnAxis(rhs, GetAxis(BoundingBox::BoxAxis(i)))) {
				return false;
			}

			// The other box Axes (todo: test if it's an OBB)
			if (!OverlapOnAxis(rhs, static_cast<BoundingBox>(rhs).GetAxis(BoundingBox::BoxAxis(i)))) {
				return false;
			}

			// TODO: Do the cross etc
		}

		return true;
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

	// Rotate the ray by the Rotation Matrix
	// Get origin in reference to object
	if (m_type == Type::OBB) {
		r.vDirection() = inverse(RotationMatrix(GetOrientation())) * r.GetVector();
		r.ptOrigin() = GetOrigin() - (point)(inverse(RotationMatrix(GetOrientation())) * (GetOrigin() - r.GetOrigin()));
	}

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
	m_vHalfSize = (ptMax - GetOrigin());
	return R_PASS;
}

// http://www.willperone.net/Code/coderr.php
vector BoundingBox::GetHalfVector() {
	if (m_type == Type::AABB) {
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
	
	// Otherwise it's OBB
	return m_vHalfSize;
}

bool BoundingBox::OverlapOnAxis(const BoundingBox& rhs, const vector &vAxis) {
	// Project the half-size of one onto axis
	double selfProject = TransformToAxis(vAxis);
	double rhsProject = static_cast<BoundingBox>(rhs).TransformToAxis(vAxis);

	vector vToCenter = static_cast<BoundingBox>(rhs).GetOrigin() - GetOrigin();

	double distance = std::abs(vToCenter.dot(vAxis));

	return (distance <= (selfProject + rhsProject));
}

// Project half size onto vector axis
double BoundingBox::TransformToAxis(const vector &vAxis) {
	double retVal = 0.0f;

	retVal += GetHalfVector().x() * std::abs(vAxis.dot(GetAxis(BoxAxis::X_AXIS)));
	retVal += GetHalfVector().y() * std::abs(vAxis.dot(GetAxis(BoxAxis::Y_AXIS)));
	retVal += GetHalfVector().z() * std::abs(vAxis.dot(GetAxis(BoxAxis::Z_AXIS)));

	return retVal;
}

vector BoundingBox::GetAxis(BoxAxis boxAxis) {
	vector retVector = vector(0.0f, 0.0f, 0.0f);

	switch (boxAxis) {
		case BoxAxis::X_AXIS: retVector = vector::iVector(1.0f); break;
		case BoxAxis::Y_AXIS: retVector = vector::jVector(1.0f); break;
		case BoxAxis::Z_AXIS: retVector = vector::kVector(1.0f); break;
	}

	// Rotate by OBB if so
	if (m_type == Type::OBB) {
		retVector = RotationMatrix(GetOrientation()) * retVector;
		retVector.Normalize();
	}

	return retVector;
}

double BoundingBox::GetWidth() {
	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double width = 0.0f;
		for (int i = 0; i < 8; i++) {
			point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			if (pt.x() > width)
				width = pt.x();
		}

		return static_cast<double>(width * 2.0f);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.x() * 2.0f);
}

double BoundingBox::GetHeight() {
	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double height = 0.0f;
		for (int i = 0; i < 8; i++) {
			point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			if (pt.y() > height)
				height = pt.y();
		}

		return static_cast<double>(height * 2.0f);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.y() * 2.0f);
}

double BoundingBox::GetLength() {
	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double length = 0.0f;
		for (int i = 0; i < 8; i++) {
			point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			if (pt.z() > length)
				length = pt.z();
		}

		return static_cast<double>(length * 2.0f);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.z() * 2.0f);
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