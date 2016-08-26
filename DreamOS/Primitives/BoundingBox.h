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
	typedef enum class Type {
		AABB,
		OBB,
		INVALID
	};

public:
	BoundingBox();
	BoundingBox(point ptOrigin, vector vHalfSize);

	bool Intersect(const BoundingSphere& rhs);
	bool Intersect(const BoundingBox& rhs);

	//bool Intersect(const point& pt) {
	bool Intersect(point& pt);

	virtual RESULT SetMaxPointFromOrigin(point ptMax) override;

protected:
	BoundingBox::Type m_type;
	vector m_vHalfSize;

	// TODO: this
	quaternion m_qOrientation;

};

#endif // !BOUNDING_BOX_H_
