#include "DimPlane.h"

#include "PhysicsEngine/CollisionManifold.h"

DimPlane::DimPlane(point ptOrigin, vector vNormal) :
	plane(ptOrigin, vNormal)
{
	RESULT r = R_PASS;
	CR(Allocate());

	SetPlaneVertices();

	//TODO: BoundingPlane
	CR(InitializeBoundingPlane(GetOrigin(), GetNormal()));

	//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

unsigned int DimPlane::NumberIndices() {
	return 6;
}

unsigned int DimPlane::NumberVertices() {
	return 4;
}

RESULT DimPlane::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));

Error:
	return r;
}

RESULT DimPlane::SetPlaneVertices(float scale) {
	RESULT r = R_PASS;

	int vertCounter = 0;

	// Set up indices 
	TriangleIndexGroup *pTriIndices = reinterpret_cast<TriangleIndexGroup*>(m_pIndices);

	// Vertices 
	point ptVertPoint = point(-scale, 0.0f, -scale);
	uvcoord uvUVCoord = uvcoord(0.0f, 0.0f);
	m_pVertices[vertCounter++] = vertex(ptVertPoint, vector::jVector(1.0f), uvUVCoord);

	ptVertPoint = point(-scale, 0.0f, scale);
	uvUVCoord = uvcoord(0.0f, 1.0f);
	m_pVertices[vertCounter++] = vertex(ptVertPoint, vector::jVector(1.0f), uvUVCoord);

	ptVertPoint = point(scale, 0.0f, scale);
	uvUVCoord = uvcoord(1.0f, 1.0f);
	m_pVertices[vertCounter++] = vertex(ptVertPoint, vector::jVector(1.0f), uvUVCoord);

	ptVertPoint = point(scale, 0.0f, -scale);
	uvUVCoord = uvcoord(1.0f, 0.0f);
	m_pVertices[vertCounter++] = vertex(ptVertPoint, vector::jVector(1.0f), uvUVCoord);

	// Orient the points accordingly 
	{
		vector vNormal = m_vNormal.Normal();
		quaternion qOrientation = quaternion(vector::jVector(1.0f), vNormal);
		RotationMatrix rotMat(qOrientation);

		for (unsigned int i = 0; i < NumberVertices(); i++) {
			m_pVertices[i].m_point = rotMat * m_pVertices[i].m_point;
			m_pVertices[i].m_normal = rotMat * m_pVertices[i].m_normal;
		}
	}

	// Indices 
	pTriIndices[0] = TriangleIndexGroup(0, 1, 2);
	pTriIndices[1] = TriangleIndexGroup(2, 3, 0);

//Error:
	return r;
}

//bool DimPlane::Intersect(VirtualObj* pObj) {
//
//}
//
//CollisionManifold DimPlane::Collide(VirtualObj* pObj) {
//
//}

RESULT DimPlane::UpdateFromPlane(const plane &planeSource) {
	SetOrigin(planeSource.GetPosition());

	ResetRotation();
	SetNormal(planeSource.GetNormal());

	return R_PASS;
}