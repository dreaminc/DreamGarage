#ifndef BOUNDING_VOLUME_H_
#define BOUNDING_VOLUME_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingVolume.h
// Bounding Volume Generic Class - this can be sub-classed by specific versions of volumes like Boxes, Spheres or otherwise
// TODO: bounding volume suggests volume as in volume in the engine, whcih is a rectilinear volume - but this is more general

#include "point.h"

class BoundingSphere;
class BoundingBox;

class BoundingVolume {
public:
	BoundingVolume() :
		m_ptOrigin()
	{
		// Empty
	}

	BoundingVolume(point ptOrigin) :
		m_ptOrigin(ptOrigin)
	{
		// Empty
	}

	bool Intersect(BoundingVolume* pRHS);

	virtual bool Intersect(const BoundingSphere& rhs) = 0;
	virtual bool Intersect(const BoundingBox& rhs) = 0;

	//bool Intersect(const point& pt) {
	virtual bool Intersect(point& pt) = 0;
	//TODO: virtual bool Intersect(const line& ln) = 0;

	virtual RESULT SetMaxPointFromOrigin(point ptMax) = 0;

	RESULT UpdateBoundingVolume(point ptOrigin, point ptMax) {
		RESULT r = R_PASS;

		CR(SetOrigin(ptOrigin));
		CR(SetMaxPointFromOrigin(ptMax));

	Error:
		return r;
	}

	RESULT SetOrigin(point ptOrigin) {
		m_ptOrigin = ptOrigin;
		return R_PASS;
	}

	point GetOrigin() { return m_ptOrigin; }

protected:
	point m_ptOrigin;
};

#endif // !BOUNDING_VOLUME_H_
