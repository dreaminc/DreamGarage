#ifndef QUAD_H_
#define QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/quad.h
// Quadrilateral Primitive (square, rectangle, parallelogram, trapezoid, rhombus, trapezium)
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#define NUM_QUAD_POINTS 4
#define NUM_QUAD_TRIS 2
#define DEFAULT_HEIGHT_MAP_SCALE 1.0f

#include "DimObj.h"
#include "Vertex.h"
#include "point.h"
#include "color.h"
#include "texture.h"

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

	inline unsigned int NumberVertices() { 
		//return NUM_QUAD_POINTS; 

		unsigned int numVerts = (m_numVerticalDivisions + 1) * (m_numHorizontalDivisions + 1);
		return numVerts;
	}

	inline unsigned int NumberIndices() { 
		//return NUM_QUAD_TRIS * 3; 

		unsigned int numDivisions = m_numVerticalDivisions * m_numHorizontalDivisions;
		unsigned int numTris = numDivisions * 2;
		unsigned int numIndices = numTris * 3;

		return numIndices;
	}

	bool GetBillboard() { return m_isBillboard; }
	void SetBillboard(bool billboard) { m_isBillboard = billboard;  }

private:
	QUAD_TYPE m_quadType;

	int m_numVerticalDivisions;
	int m_numHorizontalDivisions;

	texture *m_pTextureHeight;
	double m_heightMapScale;

	bool m_isBillboard = false;


public:

	// copy ctor
	quad(quad& q) : 
		m_quadType(q.m_quadType),
		m_numHorizontalDivisions(q.m_numHorizontalDivisions),
		m_numVerticalDivisions(q.m_numVerticalDivisions),
		m_pTextureHeight(q.m_pTextureHeight),
		m_heightMapScale(q.m_heightMapScale)
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
		m_numVerticalDivisions(q.m_numVerticalDivisions),
		m_pTextureHeight(q.m_pTextureHeight),
		m_heightMapScale(q.m_heightMapScale)
	{
		m_pVertices = q.m_pVertices;
		m_pIndices = q.m_pIndices;

		q.m_pVertices = nullptr;
		q.m_pIndices = nullptr;
	}

	RESULT SetVertices(float width, float height) {
		RESULT r = R_PASS;

		CR(Allocate());

		float halfHeight = height / 2.0f;
		float halfWidth = width / 2.0f;

		float widthInc = width / m_numHorizontalDivisions;
		float heightInc = height / m_numVerticalDivisions;
		
		int vertCount = 0;
		int indexCount = 0;
		int A, B, C, D;

		// Set up indices 
		TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

		for (int i = 0; i < m_numHorizontalDivisions + 1; i++) {
			for (int j = 0; j < m_numVerticalDivisions + 1; j++) {

				double yValue = 0.0f;
				uv_precision uValue = (float)(i) / (float)(m_numHorizontalDivisions);
				uv_precision vValue = (float)(j) / (float)(m_numVerticalDivisions);

				if (m_pTextureHeight != nullptr) {
					yValue = m_pTextureHeight->GetValueAtUV(uValue, vValue);
					yValue *= m_heightMapScale;
				}

				m_pVertices[vertCount] = vertex(point((widthInc * i) - halfWidth, static_cast<float>(yValue), (heightInc * j) - halfHeight),
												  vector(0.0f, 1.0f, 0.0f), 
												  uvcoord(uValue, vValue));	

				// TODO: Calculate normal (based on geometry)

				m_pVertices[vertCount].SetTangent(vector(-1.0f, 0.0f, 0.0f));
				m_pVertices[vertCount].SetBitangent(vector(0.0f, 0.0f, -1.0f));

				vertCount++;
			}
		}

		for (int i = 0; i < m_numHorizontalDivisions; i++) {
			for (int j = 0; j < m_numVerticalDivisions; j++) {
				A = (i) + ((m_numHorizontalDivisions + 1) * j);
				B = (i + 1) + ((m_numHorizontalDivisions + 1) * j);

				C = (i) + ((m_numHorizontalDivisions + 1) * (j + 1));
				D = (i + 1) + ((m_numHorizontalDivisions + 1) * (j + 1));

				pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
				SetTriangleNormal(A, B, C);

				pTriIndices[indexCount++] = TriangleIndexGroup(C, B, D);
				SetTriangleNormal(C, B, D);
			}
		}

	Error:
		return r;
	}
	
	// Square
	quad(float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr) :
		m_quadType(SQUARE),
		m_numHorizontalDivisions(numHorizontalDivisions),
		m_numVerticalDivisions(numVerticalDivisions),
		m_pTextureHeight(pTextureHeight),
		m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
	{
		RESULT r = R_PASS;

		CR(SetVertices(side, side));

		Validate();
		return;

	Error:
		Invalidate();
		return;
	}

	// Rectangle
	quad(float height, float width, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr) :
		m_quadType(RECTANGLE),
		m_numHorizontalDivisions(numHorizontalDivisions),
		m_numVerticalDivisions(numVerticalDivisions),
		m_pTextureHeight(pTextureHeight),
		m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
	{
		RESULT r = R_PASS;
		
		CR(SetVertices(width, height));

		Validate();
		return;

	Error:
		Invalidate();
		return;
	}

	// This needs to be re-designed, too specific for 2D blits.
	quad(float height, float width, vector& center, uvcoord& uv_bottomleft, uvcoord& uv_upperright) :
		m_quadType(RECTANGLE),
		m_numHorizontalDivisions(1),
		m_numVerticalDivisions(1),
		m_pTextureHeight(nullptr),
		m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
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
