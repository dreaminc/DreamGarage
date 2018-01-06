#ifndef DIM_PLANE_H_
#define DIM_PLANE_H_

#include "DimObj.h"
#include "plane.h"

#include "point.h"
#include "vector.h"

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