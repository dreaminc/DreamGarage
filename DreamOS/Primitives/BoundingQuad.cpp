#include "BoundingQuad.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include <algorithm>

#include "VirtualObj.h"
#include "PhysicsEngine/CollisionManifold.h"

BoundingQuad::BoundingQuad(VirtualObj *pParentObject) :
	BoundingVolume(pParentObject),
	m_width(0.0f),
	m_height(0.0f),
	m_vNormal(vector())
{
	// empty
}

BoundingQuad::BoundingQuad(VirtualObj *pParentObject, point ptOrigin, vector vNormal, double width, double height) :
	BoundingVolume(pParentObject, ptOrigin),
	m_width(width),
	m_height(height),
	m_vNormal(vNormal)
{
	// empty
}

bool BoundingQuad::Intersect(const BoundingSphere& rhs) {
	point ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation())) * (ptSphereOrigin - GetOrigin()));
	
	// TODO:

	return false;
}

bool BoundingQuad::Intersect(const BoundingBox& rhs) {
	// TODO:

	return false;
}

bool BoundingQuad::Intersect(point& pt) {
	// TODO:

	return false;
}

bool BoundingQuad::Intersect(const ray& r) {
	// TODO:

	return false;
}

CollisionManifold BoundingQuad::Collide(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	return manifold;
}

CollisionManifold BoundingQuad::Collide(const BoundingSphere& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	return manifold;
}

RESULT BoundingQuad::SetMaxPointFromOrigin(point ptMax) {		
	vector vDiff = (ptMax - GetOrigin());
	return R_SUCCESS;
}

double BoundingQuad::GetWidth() {
	return m_width;
}

double BoundingQuad::GetHeight() {
	return m_height;
}

vector BoundingQuad::GetNormal() {
	return m_vNormal;
}

point BoundingQuad::GetQuadPoint(QuadPoint ptType) {
	point ptRet = point();

	switch (ptType) {
		case QuadPoint::TOP_RIGHT: {
			ptRet.x() += m_width;
			ptRet.y() += m_height;
		} break;

		case QuadPoint::TOP_LEFT: {
			ptRet.x() -= m_width;
			ptRet.y() += m_height;
		} break;

		case QuadPoint::BOTTOM_RIGHT: {
			ptRet.x() += m_width;
			ptRet.y() -= m_height;
		} break;

		case QuadPoint::BOTTOM_LEFT: {
			ptRet.x() -= m_width;
			ptRet.y() -= m_height;
		} break;
	}

	quaternion qOrientation = quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat(qOrientation);
	ptRet = rotMat * ptRet;
	ptRet = ptRet + GetOrigin();
		
	return ptRet;
}