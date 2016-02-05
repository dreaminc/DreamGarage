#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Triangle.h
// Triangle Primitive

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

	}

	triangle(double height, double width) :
		m_triangleType(ISOCELES)
	{

	}

	triangle(point a, point b, point c) :
		m_triangleType(INVALID)			// Determined by points
	{
		m_triangleType = EvaluatePoints(a, b, c);
	}

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