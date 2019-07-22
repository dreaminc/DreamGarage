#ifndef DIM_PLANE_H_
#define DIM_PLANE_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/dimension/DimPlane.h

// Dimension Plane

#include "DimObj.h"

#include "core/primitives/plane.h"
#include "core/primitives/point.h"
#include "core/primitives/vector.h"

class CollisionManifold;

//class DimRay : public ray, public DimObj {
class DimPlane : public plane, public virtual DimObj {
public:
	DimPlane(point ptOrigin, vector vNormal);

	virtual unsigned int NumberIndices() override;
	virtual unsigned int NumberVertices() override;

	virtual RESULT Allocate() override;

	RESULT SetPlaneVertices(float scale = 100.0f);

	//bool Intersect(VirtualObj* pObj);
	//CollisionManifold Collide(VirtualObj* pObj);
	
	RESULT UpdateFromPlane(const plane &planeSource);
};

#endif // ! DIM_PLANE_H_