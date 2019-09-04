#include "BoundingQuad.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingPlane.h"
#include <algorithm>

#include "VirtualObj.h"
#include "PhysicsEngine/CollisionManifold.h"

BoundingQuad::BoundingQuad(VirtualObj *pParentObject) :
	BoundingVolume(pParentObject),
	m_width(0.0f),
	m_height(0.0f),
	m_vNormal(vector::jVector(1.0f))
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

bool BoundingQuad::Intersect(const BoundingBox& rhs) {
	// TODO:

	return false;
}

bool BoundingQuad::Intersect(const BoundingQuad& rhs) {

	// Do both
	for (int i = 0; i < 4; i++) {
		line edgeQuad = static_cast<BoundingQuad>(rhs).GetAbsoluteQuadEdge((QuadEdge)(i));

		if (Intersect(edgeQuad)) {
			return true;
		}
	}	

	for (int i = 0; i < 4; i++) {
		line edgeQuad = GetAbsoluteQuadEdge((QuadEdge)(i));

		if (static_cast<BoundingQuad>(rhs).Intersect(edgeQuad)) {
			return true;
		}
	}

	return false;
}

bool BoundingQuad::Intersect(const BoundingPlane& rhs) {
	// TODO: 

	return false;
}

bool BoundingQuad::Intersect(point& pt) {
	// TODO:

	return false;
}

CollisionManifold BoundingQuad::Collide(const BoundingQuad& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());
	point ptCollide;

	// Do for both

	vector vNormal = GetAbsoluteNormal();

	for (int i = 0; i < 4; i++) {
		line edgeQuad = static_cast<BoundingQuad>(rhs).GetAbsoluteQuadEdge((QuadEdge)(i));

		if (Intersect(edgeQuad, &ptCollide)) {
			float penetration = 0.0f;	// TODO: Fix this
			manifold.AddContactPoint(ptCollide, vNormal, -penetration, 1);
		}
	}

	vNormal = static_cast<BoundingQuad>(rhs).GetAbsoluteNormal();

	for (int i = 0; i < 4; i++) {
		line edgeQuad = GetAbsoluteQuadEdge((QuadEdge)(i));

		if (static_cast<BoundingQuad>(rhs).Intersect(edgeQuad, &ptCollide)) {
			float penetration = 0.0f;	// TODO: Fix this
			manifold.AddContactPoint(ptCollide, vNormal, -penetration, 1);
		}
	}

	return manifold;
}

CollisionManifold BoundingQuad::Collide(const BoundingPlane& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

CollisionManifold BoundingQuad::Collide(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

bool BoundingQuad::Intersect(const BoundingSphere& rhs) {
	
	// TODO: Change the arch to utilize inheritence and not duplicate this code
	// Lets fake a sphere test for early exit
	vector vScale = GetScale(true);
	//vector vHV = vector(m_width/2.0f * vScale.x(), m_height/2.0f * vScale.y(), length * vScale.z());
	float quadRadiusSquared = pow((m_width / 2.0f)*vScale.x(), 2.0f) + pow((m_height / 2.0f)*vScale.y(), 2.0f);
	double distance = (const_cast<BoundingSphere&>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin()).magnitude();
	if ((distance * distance) > (rhs.GetRadiusSquared() + quadRadiusSquared))
		return false;
	
	// First calculate rotation per normal and re-orient

	//quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix matRotation = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(matRotation) * (static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin());
	//double distance = ptSphereOrigin.y();
	distance = ptSphereOrigin.y();

	if (std::abs(distance) < static_cast<BoundingSphere>(rhs).GetRadius()) {
		//point ptMax = GetMaxPoint();
		//point ptMin = GetMinPoint();

		///*
		//point ptMax = point(GetWidth() / 2.0f, GetHeight() / 2.0f, 0.0f);
		//point ptMin = point((-1.0f * GetWidth()) / 2.0f, (-1.0f * GetHeight()) / 2.0f, 0.0f);

		point ptMax = point(GetWidth() / 2.0f, 0.0f, GetHeight() / 2.0f);
		point ptMin = point((-1.0f * GetWidth()) / 2.0f, 0.0f, (-1.0f * GetHeight()) / 2.0f);

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

CollisionManifold BoundingQuad::Collide(const BoundingSphere& rhs) {
	// First calculate rotation per normal and re-orient

	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix rotMat = RotationMatrix(qOrientation);

	point ptSphereOrigin = inverse(rotMat) * (static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin());
	double distance = ptSphereOrigin.y();

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	if (std::abs(distance) < static_cast<BoundingSphere>(rhs).GetRadius()) {
		//point ptMax = GetMaxPoint();
		//point ptMin = GetMinPoint();

		///*
		//point ptMax = point(GetWidth() / 2.0f, GetHeight() / 2.0f, 0.0f);
		//point ptMin = point((-1.0f * GetWidth()) / 2.0f, (-1.0f * GetHeight())/2.0f, 0.0f);

		point ptMax = point(GetWidth() / 2.0f, 0.0f, GetHeight() / 2.0f);
		point ptMin = point((-1.0f * GetWidth()) / 2.0f, 0.0f, (-1.0f * GetHeight()) / 2.0f);

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

			ptClosestPoint = (rotMat * ptClosestPoint) + GetAbsoluteOrigin();
			
			//vector vNormal = rotMat * vector::jVector(1.0f);
			//vNormal.Normalize();

			vector vNormal = static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - ptClosestPoint;
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

// TODO: Push some of this functionality into primitives potentially 
bool BoundingQuad::Intersect(const line& l, point *pptCollision) {
	double t = -1.0f;

	vector vNormal = RotationMatrix(GetAbsoluteOrientation()) * m_vNormal;
	vNormal.Normalize();

	ray r = static_cast<line>(l).GetRay();

	t = ((vector)(GetAbsoluteOrigin() - r.GetOrigin())).dot(vNormal);
	double denom = r.GetVector().Normal().dot(vNormal);

	if (denom != 0) {
		t /= denom;

		if (t >= 0 && t < static_cast<line>(l).length()) {
			quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
			RotationMatrix rotMat = RotationMatrix(qOrientation);

			point ptPlane = inverse(rotMat) * ((r.GetOrigin() + r.GetVector() * t) - GetAbsoluteOrigin());

			if (ptPlane.x() < GetWidth(true) / 2.0f && ptPlane.x() > -GetWidth(true) / 2.0f &&
				ptPlane.z() < GetHeight(true) / 2.0f && ptPlane.z() > -GetHeight(true) / 2.0f)
			{
				if (pptCollision != nullptr) {
					*pptCollision = r.GetOrigin() + (r.GetVector() * t);
				}

				return true;
			}
		}
	}
	else {
		// parallel 
	}

	return false;
}

bool BoundingQuad::Intersect(const ray& r) {
	double t = -1.0f;

	vector vNormal = RotationMatrix(GetAbsoluteOrientation()) * m_vNormal;
	vNormal.Normalize();

	t = ((vector)(GetAbsoluteOrigin() - r.GetOrigin())).dot(vNormal);
	double denom = r.GetVector().Normal().dot(vNormal);

	if (denom != 0) {
		t /= denom;

		if (t >= 0) {
			quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
			RotationMatrix rotMat = RotationMatrix(qOrientation);

			point ptPlane = inverse(rotMat) * ((r.GetOrigin() + r.GetVector() * t) - GetAbsoluteOrigin());

			if (ptPlane.x() < GetWidth(true) / 2.0f && ptPlane.x() > -GetWidth(true) / 2.0f &&
				ptPlane.z() < GetHeight(true) / 2.0f && ptPlane.z() > -GetHeight(true) / 2.0f)
			{
				return true;
			}
		}
	}
	else {
		// parallel 
	}

	return false;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
// Note this does not check bounds like intersect
CollisionManifold BoundingQuad::Collide(const ray &rCast) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, nullptr);

	double t = -1.0f;

	vector vNormal = RotationMatrix(GetAbsoluteOrientation()) * m_vNormal;
	vNormal.Normalize();
	
	t = ((vector)(GetAbsoluteOrigin() - rCast.GetOrigin())).dot(vNormal);
	double denom = rCast.GetVector().Normal().dot(vNormal);

	if (denom != 0) {
		t /= denom;

		if (t > 0) {
			point ptContact = rCast.GetOrigin() + rCast.GetVector() * t;
			manifold.AddContactPoint(ptContact, vNormal, 0.0f, 1);
		}
	}
	else {
		// parallel 
	}

	return manifold;
}

RESULT BoundingQuad::SetMaxPointFromOrigin(point ptMax) {		
	vector vDiff = (ptMax - GetOrigin());
	return R_SUCCESS;
}

RESULT BoundingQuad::SetBounds(float width, float height) {
	m_width = width;
	m_height = height;

	return R_SUCCESS;
}

RESULT BoundingQuad::SetHalfVector(vector vHalfVector) {
	RESULT r = R_PASS;

	// TODO: Apply Quad normal

	m_width = vHalfVector.x() * 2.0f;
	m_height = vHalfVector.z() * 2.0f;

//Error:
	return r;
}

double BoundingQuad::GetWidth(bool fAbsolute) {
	return m_width * GetScale(fAbsolute).x();
}

double BoundingQuad::GetHeight(bool fAbsolute) {
	return m_height * GetScale(fAbsolute).y();
}

vector BoundingQuad::GetNormal() {
	return m_vNormal;
}

vector BoundingQuad::GetAbsoluteNormal() {
	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix matRotation = RotationMatrix(qOrientation);
	vector vNormal = (matRotation * m_vNormal);
	return vNormal.Normal();
}

// This will re-orient the HV perpendicular to the normal
// So height is now in terms of the Z component
double BoundingQuad::GetLeft(bool fAbsolute) {
	vector vHVNormal;

	if (fAbsolute)
		vHVNormal = GetOrigin() - (vector)(inverse(RotationMatrix(GetOrientation())) * GetHalfVector());
	else
		return GetCenter().x() - (m_width / 2.0f);
	
	return vHVNormal.x();
}

double BoundingQuad::GetRight(bool fAbsolute) {
	vector vHVNormal;

	if (fAbsolute)
		vHVNormal = GetOrigin() + (vector)(inverse(RotationMatrix(GetOrientation())) * GetHalfVector());
	else
		return GetCenter().x() + (m_width / 2.0f);

	return vHVNormal.x();
}

double BoundingQuad::GetTop(bool fAbsolute) {
	vector vHVNormal;

	if (fAbsolute)
		vHVNormal = GetOrigin() + (vector)(inverse(RotationMatrix(GetOrientation())) * GetHalfVector());
	else
		return GetCenter().z() + (m_height / 2.0f);

	return vHVNormal.z();
}

double BoundingQuad::GetBottom(bool fAbsolute) {
	vector vHVNormal;

	if (fAbsolute)
		vHVNormal = GetOrigin() - (vector)(inverse(RotationMatrix(GetOrientation())) * GetHalfVector());
	else
		return GetCenter().z() - (m_height / 2.0f);

	return vHVNormal.z();
}

// This is busted
vector BoundingQuad::GetHalfVector(bool fAbsolute) {
	vector vScale = GetScale(fAbsolute);
	//RotationMatrix matRotation = RotationMatrix(GetAbsoluteOrientation());	// .GetEulerAngles(&phi, &theta, &psi);
	RotationMatrix matRotation = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

	double width = 0.0f;
	double height = 0.0f;
	double length = 0.0f;

	for (int i = 0; i < 8; i++) {
		point pt = GetQuadPoint((QuadPoint)(i));
		pt = matRotation * pt;

		if (pt.x() > width)
			width = pt.x();

		if (pt.y() > height)
			height = pt.y();

		if (pt.z() > length)
			length = pt.z();
	}

	return vector(width * vScale.x(), height * vScale.y(), length * vScale.z());
}

point BoundingQuad::GetMinPoint(bool fAbsolute) {
	return (GetHalfVector(fAbsolute) * -1.0f) + GetOrigin();
}

point BoundingQuad::GetMaxPoint(bool fAbsolute) {
	return GetHalfVector(fAbsolute) + GetOrigin();
}

point BoundingQuad::GetQuadPoint(QuadPoint ptType) {
	point ptRet = point();

	double halfWidth = m_width / 2.0f;
	double halfHeight = m_height / 2.0f;

	switch (ptType) {
		case QuadPoint::TOP_RIGHT: {
			ptRet.x() += halfWidth;
			ptRet.z() += halfHeight;
		} break;

		case QuadPoint::TOP_LEFT: {
			ptRet.x() -= halfWidth;
			ptRet.z() += halfHeight;
		} break;

		case QuadPoint::BOTTOM_RIGHT: {
			ptRet.x() += halfWidth;
			ptRet.z() -= halfHeight;
		} break;

		case QuadPoint::BOTTOM_LEFT: {
			ptRet.x() -= halfWidth;
			ptRet.z() -= halfHeight;
		} break;
	}

	// Accounts for normal rotation
	quaternion qOrientation = quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix matRotation(qOrientation);
	ptRet = matRotation * ptRet;

	//ptRet = ptRet + GetOrigin();
		
	return ptRet;
}

point BoundingQuad::GetAbsoluteQuadPoint(QuadPoint ptType) {
	point ptRet = point();

	double halfWidth = m_width / 2.0f;
	double halfHeight = m_height / 2.0f;

	switch (ptType) {
	case QuadPoint::TOP_RIGHT: {
		ptRet.x() += halfWidth;
		ptRet.z() += halfHeight;
	} break;

	case QuadPoint::TOP_LEFT: {
		ptRet.x() -= halfWidth;
		ptRet.z() += halfHeight;
	} break;

	case QuadPoint::BOTTOM_RIGHT: {
		ptRet.x() += halfWidth;
		ptRet.z() -= halfHeight;
	} break;

	case QuadPoint::BOTTOM_LEFT: {
		ptRet.x() -= halfWidth;
		ptRet.z() -= halfHeight;
	} break;
	}

	// Absolute Orientation
	quaternion qOrientation = GetAbsoluteOrientation() * quaternion(vector::jVector(1.0f), m_vNormal);
	RotationMatrix matRotation(qOrientation);
	ptRet = matRotation * ptRet;

	ptRet = ptRet + GetOrigin();

	return ptRet;
}

line BoundingQuad::GetAbsoluteQuadEdge(QuadEdge edgeType) {
	point ptA, ptB;

	switch (edgeType) {
		case QuadEdge::LEFT: {
			ptB = GetAbsoluteQuadPoint(QuadPoint::TOP_LEFT);
			ptA = GetAbsoluteQuadPoint(QuadPoint::BOTTOM_LEFT);
		} break;

		case QuadEdge::RIGHT: {
			ptB = GetAbsoluteQuadPoint(QuadPoint::TOP_RIGHT);
			ptA = GetAbsoluteQuadPoint(QuadPoint::BOTTOM_RIGHT);
		} break;

		case QuadEdge::TOP: {
			ptB = GetAbsoluteQuadPoint(QuadPoint::TOP_RIGHT);
			ptA = GetAbsoluteQuadPoint(QuadPoint::TOP_LEFT);
		} break;

		case QuadEdge::BOTTOM: {
			ptB = GetAbsoluteQuadPoint(QuadPoint::BOTTOM_RIGHT);
			ptA = GetAbsoluteQuadPoint(QuadPoint::BOTTOM_LEFT);
		} break;
	}

	return line(ptA, ptB);
}