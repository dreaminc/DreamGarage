#ifndef QUAD_H_
#define QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/quad.h
// Quadrilateral Primitive (square, rectangle, parallelogram, trapezoid, rhombus, trapezium)
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#define NUM_QUAD_POINTS 4

#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"

class quad : public DimObj {
public:
	typedef enum {
		SQUARE,
		RECTANGLE,
		PARALLELOGRAM,
		TRAPEZOID,
		RHOMBUS,
		TRAPEZIUM,
		INVALID
	} QUAD_TYPE;

protected:
	vertex m_vertices[NUM_QUAD_POINTS];

private:
	QUAD_TYPE m_quadType;

public:
	// Square
	quad(double side) :
		m_quadType(SQUARE)
	{
		double halfSide = side / 2.0f;

		m_vertices[0] = vertex(point(-halfSide, halfSide, 0.0f));		// A
		m_vertices[1] = vertex(point(halfSide, halfSide, 0.0f));		// B
		m_vertices[2] = vertex(point(-halfSide, -halfSide, 0.0f));		// C
		m_vertices[3] = vertex(point(halfSide, -halfSide, 0.0f));		// D
	}

	// Rectangle
	quad(double height, double width) :
		m_quadType(RECTANGLE)
	{
		double halfHeight = height / 2.0f;
		double halfWidth = width / 2.0f;

		m_vertices[0] = vertex(point(-halfWidth, halfHeight, 0.0f));	// A
		m_vertices[1] = vertex(point(halfWidth, halfHeight, 0.0f));		// B
		m_vertices[2] = vertex(point(-halfWidth, -halfHeight, 0.0f));	// C
		m_vertices[3] = vertex(point(halfWidth, -halfHeight, 0.0f));	// D
	}

	RESULT SetColor(color c) {
		for (int i = 0; i < NUM_QUAD_POINTS; i++)
			m_vertices[i].SetColor(c);

		return R_PASS;
	}

	// TODO: Parallelogram
	// TODO: Trapezoid
	// TODO: Rhombus
	// TODO: Trapezium + Evaluate Points
	QUAD_TYPE EvaluatePoints(point a, point b, point c) {
		return INVALID;
	}

	// TODO: Move to DimObj?
	RESULT CopyVertices(vertex verts[NUM_QUAD_POINTS]) {
		for (int i = 0; i < NUM_QUAD_POINTS; i++)
			m_vertices[i].SetVertex(verts[i]);

		return R_PASS;
	}
};

#endif // ! QUAD_H_
