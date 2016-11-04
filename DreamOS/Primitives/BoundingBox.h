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

public:
	BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type);
	BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type, point ptOrigin, vector vHalfSize);

	bool Intersect(const BoundingSphere& rhs);
	bool Intersect(const BoundingBox& rhs);

	//bool Intersect(const point& pt) {
	bool Intersect(point& pt);
	virtual bool Intersect(ray& r) override;

	virtual RESULT SetMaxPointFromOrigin(point ptMax) override;

	virtual BoundingVolume::Type GetType() override {
		return BoundingVolume::Type::BOX;
	}

	double GetWidth();
	double GetHeight();
	double GetLength();

	vector GetHalfVector();

	point GetMinPoint();
	point GetMaxPoint();

protected:
	BoundingBox::Type m_type;
	vector m_vHalfSize;

	// TODO: this
	quaternion m_qOrientation;

};

#endif // !BOUNDING_BOX_H_
