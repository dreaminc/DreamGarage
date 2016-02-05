#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Triangle.h
// Triangle Primitive
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#include "DimObj.h"
#include "Vertex.h"
#include "point.h"

#define NUM_TRI_POINTS 3

class triangle : public DimObj {
private:
	vertex m_vertices[NUM_TRI_POINTS];

public:
	typedef enum {
		EQUILATERAL,
		ISOCELES, 
		SCALANE,
		INVALID
	} TRIANGLE_TYPE;

	triangle(double side) :
		m_triangleType(EQUILATERAL)
	{
		double halfSide = side / 2.0f;
		double halfHeight = (halfSide*sqrt(3.0f)) / 2.0f;

		m_vertices[0] = vertex(point(0.0f, halfHeight, 0.0f));			// A
		m_vertices[1] = vertex(point(-halfSide, -halfHeight, 0.0f));	// B
		m_vertices[2] = vertex(point(halfSide, -halfHeight, 0.0f));		// C
	}

	triangle(double height, double width) :
		m_triangleType(ISOCELES)
	{
		double halfHeight = height / 2.0f;
		double halfWidth = width / 2.0f;

		m_vertices[0] = vertex(point(0.0f, halfHeight, 0.0f));			// A
		m_vertices[1] = vertex(point(-halfWidth, -halfHeight, 0.0f));	// B
		m_vertices[2] = vertex(point(halfWidth, -halfHeight, 0.0f));	// C
	}

	// TODO: Scalene arbitrary triangle 
	/*
	triangle(point a, point b, point c) :
		m_triangleType(INVALID)			// Determined by points
	{
		m_triangleType = EvaluatePoints(a, b, c);
	}
	*/

	// equ: AB == BC == AC
	// iso: AB == BC != AC || AB == AC != BC | AB != BC == AC 
	TRIANGLE_TYPE EvaluatePoints(point a, point b, point c) {
		int count = 0;

		double AB = (b - a).magnitude();
		double BC = (c - b).magnitude();
		double AC = (c - a).magnitude();

		if (AB == BC) count++;
		if (AB == AB) count++;
		if (AC == AB) count++;

		if (count == 3)
			return EQUILATERAL;
		else if (count == 2)
			return ISOCELES;
		else
			return SCALANE;
	}

private:
	TRIANGLE_TYPE m_triangleType;
};

#endif // !TRIANGLE_H_