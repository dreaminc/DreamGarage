#include "BoundingVolume.h"

#include "BoundingSphere.h"
#include "BoundingBox.h"
#include "BoundingQuad.h"

#include "VirtualObj.h"

#include "PhysicsEngine/CollisionManifold.h"

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

quaternion BoundingVolume::GetOrientation() { 
	return m_pParent->GetOrientation(); 
}

point BoundingVolume::GetParentOrigin() {
	return (m_pParent->GetOrigin());
}

point BoundingVolume::GetBoundingVolumeOrigin() {
	return (RotationMatrix(GetOrientation()) * vector(m_ptOrigin));
}

point BoundingVolume::GetOrigin() {
	if (!m_ptOrigin.IsZero()) {
		point ptRotated = RotationMatrix(GetOrientation()) * vector(m_ptOrigin);
		return (m_pParent->GetOrigin() + ptRotated);
	}
	else {
		return (m_pParent->GetOrigin());
	}
}

VirtualObj* BoundingVolume::GetParentObject() const {
	return m_pParent;
}