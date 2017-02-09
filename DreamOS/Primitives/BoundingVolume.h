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

#include <memory>

class VirtualObj;
class BoundingSphere;
class BoundingBox;
class BoundingQuad;

class CollisionManifold;

class BoundingVolume : public dirty {
public:
	enum class Type {
		BOX,
		SPHERE,
		QUAD,
		PLANE,
		INVALID
	};

public:
	BoundingVolume(VirtualObj *pParentObject);
	BoundingVolume(VirtualObj *pParentObject, point ptOrigin);

	bool Intersect(BoundingVolume* pRHS);
	virtual bool Intersect(const BoundingSphere& rhs) = 0;
	virtual bool Intersect(const BoundingBox& rhs) = 0;
	virtual bool Intersect(const BoundingQuad& rhs) = 0;

	virtual bool Intersect(point& pt) = 0;
	virtual bool Intersect(const ray &rCast) = 0;
	//TODO: virtual bool Intersect(line& ln) = 0;
	// TODO: virtual bool Intersect(const plane& pl) = 0;

	CollisionManifold Collide(BoundingVolume* pRHS);
	virtual CollisionManifold Collide(const BoundingSphere& rhs) = 0;
	virtual CollisionManifold Collide(const BoundingBox& rhs) = 0;
	virtual CollisionManifold Collide(const BoundingQuad& rhs) = 0;

	virtual point GetMinPoint() = 0;
	virtual point GetMaxPoint() = 0;

	// This will only apply to certain bounding volumes - otherwise use the above methods 
	virtual point GetMinPointOriented() { return GetMinPoint(); };
	virtual point GetMaxPointOriented() { return GetMaxPoint(); };

	virtual RESULT SetMaxPointFromOrigin(point ptMax) = 0;
	virtual RESULT SetHalfVector(vector vHalfVector) = 0;

	RESULT UpdateBoundingVolume(point ptOrigin, point ptMax);
	RESULT UpdateBoundingVolumeMinMax(point ptMin, point ptMax);

	point GetCenter();
	RESULT SetCenter(point ptOrigin);

	point GetBoundingVolumeOrigin();
	point GetParentOrigin();
	point GetParentPivot();
	VirtualObj *GetParentObject() const;
	point GetOrigin();
	quaternion GetOrientation();

	// These provide the absolute orientation/origin of the object
	point GetAbsoluteOrigin();
	quaternion GetAbsoluteOrientation();

	virtual BoundingVolume::Type GetType() = 0;

protected:
	point m_ptCenter;	// TODO: rename to center point instead of origin?
	VirtualObj *m_pParent;
};

#endif // !BOUNDING_VOLUME_H_
