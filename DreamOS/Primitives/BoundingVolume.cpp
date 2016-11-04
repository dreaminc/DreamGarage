#include "BoundingVolume.h"

#include "BoundingSphere.h"
#include "BoundingBox.h"

#include "VirtualObj.h"

bool BoundingVolume::Intersect(BoundingVolume* pRHS) {
	BoundingBox *pBoundingBox = dynamic_cast<BoundingBox*>(pRHS);
	if (pBoundingBox != nullptr)
		return Intersect(*pBoundingBox);

	BoundingSphere *pBoundingSphere = dynamic_cast<BoundingSphere*>(pRHS);
	if (pBoundingSphere != nullptr)
		return Intersect(*pBoundingSphere);

	return false;
}

quaternion BoundingVolume::GetOrientation() { 
	return m_pParent->GetOrientation(); 
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