#ifndef SPHERE_H_
#define SPHERE_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/primitives/sphere.h

// Sphere Primitive
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#include "core/dimension/DimObj.h"
#include "core/bounding/BoundingSphere.h"

#include "core/primitives/PrimParams.h"
#include "core/primitives/Vertex.h"
#include "core/primitives/point.h"
#include "core/primitives/color.h"

#define MIN_SPHERE_DIVISIONS 3

class sphere : public virtual DimObj {

public:
	struct params : 
		public PrimParams 
	{
		virtual PRIMITIVE_TYPE GetPrimitiveType() override { return PRIMITIVE_TYPE::SPHERE; }

		params(float radius, int numAngularDivisions, int numVerticalDivisions) :
			radius(radius),
			numAngularDivisions(numAngularDivisions),
			numVerticalDivisions(numVerticalDivisions)
		{ }

		int numAngularDivisions = MIN_SPHERE_DIVISIONS;
		int numVerticalDivisions = MIN_SPHERE_DIVISIONS;
		float radius = 1.0f;
	};

public:
	sphere(sphere::params *pSphereParams, color c = color(COLOR_WHITE));
	sphere(float radius = 1.0f, int numAngularDivisions = MIN_SPHERE_DIVISIONS, int numVerticalDivisions = MIN_SPHERE_DIVISIONS, color c = color(COLOR_WHITE));
	sphere(BoundingSphere *pBoundingSphere, bool fTriangleBased = true);

	virtual unsigned int NumberIndices() override;
	virtual unsigned int NumberVertices() override;
	virtual RESULT Allocate() override;

	RESULT SetSphereVertices(BoundingSphere* pBoundingSphere, bool fTriangleBased = true);
	//RESULT SetSphereVertices(float radius = 1.0f, int numAngularDivisions = MIN_SPHERE_DIVISIONS, int numVerticalDivisions = MIN_SPHERE_DIVISIONS, color c = color(COLOR_WHITE));
	RESULT SetSphereVertices(float radius = 1.0f, int numAngularDivisions = MIN_SPHERE_DIVISIONS, int numVerticalDivisions = MIN_SPHERE_DIVISIONS, point ptOrigin = point(0.0f, 0.0f, 0.0f), color c = color(COLOR_WHITE));

	RESULT UpdateFromBoundingSphere(BoundingSphere* pBoundingSphere, bool fTriangleBased = true);

protected:
	sphere::params m_params;
};

#endif // !SPHERE_H_