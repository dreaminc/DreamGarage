#include "DimRay.h"
#include "quaternion.h"
#include "volume.h"

#include "PhysicsEngine/CollisionManifold.h"

DimRay::DimRay(point ptOrigin, vector vDirection) :
	ray(ptOrigin, vDirection)
{
	RESULT r = R_PASS;
	CR(Allocate());

	SetRayVertices(vDirection.magnitude());

	//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

DimRay::DimRay(point ptOrigin, vector vDirection, float rayVisualLength) :
	ray(ptOrigin, vDirection)
{
	RESULT r = R_PASS;
	CR(Allocate());

	SetRayVertices(rayVisualLength);

	//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

unsigned int DimRay::NumberIndices() {
	return 10;
}

unsigned int DimRay::NumberVertices() {
	return 6;
}

RESULT DimRay::Allocate() {
	RESULT r = R_PASS;

	CR(AllocateVertices(NumberVertices()));
	CR(AllocateIndices(NumberIndices()));

Error:
	return r;
}

RESULT DimRay::SetRayVertices(float rayVisualLength) {
	RESULT r = R_PASS;

	// line segment vertices
	vertex vBegin = vertex(m_ptOrigin);
	point ptEnd = m_ptOrigin + point(rayVisualLength * m_vDirection);
	vertex vEnd = vertex(ptEnd);

	int endIndex;
	int indexCounter = 0;
	int vertCounter = 0;
	float flare = 0.5f;

	// Line
	m_pVertices[vertCounter++] = vBegin;
	m_pVertices[vertCounter++] = vEnd;
	m_pIndices[indexCounter++] = 0;
	m_pIndices[indexCounter++] = endIndex = 1;

	// Arrow Line Left
	point ptArrowLineLeft = ptEnd - (vector)(m_vDirection + (m_vDirection.NormalizedCross(vector::iVector(1.0f))) * flare) * (rayVisualLength * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineLeft);
	m_pIndices[indexCounter++] = 2;
	m_pIndices[indexCounter++] = endIndex;

	// Arrow Line Right
	point ptArrowLineRight = ptEnd - (vector)(m_vDirection - (m_vDirection.NormalizedCross(vector::iVector(1.0f))) * flare) * (rayVisualLength * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineRight);
	m_pIndices[indexCounter++] = 3;
	m_pIndices[indexCounter++] = endIndex;

	// Arrow Line Front
	point ptArrowLineFront = ptEnd - (vector)(m_vDirection + (m_vDirection.NormalizedCross(vector::kVector(1.0f))) * flare) * (rayVisualLength * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineFront);
	m_pIndices[indexCounter++] = 4;
	m_pIndices[indexCounter++] = endIndex;

	// Arrow Line Back
	point ptArrowLineBack = ptEnd - (vector)(m_vDirection - (m_vDirection.NormalizedCross(vector::kVector(1.0f))) * flare) * (rayVisualLength * 0.1f);
	m_pVertices[vertCounter++] = vertex(ptArrowLineBack);
	m_pIndices[indexCounter++] = 5;
	m_pIndices[indexCounter++] = endIndex;

//Error:
	return r;
}

bool DimRay::Intersect(VirtualObj* pObj) {
	DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);

	if (pDimObj == nullptr || pDimObj->GetBoundingVolume() == nullptr) {
		return false;
	}
	else {
		return pDimObj->GetBoundingVolume()->Intersect(GetRay());
	}
}

CollisionManifold DimRay::Collide(VirtualObj* pObj) {
	DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);

	if (pDimObj == nullptr || pDimObj->GetBoundingVolume() == nullptr) {
		return CollisionManifold(this, pObj);
	}
	else {
		return pDimObj->GetBoundingVolume()->Collide(GetRay());
	}
}

// Returns a ray adjusted with the object state
// Attempts to make ray into a virtual object failed as a result of extending
// DimRay with DimObj - the issue being that otherwise every ray would need
// to have verts which might not be ideal in terms of using ray as a primitive
// The use of DimRay as an actual DimObj should be only done for debugging purposes
// since this is not a general solution

ray DimRay::GetRay(bool fAbsolute) {
	//point ptOrigin = m_ptOrigin + DimObj::GetOrigin(fAbsolute);
	point ptOrigin = DimObj::GetOrigin(fAbsolute);
	vector vDirection = RotationMatrix(GetOrientation(fAbsolute)) * m_vDirection;

	return ray(ptOrigin, vDirection);
}

ray DimRay::GetRayFromVerts() {
	point ptOrigin = m_pVertices[0].GetPoint();
	//vector vDirection = (RotationMatrix(GetOrientation()) * m_pVertices[1].GetPoint()) - (RotationMatrix(GetOrientation()) * m_pVertices[0].GetPoint());
	vector vDirection = m_pVertices[1].GetPoint() - m_pVertices[0].GetPoint();
	vDirection.Normalize();

	return ray(ptOrigin, vDirection);
}

RESULT DimRay::UpdateFromRay(const ray &rCast) {
	SetOrigin(rCast.GetOrigin());
	
	ResetRotation();
	SetDirectionVector(rCast.GetVector());

	return R_PASS;
}