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

class VirtualObj;

class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere(VirtualObj *pParentObject);
	BoundingSphere(VirtualObj *pParentObject, point ptOrigin, float radius);

	bool Intersect(const BoundingSphere& rhs);
	bool Intersect(const BoundingBox& rhs);

	bool Intersect(point& pt);
	bool Intersect(line& ln);
	virtual bool Intersect(ray& r) override;

	float GetRadius() {
		return m_radius;
	}

	virtual RESULT SetMaxPointFromOrigin(point ptMax) override;

	virtual BoundingVolume::Type GetType() override {
		return BoundingVolume::Type::SPHERE;
	}

protected:
	float m_radius;
};

#endif // !BOUNDING_VOLUME_H_
