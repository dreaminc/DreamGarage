#include "volume.h"

volume::volume(double width, double length, double height, bool fTriangleBased) :
	m_volumeType(RECTANGULAR_CUBOID),
	m_fTriangleBased(fTriangleBased)
{
	RESULT r = R_PASS;
	CR(Allocate());

	if (width == length && width == height && length == height) {
		m_volumeType = CUBE;
	}

	CR(SetVolumeVertices(width, length, height, m_fTriangleBased));

	// TODO: Allow for changing this - put it into a factory
	//CR(InitializeAABB());
	CR(InitializeOBB());
	//CR(InitializeBoundingSphere());

	//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

volume::volume(double side, bool fTriangleBased) :
	m_volumeType(CUBE),
	m_fTriangleBased(fTriangleBased)
{
	RESULT r = R_PASS;
	CR(Allocate());

	CR(SetVolumeVertices(side, side, side, m_fTriangleBased));
	
	// TODO: Allow for changing this - put it into a factory
	CR(InitializeAABB());

//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

volume::volume(BoundingBox* pBoundingBox, bool fTriangleBased) :
	m_volumeType(RECTANGULAR_CUBOID),
	m_fTriangleBased(fTriangleBased)
{
	RESULT r = R_PASS;
	CR(Allocate());

	CR(SetVolumeVertices(pBoundingBox, fTriangleBased));

	// TODO: Allow for changing this - put it into a factory
	CR(InitializeAABB());

//Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

RESULT volume::UpdateFromBoundingBox(BoundingBox* pBoundingBox, bool fTriangleBased) {
	RESULT r = R_PASS;

	if (pBoundingBox->GetBoxType() == BoundingBox::Type::AABB) {
		//CR(SetVolumeVertices(&(pBoundingBox->GetBoundingAABB()), fTriangleBased));
		CR(SetVolumeVertices(pBoundingBox, fTriangleBased));

		SetOrigin(pBoundingBox->GetOrigin() - pBoundingBox->GetCenter());
		//SetPivotPoint(pBoundingBox->GetCenter());

		//SetOrientation(pBoundingBox->GetOrientation());
	}
	else if (pBoundingBox->GetBoxType() == BoundingBox::Type::OBB) {
		//m_ptOrigin = RotationMatrix(pBoundingBox->GetOrientation()) * vector(pBoundingBox->GetBoundingVolumeOrigin());
		
		// TODO: There is more to this 
		CR(SetVolumeVertices(pBoundingBox, fTriangleBased));

		SetOrigin(pBoundingBox->GetParentOrigin());
		SetPivotPoint(pBoundingBox->GetParentPivot());
		
		//SetOrigin(pBoundingBox->GetParentOrigin() + pBoundingBox->GetCenter());
		SetOrientation(pBoundingBox->GetOrientation());
	}

Error:
	return r;
}

RESULT volume::SetVolumeVertices(BoundingBox* pBoundingBox, bool fTriangleBased) {
	RESULT r = R_PASS;

	double length = pBoundingBox->GetWidth();
	double height = pBoundingBox->GetHeight();
	double width = pBoundingBox->GetLength();

	if (width == length &&
		width == height &&
		length == height)
		m_volumeType = CUBE;

	CR(SetVolumeVertices(width, length, height, m_fTriangleBased, pBoundingBox->GetCenter()));

Error:
	return r;
}

RESULT volume::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NUM_VOLUME_POINTS));

	if (m_fTriangleBased) {
		CR(AllocateTriangleIndexGroups(NUM_VOLUME_TRIS));
	}
	else {
		CR(AllocateQuadIndexGroups(NUM_VOLUME_QUADS));
	}

Error:
	return R_PASS;
}

unsigned int volume::NumberVertices() {
	return NUM_VOLUME_POINTS;
}

unsigned int volume::NumberIndices() {
	//return NUM_VOLUME_INDICES;
	if (m_fTriangleBased) {
		return NUM_VOLUME_TRI_INDICES;
	}
	else {
		return NUM_VOLUME_QUAD_INDICES;
	}
}

RESULT volume::SetVolumeVertices(double width, double length, double height, bool fTriangleBased, point ptOrigin) {
	RESULT r = R_PASS;

	point_precision halfWidth = static_cast<point_precision>(length / 2.0f);
	point_precision halfLength = static_cast<point_precision>(width / 2.0f);
	point_precision halfHeight = static_cast<point_precision>(height / 2.0f);
	int vertCount = 0;
	int triCount = 0;
	int quadCount = 0;
	int TL, TR, BL, BR;

	// Set up indices 
	TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);
	QuadIndexGroup *pQuadIndices = reinterpret_cast<QuadIndexGroup*>(m_pIndices);

	// OpenGL is default counter clockwise

	///*
	// Front face
	m_pVertices[TR = vertCount++] = vertex(point(halfWidth, halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 1.0f));		// A top left
	m_pVertices[TL = vertCount++] = vertex(point(-halfWidth, halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 1.0f));		// B top right
	m_pVertices[BR = vertCount++] = vertex(point(halfWidth, -halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(1.0f, 0.0f));		// C bottom left
	m_pVertices[BL = vertCount++] = vertex(point(-halfWidth, -halfHeight, halfLength), vector(0.0f, 0.0f, 1.0f), uvcoord(0.0f, 0.0f));		// D bottom right

	if (fTriangleBased) {
		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);
	}
	else {
		pQuadIndices[quadCount++] = QuadIndexGroup(TR, TL, BL, BR);
		SetQuadTangentBitangent(TL, TR, BL, BR);
	}

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

	if (fTriangleBased) {
		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);
	}
	else {
		pQuadIndices[quadCount++] = QuadIndexGroup(TR, TL, BL, BR);
		SetQuadTangentBitangent(TL, TR, BL, BR);
	}

	//SetQuadTangentBitangent(TL, TR, BL, BR);
	//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::iVector(-1.0f));
	//*/

	///*
	// Left face
	m_pVertices[TL = vertCount++] = vertex(point(halfWidth, -halfHeight, halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(0.0f, 1.0f));		// A
	m_pVertices[TR = vertCount++] = vertex(point(halfWidth, halfHeight, halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(1.0f, 1.0f));		// B
	m_pVertices[BL = vertCount++] = vertex(point(halfWidth, -halfHeight, -halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(0.0f, 0.0f));		// C
	m_pVertices[BR = vertCount++] = vertex(point(halfWidth, halfHeight, -halfLength), vector(1.0f, 0.0f, 0.0f), uvcoord(1.0f, 0.0f));		// D

	if (fTriangleBased) {
		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);
	}
	else {
		pQuadIndices[quadCount++] = QuadIndexGroup(TR, TL, BL, BR);
		SetQuadTangentBitangent(TL, TR, BL, BR);
	}

	//SetQuadTangentBitangent(TL, TR, BL, BR);
	//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::kVector(1.0f));
	//*/

	///*
	// Right face
	m_pVertices[TR = vertCount++] = vertex(point(-halfWidth, -halfHeight, halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(1.0f, 1.0f));		// A
	m_pVertices[TL = vertCount++] = vertex(point(-halfWidth, halfHeight, halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(0.0f, 1.0f));		// B
	m_pVertices[BR = vertCount++] = vertex(point(-halfWidth, -halfHeight, -halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(1.0f, 0.0f));		// C
	m_pVertices[BL = vertCount++] = vertex(point(-halfWidth, halfHeight, -halfLength), vector(-1.0f, 0.0f, 0.0f), uvcoord(0.0f, 0.0f));		// D

	if (fTriangleBased) {
		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);
	}
	else {
		pQuadIndices[quadCount++] = QuadIndexGroup(TR, TL, BL, BR);
		SetQuadTangentBitangent(TL, TR, BL, BR);
	}

	//SetQuadTangentBitangent(TL, TR, BL, BR);
	//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::kVector(-1.0f));
	//*/

	///*
	// Top face
	m_pVertices[TR = vertCount++] = vertex(point(-halfWidth, halfHeight, halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(1.0f, 1.0f));		// A
	m_pVertices[TL = vertCount++] = vertex(point(halfWidth, halfHeight, halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(0.0f, 1.0f));			// B
	m_pVertices[BR = vertCount++] = vertex(point(-halfWidth, halfHeight, -halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(1.0f, 0.0f));		// C
	m_pVertices[BL = vertCount++] = vertex(point(halfWidth, halfHeight, -halfLength), vector(0.0f, 1.0f, 0.0f), uvcoord(0.0f, 0.0f));		// D

	if (fTriangleBased) {
		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);
	}
	else {
		pQuadIndices[quadCount++] = QuadIndexGroup(TR, TL, BL, BR);
		SetQuadTangentBitangent(TL, TR, BL, BR);
	}

	//SetQuadTangentBitangent(TL, TR, BL, BR);
	//SetQuadTangentBitangent(TL, TR, BL, BR, vector::jVector(1.0f), vector::iVector(-1.0f));
	//*/

	///*
	// Bottom face
	m_pVertices[TL = vertCount++] = vertex(point(-halfWidth, -halfHeight, halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(0.0f, 1.0f));		// A
	m_pVertices[TR = vertCount++] = vertex(point(halfWidth, -halfHeight, halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(1.0f, 1.0f));		// B
	m_pVertices[BL = vertCount++] = vertex(point(-halfWidth, -halfHeight, -halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(0.0f, 0.0f));		// C
	m_pVertices[BR = vertCount++] = vertex(point(halfWidth, -halfHeight, -halfLength), vector(0.0f, -1.0f, 0.0f), uvcoord(1.0f, 0.0f));		// D

	if (fTriangleBased) {
		pTriIndices[triCount++] = TriangleIndexGroup(BL, BR, TR);
		SetTriangleTangentBitangent(BL, BR, TR);

		pTriIndices[triCount++] = TriangleIndexGroup(TR, TL, BL);
		SetTriangleTangentBitangent(TR, TL, BL);
	}
	else {
		pQuadIndices[quadCount++] = QuadIndexGroup(TR, TL, BL, BR);
		SetQuadTangentBitangent(TL, TR, BL, BR);
	}

	//SetQuadTangentBitangent(TL, TR, BL, BR);
	//*/

	// Adjust for center point
	if (!ptOrigin.IsZero()) {
		for (unsigned int i = 0; i < NumberVertices(); i++) {
			m_pVertices[i].TranslatePoint(ptOrigin);
		}
	}

	//	Error:
	return r;
}

bool volume::IsTriangleBased() {
	return m_fTriangleBased;
}