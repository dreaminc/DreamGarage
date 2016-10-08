#include "cylinder.h"

RESULT cylinder::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));

Error:
	return r;
}

unsigned int cylinder::NumberIndices() {
	int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
	int numStrips = m_numVerticalDivisions;
	return (numTriangleStripVerts * numStrips);
}

unsigned int cylinder::NumberVertices() {
	int numVertsPerStrip = m_numAngularDivisions + 1;
	int numStrips = m_numVerticalDivisions;
	return (numStrips) * (numVertsPerStrip);
}

cylinder::cylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) :
	m_radius(radius),
	m_height(height),
	m_numAngularDivisions(numAngularDivisions),
	m_numVerticalDivisions(numVerticalDivisions)
{
	RESULT r = R_PASS;

	CR(SetCylinderVertices(radius, height, numAngularDivisions, numVerticalDivisions));

// Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT cylinder::SetCylinderVertices(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	RESULT r = R_PASS;

	if (m_numAngularDivisions < MIN_CYLINDER_DIVISIONS) m_numAngularDivisions = MIN_CYLINDER_DIVISIONS;
	if (m_numVerticalDivisions < MIN_CYLINDER_DIVISIONS) m_numVerticalDivisions = MIN_CYLINDER_DIVISIONS;



Error:
	return r;
}