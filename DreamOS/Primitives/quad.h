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
		//CR(AllocateTriangleIndexGroups(NUM_QUAD_TRIS));

	Error:
		return R_PASS;
	}

	inline dimindex NumberVertices() { return NUM_QUAD_POINTS; }
	inline dimindex NumberIndices() { return NUM_QUAD_TRIS * 3; }

private:
	QUAD_TYPE m_quadType;

public:

	// copy ctor
	quad(quad& q) : m_quadType(q.m_quadType) {
		m_pVertices = q.m_pVertices;
		m_pIndices = q.m_pIndices;

		q.m_pVertices = nullptr;
		q.m_pIndices = nullptr;
	}

	// move ctor
	quad(quad&& q) : m_quadType(q.m_quadType) {
		m_pVertices = q.m_pVertices;
		m_pIndices = q.m_pIndices;

		q.m_pVertices = nullptr;
		q.m_pIndices = nullptr;
	}
	
	// Square
	quad(float side) :
		m_quadType(SQUARE)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		float halfSide = side / 2.0f;
		int vertCount = 0;
		int indexCount = 0;
		int A, B, C, D;

		// Set up indices 
		TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

		m_pVertices[A = vertCount++] = vertex(point(-halfSide, halfSide, 0.0f), vector(0, 0, 1), uvcoord(0, 1.0f));		// A
		m_pVertices[B = vertCount++] = vertex(point(halfSide, halfSide, 0.0f), vector(0, 0, 1), uvcoord(1.0f, 1.0f));			// B
		m_pVertices[C = vertCount++] = vertex(point(-halfSide, -halfSide, 0.0f), vector(0, 0, 1), uvcoord(0, 0));		// C
		m_pVertices[D = vertCount++] = vertex(point(halfSide, -halfSide, 0.0f), vector(0, 0, 1), uvcoord(1.0f, 0));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, C, B);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, C, D);

		Validate();
	Error:
		Invalidate();
	}

	// Rectangle
	quad(float height, float width) :
		m_quadType(RECTANGLE)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		float halfHeight = height / 2.0f;
		float halfWidth = width / 2.0f;

		m_pVertices[0] = vertex(point(-halfWidth, halfHeight, 0.0f));	// A
		m_pVertices[1] = vertex(point(halfWidth, halfHeight, 0.0f));		// B
		m_pVertices[2] = vertex(point(-halfWidth, -halfHeight, 0.0f));	// C
		m_pVertices[3] = vertex(point(halfWidth, -halfHeight, 0.0f));	// D

		Validate();
	Error:
		Invalidate();
	}

	// This needs to be re-designed, too specific for 2D blits.
	quad(float height, float width, vector& center, uvcoord& uv_bottomleft, uvcoord& uv_upperright) :
		m_quadType(RECTANGLE)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		float halfSideX = width / 2.0f;
		float halfSideY = height / 2.0f;
		int vertCount = 0;
		int indexCount = 0;
		int A, B, C, D;

		// Set up indices 
		TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);
	
		m_pVertices[A = vertCount++] = vertex(point(-halfSideX + center.x(), halfSideY + center.y(), center.z()), vector(0, 0, 1), uvcoord(uv_bottomleft.u(), uv_upperright.v()));		// A
		m_pVertices[B = vertCount++] = vertex(point(halfSideX + center.x(), halfSideY + center.y(), center.z()), vector(0, 0, 1), uv_upperright);			// B
		m_pVertices[C = vertCount++] = vertex(point(-halfSideX + center.x(), -halfSideY + center.y(), center.z()), vector(0, 0, 1), uv_bottomleft);		// C
		m_pVertices[D = vertCount++] = vertex(point(halfSideX + center.x(), -halfSideY + center.y(), center.z()), vector(0, 0, 1), uvcoord(uv_upperright.u(), uv_bottomleft.v()));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, C, B);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, C, D);

		Validate();
	Error:
		Invalidate();
	}

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
