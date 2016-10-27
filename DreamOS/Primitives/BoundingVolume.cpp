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