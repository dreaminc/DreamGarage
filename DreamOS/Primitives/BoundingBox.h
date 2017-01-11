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

	enum class BoxAxis {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		INVALID
	};

public:
	BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type);
	BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type, point ptOrigin, vector vHalfSize);

	bool Intersect(const BoundingSphere& rhs);
	bool Intersect(const BoundingBox& rhs);

	//bool Intersect(const point& pt) {
	bool Intersect(point& pt);
	virtual bool Intersect(const ray& r) override;

	virtual CollisionManifold Collide(const BoundingVolume& rhs) override;
	virtual CollisionManifold Collide(const BoundingSphere& rhs) override;

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

	double GetWidth();
	double GetHeight();
	double GetLength();

	vector GetHalfVector();

	point GetMinPoint();
	point GetMaxPoint();

	point GetBoxPoint(BoxPoint ptType);

protected:
	BoundingBox::Type m_type;
	vector m_vHalfSize;

	// TODO: this
	quaternion m_qOrientation;

};

#endif // !BOUNDING_BOX_H_
