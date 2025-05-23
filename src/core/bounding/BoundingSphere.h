#ifndef BOUNDING_SPHERE_H_
#define BOUNDING_SPHERE_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/bounding/BoundingSphere.h

// Bounding sphere

#include <memory>

#include "BoundingVolume.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"
#include "core/primitives/line.h"

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
	virtual CollisionManifold Collide(const BoundingPlane& rhs) override;

	virtual CollisionManifold Collide(const ray &rCast) override;

	// TODO: Bounding sphere does not work with scale where not all equal
	inline float GetRadius(bool fAbsolute = true) const {
		return m_radius * GetScale(fAbsolute).x();
	}

	inline float GetRadiusSquared(bool fAbsolute = true) const {
		return pow(m_radius * GetScale(fAbsolute).x(), 2.0f);
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
