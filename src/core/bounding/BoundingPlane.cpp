#include "BoundingPlane.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingQuad.h"
#include <algorithm>

#include "plane.h"

#include "VirtualObj.h"
#include "PhysicsEngine/CollisionManifold.h"

BoundingPlane::BoundingPlane(VirtualObj *pParentObject) :
	BoundingVolume(pParentObject),
	m_vNormal(vector::jVector(1.0f))
{
	//empty
}

BoundingPlane::BoundingPlane(VirtualObj *pParentObject, point ptOrigin, vector vNormal) :
	BoundingVolume(pParentObject, ptOrigin),
	m_vNormal(vNormal)
{
	// empty
}

bool BoundingPlane::Intersect(const BoundingSphere& rhs) {
	// First calculate rotation per normal and re-orient

	//quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix matRotation = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(matRotation) * (static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin());
	double distance = ptSphereOrigin.y();

	if (std::abs(distance) < static_cast<BoundingSphere>(rhs).GetRadius()) {
		return true;
	}
	else {
		return false;
	}
}

bool BoundingPlane::Intersect(const BoundingBox& rhs) {
	
	vector vBoxHalfVector = RotationMatrix(static_cast<BoundingBox>(rhs).GetAbsoluteOrientation()) * static_cast<BoundingBox>(rhs).GetHalfVector(true);

	vector vPlaneNormal = GetNormal();

	vector vProjectHVOnPlaneNormal = (vBoxHalfVector.dot(vPlaneNormal)) * vPlaneNormal;

	point ptBoxOrigin = static_cast<BoundingBox>(rhs).GetOrigin();
	point ptPlaneOrigin = GetOrigin();

	float distanceFromPlane = plane(ptPlaneOrigin, vPlaneNormal).Distance(ptBoxOrigin);
	float projectionMagnitude = vProjectHVOnPlaneNormal.magnitude();

	if (std::abs(distanceFromPlane) <= projectionMagnitude) {
		return true;
	}

	return false;
}

bool BoundingPlane::Intersect(const BoundingQuad& rhs) {
	//vector vQuadHalfVector = RotationMatrix(static_cast<BoundingQuad>(rhs).GetAbsoluteOrientation()) * static_cast<BoundingQuad>(rhs).GetHalfVector(true);
	vector vQuadHalfVector = static_cast<BoundingQuad>(rhs).GetHalfVector(true);

	vector vPlaneNormal = GetNormal();

	vector vProjectHVOnPlaneNormal = (vQuadHalfVector.dot(vPlaneNormal)) * vPlaneNormal;

	point ptQuadOrigin = static_cast<BoundingQuad>(rhs).GetOrigin();
	point ptPlaneOrigin = GetOrigin();

	float distanceFromPlane = plane(ptPlaneOrigin, vPlaneNormal).Distance(ptQuadOrigin);
	float projectionMagnitude = vProjectHVOnPlaneNormal.magnitude();

	if (std::abs(distanceFromPlane) <= projectionMagnitude) {
		return true;
	}

	return false;
}

bool BoundingPlane::Intersect(const BoundingPlane& rhs) {
	vector vCross = rhs.GetNormal().cross(m_vNormal);

	if (vCross.magnitude() == 0.0f) {
		return Intersect(static_cast<BoundingPlane>(rhs).GetOrigin());
	}
	
	return true;
}

bool BoundingPlane::Intersect(point& pt) {
	float distance = plane(GetOrigin(), GetNormal()).Distance(pt);

	if (distance == 0) {
		return true;
	}
	
	return false;
}

bool BoundingPlane::Intersect(const ray& r) {
	double t = -1.0f;

	vector vNormal = RotationMatrix(GetAbsoluteOrientation()) * m_vNormal;
	vNormal.Normalize();

	t = ((vector)(GetAbsoluteOrigin() - r.GetOrigin())).dot(vNormal);
	double denom = r.GetVector().Normal().dot(vNormal);

	if (denom != 0) {
		t /= denom;

		if (t >= 0) {
			return true;
		}
	}
	else {
		// parallel 
	}

	return false;
}

CollisionManifold BoundingPlane::Collide(const BoundingBox& rhs) {
	return static_cast<BoundingBox>(rhs).Collide(*this);
}

CollisionManifold BoundingPlane::Collide(const BoundingSphere& rhs) {
	// First calculate rotation per normal and re-orient

	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(rotMat) * (static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin());
	double distance = ptSphereOrigin.y();

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());
	float absDistance = std::abs(distance);

	if (absDistance < static_cast<BoundingSphere>(rhs).GetRadius()) {

		point ptClosestPoint = point(ptSphereOrigin.x(), 0.0f, ptSphereOrigin.z());
		ptClosestPoint = (rotMat * ptClosestPoint) + GetAbsoluteOrigin();

		vector vNormal = static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - ptClosestPoint;
		vNormal.Normalize();

		point ptContact = ptClosestPoint;
		float penetration = static_cast<BoundingSphere>(rhs).GetRadius() - absDistance;

		manifold.AddContactPoint(ptContact, vNormal, -penetration, 1);
	}

	return manifold;
}

CollisionManifold BoundingPlane::Collide(const BoundingQuad& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

CollisionManifold BoundingPlane::Collide(const BoundingPlane& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:  This will return a line so return two points

	return manifold;
}

CollisionManifold BoundingPlane::Collide(const ray &rCast) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, nullptr);

	double t = -1.0f;

	vector vNormal = RotationMatrix(GetAbsoluteOrientation()) * m_vNormal;
	vNormal.Normalize();

	t = ((vector)(GetAbsoluteOrigin() - rCast.GetOrigin())).dot(vNormal);
	double denom = rCast.GetVector().Normal().dot(vNormal);

	if (denom != 0) {
		t /= denom;

		if (t >= 0) {
			point ptContact = rCast.GetOrigin() + rCast.GetVector() * t;
			manifold.AddContactPoint(ptContact, vNormal, 0.0f, 1);
		}
	}
	else {
		// parallel 
	}

	return manifold;
}

vector BoundingPlane::GetNormal() const {
	return m_vNormal;
}