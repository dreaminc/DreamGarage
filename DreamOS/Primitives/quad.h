#ifndef QUAD_H_
#define QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/quad.h
// Quadrilateral Primitive (square, rectangle, parallelogram, trapezoid, rhombus, trapezium)
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#define NUM_QUAD_POINTS 4
#define NUM_QUAD_TRIS 2

#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"

// TODO: Move this to cpp make it local
dimindex g_QuadIndices[] = { 0, 1, 2, 1, 3, 2 };

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

	/*
protected:
	vertex m_vertices[NUM_QUAD_POINTS];
	*/

	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(NumberVertices()));
		CR(AllocateIndices(NumberIndices()));

		for (int i = 0; i < NumberIndices(); i++)
			m_pIndices[i] = g_QuadIndices[i];

	Error:
		return R_PASS;
	}

	inline int NumberVertices() { 
		return NUM_QUAD_POINTS;
	}

	inline int NumberIndices() {
		return NUM_QUAD_TRIS * 3;
	}

private:
	QUAD_TYPE m_quadType;

public:
	// Square
	quad(double side) :
		m_quadType(SQUARE)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		double halfSide = side / 2.0f;

		m_pVertices[0] = vertex(point(-halfSide, halfSide, 0.0f));		// A
		m_pVertices[1] = vertex(point(halfSide, halfSide, 0.0f));		// B
		m_pVertices[2] = vertex(point(-halfSide, -halfSide, 0.0f));		// C
		m_pVertices[3] = vertex(point(halfSide, -halfSide, 0.0f));		// D

		Validate();
	Error:
		Invalidate();
	}

	// Rectangle
	quad(double height, double width) :
		m_quadType(RECTANGLE)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		double halfHeight = height / 2.0f;
		double halfWidth = width / 2.0f;

		m_pVertices[0] = vertex(point(-halfWidth, halfHeight, 0.0f));	// A
		m_pVertices[1] = vertex(point(halfWidth, halfHeight, 0.0f));		// B
		m_pVertices[2] = vertex(point(-halfWidth, -halfHeight, 0.0f));	// C
		m_pVertices[3] = vertex(point(halfWidth, -halfHeight, 0.0f));	// D

		Validate();
	Error:
		Invalidate();
	}

	/*
	// Has been moved to DIMOBJ
	RESULT SetColor(color c) {
		for (int i = 0; i < NUM_QUAD_POINTS; i++)
			m_pVertices[i].SetColor(c);

		return R_PASS;
	}
	*/

	// TODO: Parallelogram
	// TODO: Trapezoid
	// TODO: Rhombus
	// TODO: Trapezium + Evaluate Points
	QUAD_TYPE EvaluatePoints(point a, point b, point c) {
		return INVALID;
	}

	/*
	// Moved to DIMOBJ
	RESULT CopyVertices(vertex verts[NUM_QUAD_POINTS]) {
		for (int i = 0; i < NUM_QUAD_POINTS; i++)
			m_pVertices[i].SetVertex(verts[i]);

		return R_PASS;
	}
	*/
};

#endif // ! QUAD_H_
