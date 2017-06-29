#include "quad.h"
#include "BoundingQuad.h"

// copy ctor
quad::quad(quad& q) :
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
quad::quad(quad&& q) :
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

// Square
quad::quad(float side, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	m_quadType(SQUARE),
	m_numHorizontalDivisions(numHorizontalDivisions),
	m_numVerticalDivisions(numVerticalDivisions),
	m_pTextureHeight(pTextureHeight),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;

	CR(SetVertices(side, side, vNormal));
	
	//CR(InitializeOBB());
	//CR(InitializeBoundingSphere());

	CR(InitializeBoundingQuad(GetOrigin(), side, side, vNormal));
	//TODO: CR(InitializeBoundingPlane());

	// Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

// Rectangle
quad::quad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) :
	m_quadType(RECTANGLE),
	m_numHorizontalDivisions(numHorizontalDivisions),
	m_numVerticalDivisions(numVerticalDivisions),
	m_pTextureHeight(pTextureHeight),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;

	CR(SetVertices(width, height, vNormal));

	//CR(InitializeAABB());
	//CR(InitializeOBB());
	//CR(InitializeBoundingSphere());
	
	CR(InitializeBoundingQuad(GetOrigin(), width, height, vNormal));
	//TODO: CR(InitializeBoundingPlane());

// Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

// This needs to be re-designed, too specific for 2D blits.
//quad::quad(float height, float width, point& ptCenter, uvcoord& uvBottomLeft, uvcoord& uvUpperRight, vector vNormal) :
quad::quad(float width, float height, point& ptCenter, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight, vector vNormal) :
	m_quadType(RECTANGLE),
	m_numHorizontalDivisions(1),
	m_numVerticalDivisions(1),
	m_pTextureHeight(nullptr),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;
	
	// TODO: UV thing
	CR(SetVertices(width, height, vNormal, uvTopLeft, uvBottomRight));

	/*
	CR(Allocate());

	float halfSideX = width / 2.0f;
	float halfSideY = height / 2.0f;
	int vertCount = 0;
	int indexCount = 0;
	int A, B, C, D;

	// Set up indices 
	// TODO: ASDFAGKJHNSDFGKJSDFG
	TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

	m_pVertices[A = vertCount++] = vertex(point(-halfSideX + ptCenter.x(), halfSideY + ptCenter.y(), ptCenter.z()), vector(0, 0, 1), uvcoord(uvBottomLeft.u(), uvTopRight.v()));		// A
	m_pVertices[B = vertCount++] = vertex(point(halfSideX + ptCenter.x(), halfSideY + ptCenter.y(), ptCenter.z()), vector(0, 0, 1), uvTopRight);			// B
	m_pVertices[C = vertCount++] = vertex(point(-halfSideX + ptCenter.x(), -halfSideY + ptCenter.y(), ptCenter.z()), vector(0, 0, 1), uvBottomLeft);		// C
	m_pVertices[D = vertCount++] = vertex(point(halfSideX + ptCenter.x(), -halfSideY + ptCenter.y(), ptCenter.z()), vector(0, 0, 1), uvcoord(uvTopRight.u(), uvBottomLeft.v()));		// D

	pTriIndices[indexCount++] = TriangleIndexGroup(A, C, B);
	pTriIndices[indexCount++] = TriangleIndexGroup(B, C, D);
	*/

//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

quad::quad(BoundingQuad *pBoundingQuad, bool fTriangleBased) :
	m_quadType(RECTANGLE),
	m_numHorizontalDivisions(1),
	m_numVerticalDivisions(1),
	m_pTextureHeight(nullptr),
	m_heightMapScale(DEFAULT_HEIGHT_MAP_SCALE)
{
	RESULT r = R_PASS;

	CR(SetVertices(pBoundingQuad, fTriangleBased));
	
//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT quad::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));
	//CR(AllocateTriangleIndexGroups(NUM_QUAD_TRIS));

	Error:
	return R_PASS;
}

inline unsigned int quad::NumberVertices() {
//return NUM_QUAD_POINTS; 

unsigned int numVerts = (m_numVerticalDivisions + 1) * (m_numHorizontalDivisions + 1);
return numVerts;
}

inline unsigned int quad::NumberIndices() {
	//return NUM_QUAD_TRIS * 3; 

	unsigned int numDivisions = m_numVerticalDivisions * m_numHorizontalDivisions;
	unsigned int numTris = numDivisions * 2;
	unsigned int numIndices = numTris * 3;

	return numIndices;
}

bool quad::IsBillboard() {
	return m_fBillboard; 
}

void quad::SetBillboard(bool fBillboard) {
	m_fBillboard = fBillboard; 
}

bool quad::IsScaledBillboard() {
	return m_fScaledBillboard; 
}

void quad::SetScaledBillboard(bool fScale) {
	m_fScaledBillboard = fScale; 
}

RESULT quad::UpdateParams(float width, float height, vector vNormal) {
	RESULT r = R_PASS;

	if (width != m_width || height != m_height || vNormal != m_vNormal) {
		m_width = width;
		m_height = height;
		m_vNormal = vNormal;

		CR(SetVertices(width, height, vNormal));
	}
	else {
		r = R_SKIPPED;
	}

Error:
	return r;
}

RESULT quad::UpdateFromBoundingQuad(BoundingQuad* pBoundingQuad, bool fTriangleBased) {
	RESULT r = R_PASS;

	if (pBoundingQuad->GetWidth() != m_width || pBoundingQuad->GetHeight() != m_height || pBoundingQuad->GetNormal() != m_vNormal) {
		CR(SetVertices(pBoundingQuad, fTriangleBased));
	}

	SetOrigin(pBoundingQuad->GetOrigin());
	SetOrientation(pBoundingQuad->GetOrientation());
	//SetScale(pBoundingQuad->GetScale());
Error:
	return r;
}

RESULT quad::SetVertices(BoundingQuad* pBoundingQuad, bool fTriangleBased) {
	RESULT r = R_PASS;

	SetOrigin(pBoundingQuad->GetOrigin());

	m_width = pBoundingQuad->GetWidth();
	m_height = pBoundingQuad->GetHeight();
	m_vNormal = pBoundingQuad->GetNormal();

	CR(SetVertices(m_width, m_height, m_vNormal));

Error:
	return r;
}

RESULT quad::FlipUVVertical() {
	RESULT r = R_PASS;

	int vertCount = 0;

	for (int i = 0; i < m_numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_numVerticalDivisions + 1; j++) {
			uv_precision uValue = m_pVertices[vertCount].GetUV().u();
			uv_precision vValue = m_pVertices[vertCount].GetUV().v();

			vValue = (1.0f - vValue);

			m_pVertices[vertCount++].SetUV(uValue, vValue);
		}
	}

	CR(SetDirty());

Error:
	return r;
}

RESULT quad::FlipUVHorizontal() {
	RESULT r = R_PASS;

	int vertCount = 0;

	for (int i = 0; i < m_numHorizontalDivisions + 1; i++) {
		for (int j = 0; j < m_numVerticalDivisions + 1; j++) {
			uv_precision uValue = m_pVertices[vertCount].GetUV().u();
			uv_precision vValue = m_pVertices[vertCount].GetUV().v();

			uValue = (1.0f - uValue);

			m_pVertices[vertCount++].SetUV(uValue, vValue);
		}
	}

	CR(SetDirty());

Error:
	return r;
}

vector quad::GetNormal() {
	return m_vNormal;
}

// TODO: not supporting triangle based yet
//RESULT quad::SetVertices(float width, float height, vector vNormal) {
RESULT quad::SetVertices(float width, float height, vector vNormal, const uvcoord& uvTopLeft, const uvcoord& uvBottomRight) {
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

			uv_precision uRange = uvBottomRight.u() - uvTopLeft.u();
			uv_precision vRange = uvBottomRight.v() - uvTopLeft.v();

			uv_precision uValue = uvTopLeft.u() + (((float)(i) / (float)(m_numHorizontalDivisions)) * uRange);
			uv_precision vValue = uvTopLeft.v() + (((float)(j) / (float)(m_numVerticalDivisions)) * vRange);

			if (m_pTextureHeight != nullptr) {
				yValue = m_pTextureHeight->GetValueAtUV(uValue, vValue);
				yValue *= m_heightMapScale;
			}

			m_pVertices[vertCount] = vertex(point((widthInc * i) - halfWidth, static_cast<float>(yValue), (heightInc * j) - halfHeight),
				vector::jVector(1.0f),
				uvcoord(uValue, 1.0f - vValue));

			// TODO: Calculate normal (based on geometry)

			m_pVertices[vertCount].SetTangent(vector(-1.0f, 0.0f, 0.0f));
			m_pVertices[vertCount].SetBitangent(vector(0.0f, 0.0f, -1.0f));

			vertCount++;
		}
	}

	{
		vNormal.Normalize();
		quaternion qOrientation = quaternion(vector::jVector(1.0f), vNormal);
		RotationMatrix rotMat(qOrientation);
		for (unsigned int i = 0; i < NumberVertices(); i++) {
			m_pVertices[i].m_point = rotMat * m_pVertices[i].m_point;
			m_pVertices[i].m_normal = rotMat * m_pVertices[i].m_normal;
		}
	}

	for (int i = 0; i < m_numHorizontalDivisions; i++) {
		for (int j = 0; j < m_numVerticalDivisions; j++) {
			A = (i)+((m_numHorizontalDivisions + 1) * j);
			B = (i + 1) + ((m_numHorizontalDivisions + 1) * j);

			C = (i)+((m_numHorizontalDivisions + 1) * (j + 1));
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

// TODO: Parallelogram
// TODO: Trapezoid
// TODO: Rhombus
// TODO: Trapezium + Evaluate Points
quad::QUAD_TYPE quad::EvaluatePoints(point a, point b, point c) {
	return INVALID;
}