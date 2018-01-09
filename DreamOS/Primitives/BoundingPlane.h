#ifndef BOUNDING_PLANE_H_
#define BOUNDING_PLANE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingPlane.h
// Bounding Plane isn't technically a volume

#include "BoundingVolume.h"

#include "point.h"
#include "vector.h"
#include "line.h"
#include "quaternion.h"

class BoundingPlane : public BoundingVolume {

public:
	BoundingPlane(VirtualObj *pParentObject);
	BoundingPlane(VirtualObj *pParentObject, point ptOrigin, vector vNormal);

	virtual bool Intersect(const BoundingSphere& rhs) override;
	virtual bool Intersect(const BoundingBox& rhs) override;
	virtual bool Intersect(const BoundingQuad& rhs) override;
	virtual bool Intersect(const BoundingPlane& rhs) override;

	virtual bool Intersect(point& pt) override;
	virtual bool Intersect(const ray& r) override;

	virtual CollisionManifold Collide(const BoundingBox& rhs) override;
	virtual CollisionManifold Collide(const BoundingSphere& rhs) override;
	virtual CollisionManifold Collide(const BoundingQuad& rhs) override;
	virtual CollisionManifold Collide(const BoundingPlane& rhs) override;

	virtual CollisionManifold Collide(const ray &rCast) override;

	vector GetNormal() const;

	virtual BoundingVolume::Type GetType() override {
		return BoundingVolume::Type::PLANE;
	}
	
	virtual RESULT SetMaxPointFromOrigin(point ptMax) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT SetHalfVector(vector vHalfVector) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual point GetMinPoint(bool fAbsolute = false) override {
		return point();
	}

	virtual point GetMaxPoint(bool fAbsolute = false) override {
		return point();
	}

protected:
	vector m_vNormal;

	quaternion m_qOrientation;
};

#endif // !BOUNDING_BOX_H_
