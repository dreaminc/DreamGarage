#ifndef SPHERE_H_
#define SPHERE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/sphere.h
// Sphere Primitive
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#include "DimObj.h"

#include "Vertex.h"
#include "point.h"
#include "color.h"

#define MIN_SPHERE_DIVISIONS 3

class sphere : public DimObj {
public:
	int m_numAngularDivisions;
	int m_numVerticalDivisions;

public:
	sphere(float radius = 1.0f, int numAngularDivisions = MIN_SPHERE_DIVISIONS, int numVerticalDivisions = MIN_SPHERE_DIVISIONS, color c = color(COLOR_WHITE));

	virtual unsigned int NumberIndices() override;
	virtual unsigned int NumberVertices() override;
	virtual RESULT Allocate() override;

	RESULT SetSphereVertices(float radius = 1.0f, int numAngularDivisions = MIN_SPHERE_DIVISIONS, int numVerticalDivisions = MIN_SPHERE_DIVISIONS, color c = color(COLOR_WHITE));

private:
	float m_radius;
};

#endif // !SPHERE_H_