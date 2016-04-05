#ifndef VOLUME_H_
#define VOLUME_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/Volume.h
// Volume Primitive
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
		CR(AllocateTriangleIndexGroups(NUM_VOLUME_TRIS));

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

		point_precision halfSide = static_cast<point_precision>(side / 2.0f);
		int vertCount = 0;
		int triCount = 0;
		int TL, TR, BL, BR;

		// Set up indices 
		TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

		// OpenGL is default counter clockwise

		///*
		// Front face
		m_pVertices[TR = vertCount++] = vertex(point(halfSide, halfSide, halfSide), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 0.0f));		// A top left
		m_pVertices[TL = vertCount++] = vertex(point(-halfSide, halfSide, halfSide), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 0.0f));		// B top right
		m_pVertices[BR = vertCount++] = vertex(point(halfSide, -halfSide, halfSide), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 1.0f));		// C bottom left
		m_pVertices[BL = vertCount++] = vertex(point(-halfSide, -halfSide, halfSide), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 1.0f));		// D bottom right

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		//pTriIndices[triCount++] = TriangleIndexGroup(BL, TR, TL);
		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);

		//SetTriangleTangentBitangent(BL, BR, TR);
		SetQuadTangentBitangent(TL, TR, BL, BR);


		//*/

		///*
		// Back face
		m_pVertices[TL = vertCount++] = vertex(point(halfSide, halfSide, -halfSide), vector(0.0f, 0.0f, -1.0f), uvcoord(0.0f, 0.0f));		// A
		m_pVertices[TR = vertCount++] = vertex(point(-halfSide, halfSide, -halfSide), vector(0.0f, 0.0f, -1.0f), uvcoord(1.0f, 0.0f));		// B
		m_pVertices[BL = vertCount++] = vertex(point(halfSide, -halfSide, -halfSide), vector(0.0f, 0.0f, -1.0f), uvcoord(0.0f, 1.0f));		// C
		m_pVertices[BR = vertCount++] = vertex(point(-halfSide, -halfSide, -halfSide), vector(0.0f, 0.0f, -1.0f), uvcoord(1.0f, 1.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		//pTriIndices[triCount++] = TriangleIndexGroup(BL, TR, TL);
		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);

		SetQuadTangentBitangent(TL, TR, BL, BR);
		//*/
		
		///*
		// Left face
		m_pVertices[TL = vertCount++] = vertex(point(halfSide, -halfSide, halfSide), vector(1.0f, 0.0f, 0.0f), uvcoord(0.0f, 0.0f));		// A
		m_pVertices[TR = vertCount++] = vertex(point(halfSide, halfSide, halfSide), vector(1.0f, 0.0f, 0.0f), uvcoord(1.0f, 0.0f));		// B
		m_pVertices[BL = vertCount++] = vertex(point(halfSide, -halfSide, -halfSide), vector(1.0f, 0.0f, 0.0f), uvcoord(0.0f, 1.0f));		// C
		m_pVertices[BR = vertCount++] = vertex(point(halfSide, halfSide, -halfSide), vector(1.0f, 0.0f, 0.0f), uvcoord(1.0f, 1.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		//pTriIndices[triCount++] = TriangleIndexGroup(BL, TR, TL);
		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);

		SetQuadTangentBitangent(TL, TR, BL, BR);
		//*/

		///*
		// Right face
		m_pVertices[TR = vertCount++] = vertex(point(-halfSide, -halfSide, halfSide), vector(-1.0f, 0.0f, 0.0f), uvcoord(0.0f, 0.0f));		// A
		m_pVertices[TL = vertCount++] = vertex(point(-halfSide, halfSide, halfSide), vector(-1.0f, 0.0f, 0.0f), uvcoord(1.0f, 0.0f));		// B
		m_pVertices[BR = vertCount++] = vertex(point(-halfSide, -halfSide, -halfSide), vector(-1.0f, 0.0f, 0.0f), uvcoord(0.0f, 1.0f));		// C
		m_pVertices[BL = vertCount++] = vertex(point(-halfSide, halfSide, -halfSide), vector(-1.0f, 0.0f, 0.0f), uvcoord(1.0f, 1.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		//pTriIndices[triCount++] = TriangleIndexGroup(BL, TR, TL);
		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);

		SetQuadTangentBitangent(TL, TR, BL, BR);
		//*/

		///*
		// Top face
		m_pVertices[TR = vertCount++] = vertex(point(-halfSide, halfSide, halfSide), vector(0.0f, 1.0f, 0.0f), uvcoord(0.0f, 0.0f));		// A
		m_pVertices[TL = vertCount++] = vertex(point(halfSide, halfSide, halfSide), vector(0.0f, 1.0f, 0.0f), uvcoord(1.0f, 0.0f));			// B
		m_pVertices[BR = vertCount++] = vertex(point(-halfSide, halfSide, -halfSide), vector(0.0f, 1.0f, 0.0f), uvcoord(0.0f, 1.0f));		// C
		m_pVertices[BL = vertCount++] = vertex(point(halfSide, halfSide, -halfSide), vector(0.0f, 1.0f, 0.0f), uvcoord(1.0f, 1.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		//pTriIndices[triCount++] = TriangleIndexGroup(BL, TR, TL);
		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);

		SetQuadTangentBitangent(TL, TR, BL, BR);
		//*/

		///*
		// Bottom face
		m_pVertices[TL = vertCount++] = vertex(point(-halfSide, -halfSide, halfSide), vector(0.0f, -1.0f, 0.0f), uvcoord(0.0f, 0.0f));		// A
		m_pVertices[TR = vertCount++] = vertex(point(halfSide, -halfSide, halfSide), vector(0.0f, -1.0f, 0.0f), uvcoord(1.0f, 0.0f));		// B
		m_pVertices[BL = vertCount++] = vertex(point(-halfSide, -halfSide, -halfSide), vector(0.0f, -1.0f, 0.0f), uvcoord(0.0f, 1.0f));		// C
		m_pVertices[BR = vertCount++] = vertex(point(halfSide, -halfSide, -halfSide), vector(0.0f, -1.0f, 0.0f), uvcoord(1.0f, 1.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		//pTriIndices[triCount++] = TriangleIndexGroup(BL, TR, TL);
		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);

		SetQuadTangentBitangent(TL, TR, BL, BR);
		//*/

		Validate();
		return;
	Error:
		Invalidate();
		return;
	}

private:
	VOLUME_TYPE m_volumeType;
};

#endif // !VOLUME_H_