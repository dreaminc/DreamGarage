#ifndef BOUNDING_SPHERE_H_
#define BOUNDING_SPHERE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingSphere.h
// Bounding sphere

#include "BoundingVolume.h"
#include "point.h"
#include "vector.h"
#include "line.h"

#include <memory>

class VirtualObj;

class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere(VirtualObj *pParentObject);
	BoundingSphere(VirtualObj *pParentObject, point ptOrigin, float radius);

	virtual bool Intersect(const BoundingSphere& rhs) override;
	virtual bool Intersect(const BoundingBox& rhs) override;
	virtual bool Intersect(const BoundingQuad& rhs) override;
	virtual bool Intersect(const BoundingPlane& rhs) override;

	virtual bool Intersect(point& pt) override;
	virtual bool Intersect(const ray &r) override;
	bool Intersect(line& ln);

	virtual CollisionManifold Collide(const BoundingSphere& rhs) override;
	virtual CollisionManifold Collide(const BoundingBox& rhs) override;
	virtual CollisionManifold Collide(const BoundingQuad& rhs) override;

	virtual CollisionManifold Collide(const ray &rCast) override;

	// TODO: Bounding sphere does not work with scale where not all equal
	float GetRadius(bool fAbsolute = true) const {
		return m_radius * GetScale(fAbsolute).x();
	}

	vector GetHalfVector(bool fAbsolute = false);
	virtual RESULT SetHalfVector(vector vHalfVector) override;

	virtual point GetMinPoint(bool fAbsolute = false) override;
	virtual point GetMaxPoint(bool fAbsolute = false) override;
	virtual RESULT SetMaxPointFromOrigin(point ptMax) override;

	virtual BoundingVolume::Type GetType() override {
		return BoundingVolume::Type::SPHERE;
	}

protected:
	float m_radius;
};

#endif // !BOUNDING_VOLUME_H_
