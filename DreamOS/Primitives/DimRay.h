#ifndef DIM_RAY_H_
#define DIM_RAY_H_

#include "DimObj.h"
#include "ray.h"

#include "point.h"
#include "vector.h"

class volume;
class CollisionManifold;

//class DimRay : public ray, public DimObj {
class DimRay : public ray, public DimObj {
public:
	DimRay(point ptOrigin, vector vDirection);
	DimRay(point ptOrigin, vector vDirection, float rayVisualLength);

	virtual unsigned int NumberIndices() override;
	virtual unsigned int NumberVertices() override;
	virtual RESULT Allocate() override;

	RESULT SetRayVertices(float step = 1.0f);

	bool Intersect(VirtualObj* pObj);
	CollisionManifold Collide(VirtualObj* pObj);

	virtual ray GetRay(bool fAbsolute = false) override;
	RESULT UpdateFromRay(const ray &rCast);
	ray GetRayFromVerts();

	//TODO: Nested object does not fit with usual pattern of primitives
	// move to native arrow tip implementation
	//std::shared_ptr<volume> m_rayTip;
};

#endif // ! DIM_RAY_H_