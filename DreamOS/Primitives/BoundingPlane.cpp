#include "BoundingPlane.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingQuad.h"
#include <algorithm>

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
	// TODO:

	return false;
}

bool BoundingPlane::Intersect(const BoundingQuad& rhs) {
	// TODO:

	return false;
}

bool BoundingPlane::Intersect(const BoundingPlane& rhs) {
	// TODO:

	return false;
}

bool BoundingPlane::Intersect(point& pt) {
	// TODO:

	return false;
}

bool BoundingPlane::Intersect(const ray& r) {
	// TODO:

	return false;
}

CollisionManifold BoundingPlane::Collide(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

CollisionManifold BoundingPlane::Collide(const BoundingSphere& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

CollisionManifold BoundingPlane::Collide(const BoundingQuad& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

CollisionManifold BoundingPlane::Collide(const ray &rCast) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, nullptr);

	// TODO:

	return manifold;
}

vector BoundingPlane::GetNormal() {
	return m_vNormal;
}