#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Triangle.h
// Triangle Primitive
// Center is assumed as (0, 0, 0) so points are adjusted positive / negative as needed

#define NUM_VOLUME_FACES 6
#define NUM_FACE_POINTS 4
#define NUM_VOLUME_POINTS (NUM_FACE_POINTS * NUM_VOLUME_FACES)
#define NUM_VOLUME_TRIS (NUM_VOLUME_FACES * 2)
#define NUM_VOLUME_INDICES (NUM_VOLUME_TRIS * 3)

#include "DimObj.h"

#include "Vertex.h"
#include "point.h"
#include "color.h"

// TODO: Move this to cpp make it local
// TODO: Switch to index groups
dimindex g_VolumeIndices[] = { 0, 1, 2, 1, 3, 2 };

class volume : public DimObj {

	//protected:
	//	vertex m_vertices[NUM_TRI_POINTS];

public:
	typedef enum {
		CUBE,
		RECTANGULAR_CUBOID,
		QUADRILATERAL_FRUSTRUM,
		PARALLELEPIPED,
		INVALID
	} VOLUME_TYPE;

	RESULT Allocate() {
		RESULT r = R_PASS;

		CR(AllocateVertices(NUM_VOLUME_POINTS));
		CR(AllocateIndices(NUM_VOLUME_INDICES));

		for (int i = 0; i < NumberIndices(); i++)
			m_pIndices[i] = g_VolumeIndices[i];

	Error:
		return R_PASS;
	}

	inline int NumberVertices() {
		return NUM_VOLUME_POINTS;
	}

	inline int NumberIndices() {
		return NUM_VOLUME_INDICES;
	}

	volume(double side) :
		m_volumeType(CUBE)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		double halfSide = side / 2.0f;
		int vertCount = 0;
		int indexCount = 0;
		int A, B, C, D;

		// Set up indices 
		TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

		// Front face
		m_pVertices[A = vertCount++] = vertex(point(-halfSide, halfSide, halfSide));		// A
		m_pVertices[B = vertCount++] = vertex(point(halfSide, halfSide, halfSide));			// B
		m_pVertices[C = vertCount++] = vertex(point(-halfSide, -halfSide, halfSide));		// C
		m_pVertices[D = vertCount++] = vertex(point(halfSide, -halfSide, halfSide));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, D, C);

		// Back face
		m_pVertices[indexCount++] = vertex(point(-halfSide, halfSide, -halfSide));		// A
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, -halfSide));		// B
		m_pVertices[indexCount++] = vertex(point(-halfSide, -halfSide, -halfSide));		// C
		m_pVertices[indexCount++] = vertex(point(halfSide, -halfSide, -halfSide));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, D, C);

		// Left face
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, halfSide));		// A
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, -halfSide));		// B
		m_pVertices[indexCount++] = vertex(point(halfSide, -halfSide, -halfSide));		// C
		m_pVertices[indexCount++] = vertex(point(halfSide, -halfSide, halfSide));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, D, C);

		// Right face
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, halfSide));		// A
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, -halfSide));		// B
		m_pVertices[indexCount++] = vertex(point(halfSide, -halfSide, -halfSide));		// C
		m_pVertices[indexCount++] = vertex(point(halfSide, -halfSide, halfSide));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, D, C);

		// Top face
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, halfSide));		// A
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, -halfSide));		// B
		m_pVertices[indexCount++] = vertex(point(-halfSide, halfSide, -halfSide));		// C
		m_pVertices[indexCount++] = vertex(point(-halfSide, halfSide, halfSide));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, D, C);

		// Bottom face
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, halfSide));		// A
		m_pVertices[indexCount++] = vertex(point(halfSide, halfSide, -halfSide));		// B
		m_pVertices[indexCount++] = vertex(point(-halfSide, halfSide, -halfSide));		// C
		m_pVertices[indexCount++] = vertex(point(-halfSide, halfSide, halfSide));		// D

		pTriIndices[indexCount++] = TriangleIndexGroup(A, B, C);
		pTriIndices[indexCount++] = TriangleIndexGroup(B, D, C);

		Validate();
	Error:
		Invalidate();
	}



private:
	VOLUME_TYPE m_volumeType;
};

#endif // !TRIANGLE_H_