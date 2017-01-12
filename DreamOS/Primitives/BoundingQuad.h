#ifndef BOUNDING_QUAD_H_
#define BOUNDING_QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingQuad.h
// Bounding Quad isn't technically a volume

#include "BoundingVolume.h"

#include "point.h"
#include "vector.h"
#include "line.h"
#include "quaternion.h"

class BoundingQuad : public BoundingVolume {
public:
	enum class QuadPoint {
		TOP_RIGHT,
		TOP_LEFT,
		BOTTOM_RIGHT,
		BOTTOM_LEFT,
		INVALID
	};

public:
	BoundingQuad(VirtualObj *pParentObject);
	BoundingQuad(VirtualObj *pParentObject, point ptOrigin, vector vNormal, double width, double height);

	virtual bool Intersect(const BoundingSphere& rhs) override;
	virtual bool Intersect(const BoundingBox& rhs) override;
	virtual bool Intersect(const BoundingQuad& rhs) override;

	bool Intersect(point& pt);
	virtual bool Intersect(const ray& r) override;

	virtual CollisionManifold Collide(const BoundingBox& rhs) override;
	virtual CollisionManifold Collide(const BoundingSphere& rhs) override;
	virtual CollisionManifold Collide(const BoundingQuad& rhs) override;
	

	virtual RESULT SetMaxPointFromOrigin(point ptMax) override;

	virtual BoundingVolume::Type GetType() override {
		return BoundingVolume::Type::QUAD;
	}

	double GetWidth();
	double GetHeight();
	vector GetNormal();
	vector GetHalfVector();

	point GetMinPoint();
	point GetMaxPoint();
	point GetQuadPoint(QuadPoint ptType);

protected:
	double m_width;
	double m_height;
	vector m_vNormal;

	quaternion m_qOrientation;

};

#endif // !BOUNDING_BOX_H_
