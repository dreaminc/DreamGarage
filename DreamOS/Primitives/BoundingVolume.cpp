#include "BoundingVolume.h"

#include "BoundingSphere.h"
#include "BoundingBox.h"
#include "BoundingQuad.h"

#include "VirtualObj.h"

#include "PhysicsEngine/CollisionManifold.h"

BoundingVolume::BoundingVolume(VirtualObj *pParentObject) :
	m_pParent(pParentObject),
	m_ptCenter()
{
	// Empty
}

BoundingVolume::BoundingVolume(VirtualObj *pParentObject, point ptOrigin) :
	m_pParent(pParentObject),
	m_ptCenter(ptOrigin)
{
	// Empty
}

bool BoundingVolume::Intersect(BoundingVolume* pRHS) {
	BoundingBox *pBoundingBox = dynamic_cast<BoundingBox*>(pRHS);
	if (pBoundingBox != nullptr)
		return Intersect(*pBoundingBox);

	BoundingSphere *pBoundingSphere = dynamic_cast<BoundingSphere*>(pRHS);
	if (pBoundingSphere != nullptr)
		return Intersect(*pBoundingSphere);

	BoundingQuad *pBoundingQuad = dynamic_cast<BoundingQuad*>(pRHS);
	if (pBoundingQuad != nullptr)
		return Intersect(*pBoundingQuad);

	return false;
}

CollisionManifold BoundingVolume::Collide(BoundingVolume* pRHS) {
	// Sphere
	BoundingSphere *pBoundingSphere = dynamic_cast<BoundingSphere*>(pRHS);
	if (pBoundingSphere != nullptr) {
		return Collide(*pBoundingSphere);
	}

	// Box
	BoundingBox *pBoundingBox = dynamic_cast<BoundingBox*>(pRHS);
	if (pBoundingBox != nullptr) {
		return Collide(*pBoundingBox);
	}

	// Quad
	BoundingQuad *pBoundingQuad = dynamic_cast<BoundingQuad*>(pRHS);
	if (pBoundingQuad != nullptr) {
		return Collide(*pBoundingQuad);
	}

	return CollisionManifold(this->m_pParent, pRHS->GetParentObject());
}

vector BoundingVolume::GetScale() {
	return m_pParent->GetScale();
}

quaternion BoundingVolume::GetOrientation() { 
	return m_pParent->GetOrientation(); 
}

point BoundingVolume::GetParentOrigin() {
	return (m_pParent->GetOrigin());
}

point BoundingVolume::GetParentPivot() {
	return (m_pParent->GetPivotPoint());
}

point BoundingVolume::GetBoundingVolumeOrigin() {
	point ptRotated = RotationMatrix(GetOrientation()) * vector(m_ptCenter);
	return (m_pParent->GetOrigin() + ptRotated);
}

point BoundingVolume::GetCenter() {
	return m_ptCenter;
}

quaternion BoundingVolume::GetAbsoluteOrientation() {
	return m_pParent->GetOrientation(true);
}

point BoundingVolume::GetAbsoluteOrigin() {
	if (!m_ptCenter.IsZero()) {
		point ptRotated = RotationMatrix(GetAbsoluteOrientation()) * vector(m_ptCenter);
		return (m_pParent->GetOrigin(true) + ptRotated);
	}
	else {
		return (m_pParent->GetOrigin(true));
	}
}

point BoundingVolume::GetOrigin() {
	if (!m_ptCenter.IsZero()) {
		point ptRotated = RotationMatrix(GetOrientation()) * vector(m_ptCenter);
		return (m_pParent->GetOrigin() + ptRotated);
	}
	else {
		return (m_pParent->GetOrigin());
	}
}

VirtualObj* BoundingVolume::GetParentObject() const {
	return m_pParent;
}

RESULT BoundingVolume::UpdateBoundingVolume(point ptOrigin, point ptMax) {
	RESULT r = R_PASS;

	CR(SetCenter(ptOrigin));
	CR(SetMaxPointFromOrigin(ptMax));

Error:
	return r;
}

RESULT BoundingVolume::UpdateBoundingVolumeMinMax(point ptMin, point ptMax) {
	RESULT r = R_PASS;

	point ptMid = point::midpoint(ptMax, ptMin);
	vector vHalfVector = ptMax - ptMid;

	CR(SetCenter(ptMid));
	CR(SetHalfVector(vHalfVector));

Error:
	return r;
}

RESULT BoundingVolume::SetCenter(point ptOrigin) {
	m_ptCenter = ptOrigin;
	return R_PASS;
}