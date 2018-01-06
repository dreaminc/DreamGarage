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
	// TODO:

	return false;
}

bool BoundingPlane::Intersect(const BoundingBox& rhs) {
	// TODO:

	return false;
}

bool BoundingPlane::Intersect(const BoundingQuad& rhs) {
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