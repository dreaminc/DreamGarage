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

	RESULT SetVolumeVertices(double width, double length, double height) {
		RESULT r = R_PASS;

		point_precision halfWidth = static_cast<point_precision>(width / 2.0f);
		point_precision halfLength = static_cast<point_precision>(length / 2.0f);
		point_precision halfHeight = static_cast<point_precision>(height / 2.0f);
		int vertCount = 0;
		int triCount = 0;
		int TL, TR, BL, BR;

		// Set up indices 
		TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

		// OpenGL is default counter clockwise

		///*
		// Front face
		m_pVertices[TR = vertCount++] = vertex(point(halfWidth, halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 1.0f));		// A top left
		m_pVertices[TL = vertCount++] = vertex(point(-halfWidth, halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 1.0f));		// B top right
		m_pVertices[BR = vertCount++] = vertex(point(halfWidth, -halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 0.0f));		// C bottom left
		m_pVertices[BL = vertCount++] = vertex(point(-halfWidth, -halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 0.0f));		// D bottom right

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);

		//SetTriangleTangentBitangent(BL, BR, TR);
		//SetQuadTangentBitangent(TL, TR, BL, BR);
		//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::iVector(1.0f));

		//*/

		///*
		// Back face
		m_pVertices[TL = vertCount++] = vertex(point(halfWidth, halfHeight, -halfLength), vector(0.0f, 0.0f, -1.0f), uvcoord(0.0f, 1.0f));		// A
		m_pVertices[TR = vertCount++] = vertex(point(-halfWidth, halfHeight, -halfLength), vector(0.0f, 0.0f, -1.0f), uvcoord(1.0f, 1.0f));		// B
		m_pVertices[BL = vertCount++] = vertex(point(halfWidth, -halfHeight, -halfLength), vector(0.0f, 0.0f, -1.0f), uvcoord(0.0f, 0.0f));		// C
		m_pVertices[BR = vertCount++] = vertex(point(-halfWidth, -halfHeight, -halfLength), vector(0.0f, 0.0f, -1.0f), uvcoord(1.0f, 0.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);

		//SetQuadTangentBitangent(TL, TR, BL, BR);
		//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::iVector(-1.0f));
		//*/

		///*
		// Left face
		m_pVertices[TL = vertCount++] = vertex(point(halfWidth, -halfHeight, halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(0.0f, 1.0f));		// A
		m_pVertices[TR = vertCount++] = vertex(point(halfWidth, halfHeight, halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(1.0f, 1.0f));		// B
		m_pVertices[BL = vertCount++] = vertex(point(halfWidth, -halfHeight, -halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(0.0f, 0.0f));		// C
		m_pVertices[BR = vertCount++] = vertex(point(halfWidth, halfHeight, -halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(1.0f, 0.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);

		//SetQuadTangentBitangent(TL, TR, BL, BR);
		//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::kVector(1.0f));
		//*/

		///*
		// Right face
		m_pVertices[TR = vertCount++] = vertex(point(-halfWidth, -halfHeight, halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(1.0f, 1.0f));		// A
		m_pVertices[TL = vertCount++] = vertex(point(-halfWidth, halfHeight, halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(0.0f, 1.0f));		// B
		m_pVertices[BR = vertCount++] = vertex(point(-halfWidth, -halfHeight, -halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(1.0f, 0.0f));		// C
		m_pVertices[BL = vertCount++] = vertex(point(-halfWidth, halfHeight, -halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(0.0f, 0.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);

		//SetQuadTangentBitangent(TL, TR, BL, BR);
		//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::kVector(-1.0f));
		//*/

		///*
		// Top face
		m_pVertices[TR = vertCount++] = vertex(point(-halfWidth, halfHeight, halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(1.0f, 1.0f));		// A
		m_pVertices[TL = vertCount++] = vertex(point(halfWidth, halfHeight, halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(0.0f, 1.0f));			// B
		m_pVertices[BR = vertCount++] = vertex(point(-halfWidth, halfHeight, -halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(1.0f, 0.0f));		// C
		m_pVertices[BL = vertCount++] = vertex(point(halfWidth, halfHeight, -halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(0.0f, 0.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);

		//SetQuadTangentBitangent(TL, TR, BL, BR);
		//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::iVector(-1.0f));
		//*/

		///*
		// Bottom face
		m_pVertices[TL = vertCount++] = vertex(point(-halfWidth, -halfHeight, halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(0.0f, 1.0f));		// A
		m_pVertices[TR = vertCount++] = vertex(point(halfWidth, -halfHeight, halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(1.0f, 1.0f));		// B
		m_pVertices[BL = vertCount++] = vertex(point(-halfWidth, -halfHeight, -halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(0.0f, 0.0f));		// C
		m_pVertices[BR = vertCount++] = vertex(point(halfWidth, -halfHeight, -halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(1.0f, 0.0f));		// D

		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);

		//SetQuadTangentBitangent(TL, TR, BL, BR);
		//*/

	Error:
		return r;
	}

	volume(double width, double length, double height) :
		m_volumeType(RECTANGULAR_CUBOID)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		if (width == length == height)
			m_volumeType = CUBE;

		CR(SetVolumeVertices(width, length, height));

		Validate();
		return;
	Error:
		Invalidate();
		return;
	}

	volume(double side) :
		m_volumeType(CUBE)
	{
		RESULT r = R_PASS;
		CR(Allocate());

		CR(SetVolumeVertices(side, side, side));

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