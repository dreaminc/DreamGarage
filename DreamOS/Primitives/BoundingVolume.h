#ifndef BOUNDING_VOLUME_H_
#define BOUNDING_VOLUME_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingVolume.h
// Bounding Volume Generic Class - this can be sub-classed by specific versions of volumes like Boxes, Spheres or otherwise
// TODO: bounding volume suggests volume as in volume in the engine, which is a rectilinear volume - but this is more general

#include "point.h"
#include "quaternion.h"
#include "ray.h"
#include "dirty.h"

class VirtualObj;
class BoundingSphere;
class BoundingBox;

class BoundingVolume : public dirty {
public:
	enum class Type {
		BOX,
		SPHERE,
		INVALID
	};

public:
	BoundingVolume(VirtualObj *pParentObject) :
		m_pParent(pParentObject),
		m_ptOrigin()
	{
		// Empty
	}

	BoundingVolume(VirtualObj *pParentObject, point ptOrigin) :
		m_pParent(pParentObject),
		m_ptOrigin(ptOrigin)
	{
		// Empty
	}

	bool Intersect(BoundingVolume* pRHS);
	virtual bool Intersect(const BoundingSphere& rhs) = 0;
	virtual bool Intersect(const BoundingBox& rhs) = 0;

	//bool Intersect(const point& pt) {
	virtual bool Intersect(point& pt) = 0;
	virtual bool Intersect(const ray &rCast) = 0;
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

	point GetBoundingVolumeOrigin();
	point GetParentOrigin();
	point GetOrigin();
	quaternion GetOrientation();

	virtual BoundingVolume::Type GetType() = 0;

protected:
	point m_ptOrigin;	// TODO: rename to center point instead of origin?
	VirtualObj *m_pParent;
};

#endif // !BOUNDING_VOLUME_H_
