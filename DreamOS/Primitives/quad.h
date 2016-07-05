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

	inline int NumberVertices() { 
		//return NUM_QUAD_POINTS; 

		int numVerts = (m_numVerticalDivisions + 1) * (m_numHorizontalDivisions + 1);
		return numVerts;
	}

	inline int NumberIndices() { 
		//return NUM_QUAD_TRIS * 3; 

		int numDivisions = m_numVerticalDivisions * m_numHorizontalDivisions;
		int numTris = numDivisions * 2;
		int numIndices = numTris * 3;

		return numIndices;
	}

private:
	QUAD_TYPE m_quadType;

	int m_numVerticalDivisions;
	int m_numHorizontalDivisions;


public:

	// copy ctor
	quad(quad& q) : 
		m_quadType(q.m_quadType),
		m_numHorizontalDivisions(q.m_numHorizontalDivisions),
		m_numVerticalDivisions(q.m_numVerticalDivisions)
	{
		m_pVertices = q.m_pVertices;
		m_pIndices = q.m_pIndices;

		q.m_pVertices = nullptr;
		q.m_pIndices = nullptr;
	}

	// move ctor
	quad(quad&& q) : 
		m_quadType(q.m_quadType),
		m_numHorizontalDivisions(q.m_numHorizontalDivisions),
		m_numVerticalDivisions(q.m_numVerticalDivisions)
	{
		m_pVertices = q.m_pVertices;
		m_pIndices = q.m_pIndices;

		q.m_pVertices = nullptr;
		q.m_pIndices = nullptr;
	}

	RESULT SetVerticies(double width, double height) {
		RESULT r = R_PASS;

		CR(Allocate());

		double halfHeight = height / 2.0f;
		double halfWidth = width / 2.0f;
		int vertCount = 0;
		int indexCount = 0;
		int A, B, C, D;

		// Set up indices 
		TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

		m_pVertices[A = vertCount++] = vertex(point(halfWidth, halfHeight, 0.0f), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 1.0f));		// A
		m_pVertices[B = vertCount++] = vertex(point(-halfWidth, halfHeight, 0.0f), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 1.0f));			// B
		m_pVertices[C = vertCount++] = vertex(point(halfWidth, -halfHeight, 0.0f), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 0.0f));		// C
		m_pVertices[D = vertCount++] = vertex(point(-halfWidth, -halfHeight, 0.0f), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 0.0f));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
		SetTriangleTangentBitangent(A, B, C);

		pTriIndices[indexCount++] = TriangleIndexGroup(C, B, D);
		SetTriangleTangentBitangent(C, B, D);

	Error:
		return r;
	}
	
	// Square
	quad(double side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1) :
		m_quadType(SQUARE),
		m_numHorizontalDivisions(numHorizontalDivisions),
		m_numVerticalDivisions(numVerticalDivisions)
	{
		RESULT r = R_PASS;

		CR(SetVerticies(side, side));

		Validate();
		return;

	Error:
		Invalidate();
		return;
	}

	// Rectangle
	quad(double height, double width, int numHorizontalDivisions = 1, int numVerticalDivisions = 1) :
		m_quadType(RECTANGLE),
		m_numHorizontalDivisions(numHorizontalDivisions),
		m_numVerticalDivisions(numVerticalDivisions)
	{
		RESULT r = R_PASS;
		
		CR(SetVerticies(width, height));

		Validate();
		return;

	Error:
		Invalidate();
		return;
	}

	// This needs to be re-designed, too specific for 2D blits.
	quad(double height, double width, vector& center, uvcoord& uv_bottomleft, uvcoord& uv_upperright) :
		m_quadType(RECTANGLE),
		m_numHorizontalDivisions(1),
		m_numVerticalDivisions(1)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		double halfSideX = width / 2.0f;
		double halfSideY = height / 2.0f;
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
