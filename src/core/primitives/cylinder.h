#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/primitives/Cylinder.h

// Cylinder Primitive
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#include "core/dimension/DimObj.h"

#include "core/primitives/Vertex.h"
#include "core/primitives/point.h"
#include "core/primitives/color.h"

#define MIN_CYLINDER_DIVISIONS 3

class cylinder : public virtual DimObj {
public:
	int m_numAngularDivisions;
	int m_numVerticalDivisions;

public:
	cylinder(double radius = 1.0f, double height = 1.0f, int numAngularDivisions = MIN_CYLINDER_DIVISIONS, int numVerticalDivisions = MIN_CYLINDER_DIVISIONS);

	virtual unsigned int NumberIndices() override;
	virtual unsigned int NumberVertices() override;
	virtual RESULT Allocate() override;

	RESULT SetCylinderVertices(double radius, double height, int numAngularDivisions = MIN_CYLINDER_DIVISIONS, int numVerticalDivisions = MIN_CYLINDER_DIVISIONS);

private:
	float m_radius;
	float m_height;
};

#endif // !VOLUME_H_