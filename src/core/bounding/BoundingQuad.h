#ifndef BOUNDING_QUAD_H_
#define BOUNDING_QUAD_H_

#include "core/ehm/EHM.h"

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

	enum class QuadEdge {
		RIGHT,
		LEFT,
		TOP,
		BOTTOM,
		INVALID
	};

public:
	BoundingQuad(VirtualObj *pParentObject);
	BoundingQuad(VirtualObj *pParentObject, point ptOrigin, vector vNormal, double width, double height);

	virtual bool Intersect(const BoundingSphere& rhs) override;
	virtual bool Intersect(const BoundingBox& rhs) override;
	virtual bool Intersect(const BoundingQuad& rhs) override;
	virtual bool Intersect(const BoundingPlane& rhs) override;

	virtual bool Intersect(point& pt) override;
	virtual bool Intersect(const ray& r) override;
	bool Intersect(const line& l, point *pptCollision = nullptr);

	virtual CollisionManifold Collide(const BoundingBox& rhs) override;
	virtual CollisionManifold Collide(const BoundingSphere& rhs) override;
	virtual CollisionManifold Collide(const BoundingQuad& rhs) override;
	virtual CollisionManifold Collide(const BoundingPlane& rhs) override;

	virtual CollisionManifold Collide(const ray &rCast) override;
	
	virtual RESULT SetMaxPointFromOrigin(point ptMax) override;

	virtual BoundingVolume::Type GetType() override {
		return BoundingVolume::Type::QUAD;
	}

	// Quad specific 
	RESULT SetBounds(float width, float height);

	double GetWidth(bool fAbsolute = false);
	double GetHeight(bool fAbsolute = false);

	double GetLeft(bool fAbsolute = true);
	double GetRight(bool fAbsolute = true);
	double GetTop(bool fAbsolute = true);
	double GetBottom(bool fAbsolute = true);

	vector GetNormal();
	vector GetAbsoluteNormal();

	vector GetHalfVector(bool fAbsolute = false);
	virtual RESULT SetHalfVector(vector vHalfVector) override;

	virtual point GetMinPoint(bool fAbsolute = false) override;
	virtual point GetMaxPoint(bool fAbsolute = false) override;
	
	point GetQuadPoint(QuadPoint ptType);
	point GetAbsoluteQuadPoint(QuadPoint ptType);

	line GetAbsoluteQuadEdge(QuadEdge edgeType);

protected:
	double m_width;
	double m_height;
	vector m_vNormal;

	quaternion m_qOrientation;

};

#endif // !BOUNDING_BOX_H_
