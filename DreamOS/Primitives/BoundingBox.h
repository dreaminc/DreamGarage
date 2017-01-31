#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingBox.h
// Bounding box volume

#include "BoundingVolume.h"

#include "point.h"
#include "vector.h"
#include "line.h"
#include "quaternion.h"

class BoundingBox : public BoundingVolume {
public:
	enum class Type {
		AABB,
		OBB,
		INVALID
	};

	enum class BoxPoint {
		TOP_RIGHT_FAR,
		TOP_RIGHT_NEAR,
		TOP_LEFT_FAR,
		TOP_LEFT_NEAR,
		BOTTOM_RIGHT_FAR,
		BOTTOM_RIGHT_NEAR,
		BOTTOM_LEFT_FAR,
		BOTTOM_LEFT_NEAR,
		INVALID
	};

	enum class BoxFace {
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		FRONT,
		BACK,
		INVALID
	};

	enum class BoxEdge {
		TOP_LEFT,
		TOP_RIGHT,
		TOP_NEAR,
		TOP_FAR,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
		BOTTOM_NEAR,
		BOTTOM_FAR,
		LEFT_NEAR,
		LEFT_FAR,
		RIGHT_NEAR,
		RIGHT_FAR,
		INVALID
	};

	enum class BoxAxis {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		INVALID
	};

	struct face {
		BoxFace m_type;
		point m_points[4];
	};

public:
	BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type);
	BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type, point ptOrigin, vector vHalfSize);

	bool Intersect(const BoundingSphere& rhs);
	bool Intersect(const BoundingBox& rhs);
	bool Intersect(const BoundingQuad& rhs);

	//bool Intersect(const point& pt) {
	bool Intersect(point& pt);
	virtual bool Intersect(const ray& r) override;

	virtual CollisionManifold Collide(const BoundingBox& rhs) override;
	virtual CollisionManifold Collide(const BoundingSphere& rhs) override;
	virtual CollisionManifold Collide(const BoundingQuad& rhs) override;

	virtual RESULT SetMaxPointFromOrigin(point ptMax) override;

	virtual BoundingVolume::Type GetType() override {
		return BoundingVolume::Type::BOX;
	}

	BoundingBox::Type GetBoxType() {
		return m_type;
	}

	// Separating Axis Theorem (SAT) early test
	double TransformToAxis(const vector &vAxis);
	vector GetAxis(BoxAxis boxAxis);
	bool OverlapOnAxis(const BoundingBox& rhs, const vector &vAxis);
	double OverlapOnAxisDistance(const BoundingBox& rhs, const vector &vAxis);

	double GetWidth();
	double GetHeight();
	double GetLength();

	vector GetHalfVector();

	virtual point GetMinPoint() override;
	virtual point GetMaxPoint() override;

	point GetBoxPoint(BoxPoint ptType);
	BoundingBox::face GetFace(BoxFace faceType);
	vector GetBoxFaceNormal(BoxFace faceType);
	line GetBoxEdge(BoxEdge edgeType);

protected:
	BoundingBox::Type m_type;
	vector m_vHalfSize;

	// TODO: this
	quaternion m_qOrientation;

};

#endif // !BOUNDING_BOX_H_
