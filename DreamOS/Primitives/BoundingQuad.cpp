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
	// First calculate rotation per normal and re-orient
	
	quaternion qOrientation = quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(rotMat) * (static_cast<BoundingSphere>(rhs).GetOrigin() - GetOrigin());
	double distance = ptSphereOrigin.y();

	if (std::abs(distance) < static_cast<BoundingSphere>(rhs).GetRadius()) {
		//point ptMax = GetMaxPoint();
		//point ptMin = GetMinPoint();

		///*
		point ptMax = point(m_width, m_height, 0.0f);
		point ptMin = point(-m_width, -m_height, 0.0f);

		float closestX = std::max(ptMin.x(), std::min(ptSphereOrigin.x(), ptMax.x()));
		float closestY = std::max(ptMin.y(), std::min(ptSphereOrigin.y(), ptMax.y()));
		float closestZ = std::max(ptMin.z(), std::min(ptSphereOrigin.z(), ptMax.z()));
		//*/

		//point ptClosestPoint = ptSphereOrigin - (m_vNormal * distance);
		point ptClosestPoint = point(closestX, closestY, closestZ);

		double sphereRadiusSquared = pow(static_cast<BoundingSphere>(rhs).GetRadius(), 2.0f);
		double distanceSquared = pow((ptClosestPoint - ptSphereOrigin).magnitude(), 2.0f);

		if (distanceSquared < sphereRadiusSquared) {
			return true;
		}
	}

	return false;
}

bool BoundingQuad::Intersect(const BoundingBox& rhs) {
	// TODO:

	return false;
}

bool BoundingQuad::Intersect(const BoundingQuad& rhs) {
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

CollisionManifold BoundingQuad::Collide(const BoundingQuad& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	return manifold;
}

CollisionManifold BoundingQuad::Collide(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	return manifold;
}

CollisionManifold BoundingQuad::Collide(const BoundingSphere& rhs) {
	// First calculate rotation per normal and re-orient

	quaternion qOrientation = quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(rotMat) * (static_cast<BoundingSphere>(rhs).GetOrigin() - GetOrigin());
	double distance = ptSphereOrigin.y();

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	if (std::abs(distance) < static_cast<BoundingSphere>(rhs).GetRadius()) {
		//point ptMax = GetMaxPoint();
		//point ptMin = GetMinPoint();

		///*
		point ptMax = point(m_width/2.0f, m_height/2.0f, 0.0f);
		point ptMin = point(-m_width/2.0f, -m_height/2.0f, 0.0f);

		float closestX = std::max(ptMin.x(), std::min(ptSphereOrigin.x(), ptMax.x()));
		float closestY = std::max(ptMin.y(), std::min(ptSphereOrigin.y(), ptMax.y()));
		float closestZ = std::max(ptMin.z(), std::min(ptSphereOrigin.z(), ptMax.z()));
		//*/

		//if(ptSphereOrigin.x() )

		//point ptClosestPoint = ptSphereOrigin - (m_vNormal * distance);
		//point ptClosestPoint = point(closestX, closestY, closestZ);
		point ptClosestPoint = point(closestX, 0.0f, closestZ);

		double sphereRadiusSquared = pow(static_cast<BoundingSphere>(rhs).GetRadius(), 2.0f);
		double distanceSquared = pow((ptClosestPoint - ptSphereOrigin).magnitude(), 2.0f);

		//if (closestX <= 0.0f && closestZ < 0.0f) {
		if (distanceSquared <= sphereRadiusSquared) {
			// This is a plane-point collision

			ptClosestPoint = (rotMat * ptClosestPoint) + GetOrigin();
			
			//vector vNormal = rotMat * vector::jVector(1.0f);
			//vNormal.Normalize();

			vector vNormal = static_cast<BoundingSphere>(rhs).GetOrigin() - ptClosestPoint;
			vNormal.Normalize();

			point ptContact = ptClosestPoint;
			float penetration = static_cast<BoundingSphere>(rhs).GetRadius() - std::sqrt(distanceSquared);

			manifold.AddContactPoint(ptContact, vNormal, -penetration, 1);
		}
		/*else if (distanceSquared <= sphereRadiusSquared) {
			// Otherwise it's a point edge collision
		}*/
		
	}
	
	return manifold;
}

RESULT BoundingQuad::SetMaxPointFromOrigin(point ptMax) {		
	vector vDiff = (ptMax - GetOrigin());
	return R_SUCCESS;
}

RESULT BoundingQuad::SetHalfVector(vector vHalfVector) {
	return R_NOT_IMPLEMENTED;
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

vector BoundingQuad::GetHalfVector() {
	vector vReturn = vector(m_width/2.0f, m_height/2.0f, 0.0f);

	quaternion qOrientation = quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat(qOrientation);
	vReturn = rotMat * vReturn;

	return vReturn;
}

point BoundingQuad::GetMinPoint() {
	return (GetHalfVector() * -1.0f) + GetOrigin();
}

point BoundingQuad::GetMaxPoint() {
	return GetHalfVector() + GetOrigin();
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