#include "BoundingQuad.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include <algorithm>

#include "VirtualObj.h"
#include "PhysicsEngine/CollisionManifold.h"

BoundingQuad::BoundingQuad(VirtualObj *pParentObject) :
	BoundingVolume(pParentObject),
	m_width(0.0f),
	m_height(0.0f),
	m_vNormal(vector())
{
	// empty
}

BoundingQuad::BoundingQuad(VirtualObj *pParentObject, point ptOrigin, vector vNormal, double width, double height) :
	BoundingVolume(pParentObject, ptOrigin),
	m_width(width),
	m_height(height),
	m_vNormal(vNormal)
{
	// empty
}

bool BoundingQuad::Intersect(const BoundingBox& rhs) {
	// TODO:

	return false;
}

bool BoundingQuad::Intersect(const BoundingQuad& rhs) {
	// TODO:

	return false;
}

bool BoundingQuad::Intersect(point& pt) {
	// TODO:

	return false;
}

CollisionManifold BoundingQuad::Collide(const BoundingQuad& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

CollisionManifold BoundingQuad::Collide(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

bool BoundingQuad::Intersect(const BoundingSphere& rhs) {
	// First calculate rotation per normal and re-orient

	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(rotMat) * (static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin());
	double distance = ptSphereOrigin.y();

	if (std::abs(distance) < static_cast<BoundingSphere>(rhs).GetRadius()) {
		//point ptMax = GetMaxPoint();
		//point ptMin = GetMinPoint();

		///*
		point ptMax = point(GetWidth() / 2.0f, GetHeight() / 2.0f, 0.0f);
		point ptMin = point((-1.0f * GetWidth()) / 2.0f, (-1.0f * GetHeight()) / 2.0f, 0.0f);

		float closestX = std::max(ptMin.x(), std::min(ptSphereOrigin.x(), ptMax.x()));
		float closestY = std::max(ptMin.y(), std::min(ptSphereOrigin.y(), ptMax.y()));
		float closestZ = std::max(ptMin.z(), std::min(ptSphereOrigin.z(), ptMax.z()));
		//*/

		//point ptClosestPoint = ptSphereOrigin - (m_vNormal * distance);
		point ptClosestPoint = point(closestX, closestY, closestZ);

		double sphereRadiusSquared = pow(static_cast<BoundingSphere>(rhs).GetRadius(), 2.0f);
		double distanceSquared = pow((ptClosestPoint - ptSphereOrigin).magnitude(), 2.0f);

		if (distanceSquared < sphereRadiusSquared) {
			return true;
		}
	}

	return false;
}

CollisionManifold BoundingQuad::Collide(const BoundingSphere& rhs) {
	// First calculate rotation per normal and re-orient

	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(rotMat) * (static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin());
	double distance = ptSphereOrigin.y();

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	if (std::abs(distance) < static_cast<BoundingSphere>(rhs).GetRadius()) {
		//point ptMax = GetMaxPoint();
		//point ptMin = GetMinPoint();

		///*
		point ptMax = point(GetWidth() / 2.0f, GetHeight() / 2.0f, 0.0f);
		point ptMin = point((-1.0f * GetWidth()) / 2.0f, (-1.0f * GetHeight())/2.0f, 0.0f);

		float closestX = std::max(ptMin.x(), std::min(ptSphereOrigin.x(), ptMax.x()));
		float closestY = std::max(ptMin.y(), std::min(ptSphereOrigin.y(), ptMax.y()));
		float closestZ = std::max(ptMin.z(), std::min(ptSphereOrigin.z(), ptMax.z()));
		//*/

		//if(ptSphereOrigin.x() )

		//point ptClosestPoint = ptSphereOrigin - (m_vNormal * distance);
		//point ptClosestPoint = point(closestX, closestY, closestZ);
		point ptClosestPoint = point(closestX, 0.0f, closestZ);

		double sphereRadiusSquared = pow(static_cast<BoundingSphere>(rhs).GetRadius(), 2.0f);
		double distanceSquared = pow((ptClosestPoint - ptSphereOrigin).magnitude(), 2.0f);

		//if (closestX <= 0.0f && closestZ < 0.0f) {
		if (distanceSquared <= sphereRadiusSquared) {
			// This is a plane-point collision

			ptClosestPoint = (rotMat * ptClosestPoint) + GetAbsoluteOrigin();
			
			//vector vNormal = rotMat * vector::jVector(1.0f);
			//vNormal.Normalize();

			vector vNormal = static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - ptClosestPoint;
			vNormal.Normalize();

			point ptContact = ptClosestPoint;
			float penetration = static_cast<BoundingSphere>(rhs).GetRadius() - std::sqrt(distanceSquared);

			manifold.AddContactPoint(ptContact, vNormal, -penetration, 1);
		}
		/*else if (distanceSquared <= sphereRadiusSquared) {
			// Otherwise it's a point edge collision
		}*/
		
	}
	
	return manifold;
}

bool BoundingQuad::Intersect(const ray& r) {
	double t = -1.0f;

	vector vNormal = RotationMatrix(GetAbsoluteOrientation()) * m_vNormal;
	vNormal.Normalize();

	t = ((vector)(GetAbsoluteOrigin() - r.GetOrigin())).dot(vNormal);
	double denom = r.GetVector().Normal().dot(vNormal);

	if (denom != 0) {
		t /= denom;

		if (t >= 0) {
			quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
			RotationMatrix rotMat = RotationMatrix(qOrientation);

			point ptPlane = inverse(rotMat) * ((r.GetOrigin() + r.GetVector() * t) - GetAbsoluteOrigin());

			if (ptPlane.x() < GetWidth() / 2.0f && ptPlane.x() > -GetWidth() / 2.0f &&
				ptPlane.z() < GetHeight() / 2.0f && ptPlane.z() > -GetHeight() / 2.0f)
			{
				return true;
			}
		}
	}
	else {
		// parallel 
	}

	return false;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
// Note this does not check bounds like intersect
CollisionManifold BoundingQuad::Collide(const ray &rCast) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, nullptr);

	double t = -1.0f;

	vector vNormal = RotationMatrix(GetAbsoluteOrientation()) * m_vNormal;
	vNormal.Normalize();
	
	t = ((vector)(GetAbsoluteOrigin() - rCast.GetOrigin())).dot(vNormal);
	double denom = rCast.GetVector().Normal().dot(vNormal);

	if (denom != 0) {
		t /= denom;

		if (t > 0) {
			point ptContact = rCast.GetOrigin() + rCast.GetVector() * t;
			manifold.AddContactPoint(ptContact, vNormal, 0.0f, 1);
		}
	}
	else {
		// parallel 
	}

	return manifold;
}

RESULT BoundingQuad::SetMaxPointFromOrigin(point ptMax) {		
	vector vDiff = (ptMax - GetOrigin());
	return R_SUCCESS;
}

RESULT BoundingQuad::SetHalfVector(vector vHalfVector) {
	return R_NOT_IMPLEMENTED;
}

double BoundingQuad::GetWidth() {
	return m_width * GetScale().x();
}

double BoundingQuad::GetHeight() {
	return m_height* GetScale().y();
}

vector BoundingQuad::GetNormal() {
	return m_vNormal;
}

// This is busted
vector BoundingQuad::GetHalfVector() {
	vector vScale = GetScale();
	//RotationMatrix matRotation = RotationMatrix(GetAbsoluteOrientation());	// .GetEulerAngles(&phi, &theta, &psi);
	RotationMatrix matRotation = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

	double width = 0.0f;
	double height = 0.0f;
	double length = 0.0f;

	for (int i = 0; i < 8; i++) {
		point pt = GetQuadPoint((QuadPoint)(i));
		pt = matRotation * pt;

		if (pt.x() > width)
			width = pt.x();

		if (pt.y() > height)
			height = pt.y();

		if (pt.z() > length)
			length = pt.z();
	}

	return vector(width * vScale.x(), height * vScale.y(), length * vScale.z());
}

point BoundingQuad::GetMinPoint() {
	return (GetHalfVector() * -1.0f) + GetOrigin();
}

point BoundingQuad::GetMaxPoint() {
	return GetHalfVector() + GetOrigin();
}

point BoundingQuad::GetQuadPoint(QuadPoint ptType) {
	point ptRet = point();

	double halfWidth = m_width / 2.0f;
	double halfHeight = m_height / 2.0f;

	switch (ptType) {
		case QuadPoint::TOP_RIGHT: {
			ptRet.x() += halfWidth;
			ptRet.z() += halfHeight;
		} break;

		case QuadPoint::TOP_LEFT: {
			ptRet.x() -= halfWidth;
			ptRet.z() += halfHeight;
		} break;

		case QuadPoint::BOTTOM_RIGHT: {
			ptRet.x() += halfWidth;
			ptRet.z() -= halfHeight;
		} break;

		case QuadPoint::BOTTOM_LEFT: {
			ptRet.x() -= halfWidth;
			ptRet.z() -= halfHeight;
		} break;
	}

	// Accounts for normal rotation
	quaternion qOrientation = quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix matRotation(qOrientation);
	ptRet = matRotation * ptRet;

	//ptRet = ptRet + GetOrigin();
		
	return ptRet;
}