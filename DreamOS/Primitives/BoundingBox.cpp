#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingQuad.h"
#include "BoundingPlane.h"
#include <algorithm>

#include "VirtualObj.h"
#include "PhysicsEngine/CollisionManifold.h"
#include "PhysicsEngine/Simplex.h"

BoundingBox::BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type) :
	BoundingVolume(pParentObject),
	m_type(type),
	m_vHalfSize(vector(1.0f, 1.0f, 1.0f))
{
	// Empty
}

BoundingBox::BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type, point ptOrigin, vector vHalfSize) :
	BoundingVolume(pParentObject, ptOrigin),
	m_type(type),
	m_vHalfSize(vHalfSize)
{
	// Empty
}

BoundingBox::BoundingBox(VirtualObj *pParentObject, BoundingBox::Type type, point ptMin, point ptMax) :
	BoundingVolume(pParentObject),
	m_type(type)
{
	BoundingVolume::UpdateBoundingVolumeMinMax(ptMin, ptMax);
}

bool BoundingBox::Intersect(const BoundingSphere& rhs) {

	point ptSphereOrigin = static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin();
	point ptMax = GetMaxPoint();
	point ptMin = GetMinPoint();

	if (m_type == Type::OBB) {
		//ptSphereOrigin = GetOrigin() - (point)(inverse(RotationMatrix(GetOrientation())) * (GetOrigin() - ptSphereOrigin));
		ptSphereOrigin = (point)(inverse(RotationMatrix(GetAbsoluteOrientation())) * (ptSphereOrigin - GetAbsoluteOrigin()));
		ptMax = GetHalfVector();
		ptMin = GetHalfVector() * -1.0f;
	}

	// TODO: We may want to replace this with SAT as a coarse early test instead
	float closestX = std::max(ptMin.x(), std::min(ptSphereOrigin.x(), ptMax.x()));
	float closestY = std::max(ptMin.y(), std::min(ptSphereOrigin.y(), ptMax.y()));
	float closestZ = std::max(ptMin.z(), std::min(ptSphereOrigin.z(), ptMax.z()));

	//point ptClosestPoint = point::min(point::max(ptSphereOrigin, ptMin), ptMax);
	point ptClosestPoint = point(closestX, closestY, closestZ);

	double distanceSquared = pow((ptClosestPoint - ptSphereOrigin).magnitude(), 2.0f);
	double sphereRadiusSquared = pow(static_cast<BoundingSphere>(rhs).GetRadius(), 2.0f);

	if (distanceSquared < sphereRadiusSquared) {
		return true;
	}
	else {
		return false;
	}
}

CollisionManifold BoundingBox::Collide(const BoundingPlane& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());
	
	// TODO: 

	return manifold;
}

CollisionManifold BoundingBox::Collide(const BoundingSphere& rhs) {
	point ptSphereOrigin = static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin();
	point ptBoxOrigin = GetAbsoluteOrigin();
	point ptMax = GetMaxPoint();
	point ptMin = GetMinPoint();

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	if (m_type == Type::OBB) {
		ptSphereOrigin = (point)(inverse(RotationMatrix(GetAbsoluteOrientation())) * (ptSphereOrigin - GetAbsoluteOrigin()));
		ptMax = GetHalfVector();
		ptMin = GetHalfVector() * -1.0f;
	}

	float closestX = std::max(ptMin.x(), std::min(ptSphereOrigin.x(), ptMax.x()));
	float closestY = std::max(ptMin.y(), std::min(ptSphereOrigin.y(), ptMax.y()));
	float closestZ = std::max(ptMin.z(), std::min(ptSphereOrigin.z(), ptMax.z()));

	point ptClosestPoint = point(closestX, closestY, closestZ);

	double sphereRadiusSquared = pow(static_cast<BoundingSphere>(rhs).GetRadius(), 2.0f);
	double distanceSquared = pow((ptClosestPoint - ptSphereOrigin).magnitude(), 2.0f);

	if (distanceSquared < sphereRadiusSquared) {
		// Convert back to world coordinates
		if (m_type == Type::OBB) {
			ptClosestPoint = (RotationMatrix(GetAbsoluteOrientation()) * ptClosestPoint) + GetAbsoluteOrigin();
		}

		vector vNormal = static_cast<BoundingSphere>(rhs).GetAbsoluteOrigin() - ptClosestPoint;
		vNormal.Normalize();

		point ptContact = ptClosestPoint;
		float penetration = static_cast<BoundingSphere>(rhs).GetRadius() - std::sqrt(distanceSquared);

		manifold.AddContactPoint(ptContact, vNormal, -penetration, 1);
	}

	return manifold;
}

point BoundingBox::GetFarthestPointInDirection(vector vDirection) {
	
	// First orient vDirection to box (easier if AABB)

	if (m_type == Type::AABB) {
		point ptReturn;
	
		point ptMax = GetMaxPoint();
		point ptMin = GetMinPoint();
	
		ptReturn.x() = vDirection.x() >= 0 ? ptMax.x() : ptMin.x();
		ptReturn.y() = vDirection.y() >= 0 ? ptMax.y() : ptMin.y();
		ptReturn.z() = vDirection.z() >= 0 ? ptMax.z() : ptMin.z();
	
		return ptReturn;
	}
	else {
		RotationMatrix matRotation = RotationMatrix(GetAbsoluteOrientation());
		vector vDirectionAdjust = inverse(matRotation) * vDirection;

		point ptReturn;

		point ptMax = GetHalfVector();

		ptReturn.x() = vDirectionAdjust.x() >= 0 ? ptMax.x() : -ptMax.x();
		ptReturn.y() = vDirectionAdjust.y() >= 0 ? ptMax.y() : -ptMax.y();
		ptReturn.z() = vDirectionAdjust.z() >= 0 ? ptMax.z() : -ptMax.z();

		ptReturn = GetOrigin() + (vector)(matRotation * ptReturn);

		return ptReturn;
	}

	// Keeping this since it's pretty useful code for a point cloud 
	//BoundingBox::BoxPoint bestBoxPoint;
	//float bestDotProd = -INFINITY;
	//point ptBestPoint;
	//
	//for (int i = 0; i < (int)BoundingBox::BoxPoint::INVALID; i++) {
	//	point ptBox = GetBoxPoint((BoundingBox::BoxPoint)(i), true);
	//
	//	float d = ((vector)(ptBox)).dot(vDirection);
	//
	//	if (d > bestDotProd) {
	//		bestBoxPoint = (BoundingBox::BoxPoint)(i);
	//		bestDotProd = d;
	//		ptBestPoint = ptBox;
	//	}
	//
	//}
	//
	//return ptBestPoint;	
}

point BoundingBox::GetSupportPoint(const BoundingBox& bbA, const BoundingBox& bbB, vector vDirection) {
	point ptBBA = static_cast<BoundingBox>(bbA).GetFarthestPointInDirection(vDirection);
	point ptBBB = static_cast<BoundingBox>(bbB).GetFarthestPointInDirection(-1.0f * vDirection);

	return (point)(ptBBA - ptBBB);
}

// Project half size onto vector axis
double BoundingBox::TransformToAxis(const vector &vAxis) {
	double retVal = 0.0f;

	retVal += GetHalfVector().x() * std::abs(vAxis.dot(GetAxis(BoxAxis::X_AXIS)));
	retVal += GetHalfVector().y() * std::abs(vAxis.dot(GetAxis(BoxAxis::Y_AXIS)));
	retVal += GetHalfVector().z() * std::abs(vAxis.dot(GetAxis(BoxAxis::Z_AXIS)));

	return retVal;
}

double TransformToAxis(const vector &vAxis, vector vHalfVector, const vector vAxes[3]) {
	double retVal = 0.0f;

	retVal += vHalfVector.x() * std::abs(vAxis.dot(vAxes[0]));
	retVal += vHalfVector.y() * std::abs(vAxis.dot(vAxes[1]));
	retVal += vHalfVector.z() * std::abs(vAxis.dot(vAxes[2]));

	return retVal;
}

double TransformToAxisAABB(vector vAxis, vector vHalfVector) {
	double retVal = 0.0f;

	retVal += vHalfVector.x() * std::abs(vAxis.x());
	retVal += vHalfVector.y() * std::abs(vAxis.y());
	retVal += vHalfVector.z() * std::abs(vAxis.z());

	return retVal;
}

// This treats box A as AABB and box B as OBB
double OverlapAxisDistanceAABBOBB(const vector &vAxis,
	vector vHalfVectorA, vector vAxesA[3],
	vector vHalfVectorB, point ptOriginB, vector vAxesB[3])
{

	double projectA = TransformToAxisAABB(vAxis, vHalfVectorA);

	//double projectA = TransformToAxis(vAxis, vHalfVectorA, vAxesA);
	double projectB = TransformToAxis(vAxis, vHalfVectorB, vAxesB);

	// Which direction (negate the axis)
	double distanceBetweenObjects = ((vector)ptOriginB).dot(vAxis);
	distanceBetweenObjects = std::abs(distanceBetweenObjects);

	return (projectA + projectB) - distanceBetweenObjects;
}

bool IntersectSATAABB(const BoundingBox& lhs, const BoundingBox& rhs) {
	
	double temp;
	double minAxisDistance = std::numeric_limits<double>::infinity();
	vector vAxis, vAxisTemp;
	float penetration = 0.0f;

	// Move test into space of this bounding box
	point ptBoxAOrigin = static_cast<BoundingBox>(lhs).GetAbsoluteOrigin();
	vector vBoxAHV = static_cast<BoundingBox>(lhs).GetHalfVector();
	quaternion qBoxAOrientation = static_cast<BoundingBox>(lhs).GetAbsoluteOrientation();

	RotationMatrix matRotation = RotationMatrix(qBoxAOrientation);
	auto matInverseRotation = inverse(matRotation);

	point ptBoxBOrigin = static_cast<BoundingBox>(rhs).GetAbsoluteOrigin();
	vector vBoxBHV = static_cast<BoundingBox>(rhs).GetHalfVector();
	quaternion qBoxBOrientation = static_cast<BoundingBox>(rhs).GetAbsoluteOrientation();

	vector vAxesA[3], vAxesB[3];

	// Probably can replace this
	vAxesA[0] = vector::iVector(1.0f);
	vAxesA[1] = vector::jVector(1.0f);
	vAxesA[2] = vector::kVector(1.0f);

	// Get Box B axes in A space
	for (int i = 0; i < 3; i++) {
		vAxesB[i] = matInverseRotation * static_cast<BoundingBox>(rhs).GetAxis(BoundingBox::BoxAxis(i));
	}
	ptBoxBOrigin = matInverseRotation * (ptBoxBOrigin - ptBoxAOrigin);

	for (int i = 0; i < 3; i++) {
		vector vAxisA = vAxesA[i];

		// Self Box Axes
		if (temp = OverlapAxisDistanceAABBOBB(vAxisTemp = vAxisA, vBoxAHV, vAxesA, vBoxBHV, ptBoxBOrigin, vAxesB) < 0.0f) {
			return false;
		}

		// The other box Axes (todo: test if it's an OBB)
		if (temp = OverlapAxisDistanceAABBOBB(vAxisTemp = vAxesB[i], vBoxAHV, vAxesA, vBoxBHV, ptBoxBOrigin, vAxesB) < 0.0f) {
			return false;
		}

		// Go through the cross product of each of the axes
		for (int j = 0; j < 3; j++) {
			vector vAxisB = vAxesB[j];

			// Ensure not same
			if (vAxisA != vAxisB) {
				if (temp = OverlapAxisDistanceAABBOBB(vAxisTemp = vAxisB.cross(vAxisA), vBoxAHV, vAxesA, vBoxBHV, ptBoxBOrigin, vAxesB) < 0.0f) {
					return false;
				}
			}
		}
	}

	return true;
}

bool BoundingBox::IntersectSAT(const BoundingBox& rhs) {
	if (m_type == Type::AABB) {
		// TODO: Doesn't think about other box as AABB or not
		point ptMaxA = GetAbsoluteOrigin() + GetHalfVector();
		point ptMinA = GetAbsoluteOrigin() - GetHalfVector();

		point ptMaxB = const_cast<BoundingBox&>(rhs).GetAbsoluteOrigin() + static_cast<BoundingBox>(rhs).GetHalfVector();
		point ptMinB = const_cast<BoundingBox&>(rhs).GetAbsoluteOrigin() - static_cast<BoundingBox>(rhs).GetHalfVector();

		if ((ptMaxA > ptMinB) && (ptMaxB > ptMinA))
			return true;
		else
			return false;
	}
	else if (m_type == Type::OBB) {
		for (int i = 0; i < 3; i++) {
			// Self Box Axes
			if (!OverlapOnAxis(rhs, GetAxis(BoundingBox::BoxAxis(i)))) {
				return false;
			}

			// The other box Axes (todo: test if it's an OBB)
			if (!OverlapOnAxis(rhs, static_cast<BoundingBox>(rhs).GetAxis(BoundingBox::BoxAxis(i)))) {
				return false;
			}

			// Go through the cross product of each of the axes
			///*
			for (int j = 0; j < 3; j++) {
				if (!OverlapOnAxis(rhs, GetAxis(BoundingBox::BoxAxis(i)).cross(static_cast<BoundingBox>(rhs).GetAxis(BoundingBox::BoxAxis(j))))) {
					return false;
				}
			}
			//*/
		}

		return true;
	}

	return false;
}

bool BoundingBox::IntersectGJK(const BoundingBox& rhs) {
	Simplex intersectionSimplex = Simplex();

	vector vDirection = vector::iVector(1.0f);

	// First point
	point ptSupport = GetSupportPoint(*this, rhs, vDirection);
	intersectionSimplex.UpdateSimplex(ptSupport, &vDirection);

	do {
		ptSupport = GetSupportPoint(*this, rhs, vDirection);

		if (vDirection.dot(ptSupport) < 0.0f) {
			return false;
		}

		if (intersectionSimplex.UpdateSimplex(ptSupport, &vDirection)) {
			return true;
		}

	} while (1);

	return false;
}

bool BoundingBox::Intersect(const BoundingBox& rhs) {

	return IntersectSATAABB(*this, rhs);
	//return IntersectSAT(rhs);
	//return IntersectGJK(rhs);
	
}

CollisionManifold BoundingBox::Collide(const BoundingBox& rhs) {
	return CollideSAT(rhs);
	//return CollideBruteForce(rhs);

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// TODO:

	return manifold;
}

// http://www.willperone.net/Code/coderr.php
vector BoundingBox::GetHalfVector(bool fAbsolute) {
	vector vScale = GetScale(fAbsolute);

	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetAbsoluteOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double width = 0.0f;
		double height = 0.0f;
		double length = 0.0f;

		for (int i = 0; i < 8; i++) {
			point pt = rotMat * GetBoxPoint((BoxPoint)(i));

			if (pt.x() > width)
				width = pt.x();

			if (pt.y() > height)
				height = pt.y();

			if (pt.z() > length)
				length = pt.z();
		}

		return vector(width * vScale.x(), height * vScale.y(), length * vScale.z());
	}

	// Otherwise it's OBB
	//return m_vHalfSize;
	return vector(m_vHalfSize.x() * vScale.x(), m_vHalfSize.y() * vScale.y(), m_vHalfSize.z() * vScale.z());
}

double BoundingBox::OverlapOnAxisDistance(const BoundingBox& rhs, const vector &vAxis) {
	// Project the half-size of one onto axis
	double selfProject = TransformToAxis(vAxis);
	double rhsProject = static_cast<BoundingBox>(rhs).TransformToAxis(vAxis);

	vector vToCenter = static_cast<BoundingBox>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin();

	return (selfProject + rhsProject) - std::abs(vToCenter.dot(vAxis));
}

bool BoundingBox::OverlapOnAxis(const BoundingBox& rhs, const vector &vAxis) {
	// Project the half-size of one onto axis
	//double selfProject = TransformToAxis(vAxis);
	//double rhsProject = static_cast<BoundingBox>(rhs).TransformToAxis(vAxis);

	//return (OverlapOnAxisDistance(rhs, vAxis) <= (selfProject + rhsProject));
	return (OverlapOnAxisDistance(rhs, vAxis) >= 0.0f);
}

//double OverlapOnAxisDistance(const BoundingBox& rhs, const vector &vAxis) {
//	// Project the half-size of one onto axis
//	double selfProject = TransformToAxis(vAxis);
//	double rhsProject = static_cast<BoundingBox>(rhs).TransformToAxis(vAxis);
//
//	vector vToCenter = static_cast<BoundingBox>(rhs).GetAbsoluteOrigin() - GetAbsoluteOrigin();
//	return (selfProject + rhsProject) - std::abs(vToCenter.dot(vAxis));
//}

vector BoundingBox::GetAxis(BoxAxis boxAxis, bool fOriented) {
	vector retVector = vector(0.0f, 0.0f, 0.0f);

	switch (boxAxis) {
	case BoxAxis::X_AXIS: retVector = vector::iVector(1.0f); break;
	case BoxAxis::Y_AXIS: retVector = vector::jVector(1.0f); break;
	case BoxAxis::Z_AXIS: retVector = vector::kVector(1.0f); break;
	}

	// Rotate by OBB if so
	//if (m_type == Type::OBB) {
	if (fOriented && m_type == Type::OBB) {
		retVector = RotationMatrix(GetAbsoluteOrientation()) * retVector;
		retVector.Normalize();
	}

	return retVector;
}

inline double GetAABBOBBSeparation(vector vAxis, matrix<float, 4, 4> matRelativeRotationAbs, vector vTranslation,
	vector vAABBHalfVector, vector vOBBHalfVector)
{
	double separation = std::abs(vTranslation.dot(vAxis));
	separation -= std::abs(vAABBHalfVector.dot(vAxis)) + std::abs(((vector)(matRelativeRotationAbs * vOBBHalfVector)).dot(vAxis));

	return separation;
}

// TODO: Before done - clean everything up and create a proper transpose matrix function

CollisionManifold BoundingBox::CollideSAT(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	double minSeparationDistance = INFINITY;
	vector vAxis, vAxisTemp;
	double penetration = 0.0f;
	float separation = 0.0f;

	// Move test into space of this bounding box
	point ptBoxAOrigin = GetAbsoluteOrigin();
	vector vBoxAHV = GetHalfVector();
	quaternion qBoxAOrientation = GetAbsoluteOrientation();
	RotationMatrix matRotationA = RotationMatrix(qBoxAOrientation);
	auto matInverseRotationA = inverse(matRotationA);

	point ptBoxBOrigin = static_cast<BoundingBox>(rhs).GetAbsoluteOrigin();
	vector vBoxBHV = static_cast<BoundingBox>(rhs).GetHalfVector();
	quaternion qBoxBOrientation = static_cast<BoundingBox>(rhs).GetAbsoluteOrientation();
	RotationMatrix matRotationB = RotationMatrix(qBoxBOrientation);

	vector vAxesA[3], vAxesB[3];

	vAxesA[0] = vector::iVector(1.0f); vAxesA[1] = vector::jVector(1.0f); vAxesA[2] = vector::kVector(1.0f);

	vector vTranslationAB = matInverseRotationA * (ptBoxBOrigin - ptBoxAOrigin);
	auto matRelativeRotation = matInverseRotationA * matRotationB;
	auto matRelativeRotationAbs = absolute(matRelativeRotation);

	// Get Box B axes in A space
	for (int i = 0; i < 3; i++) {
		vAxesB[i] = matRelativeRotation * vAxesA[i];
	}

	for (int i = 0; i < 3; i++) {

		vAxisTemp = vAxesA[i];
		separation = GetAABBOBBSeparation(vAxisTemp, matRelativeRotationAbs, vTranslationAB, vBoxAHV, vBoxBHV);
		if (std::abs(separation) < minSeparationDistance) {
			minSeparationDistance = std::abs(separation);
			penetration = separation;
			vAxis = vAxisTemp;
		}

		vAxisTemp = vAxesB[i];
		separation = GetAABBOBBSeparation(vAxisTemp, matRelativeRotationAbs, vTranslationAB, vBoxAHV, vBoxBHV);
		if (std::abs(separation) < minSeparationDistance) {
			minSeparationDistance = std::abs(separation);
			penetration = separation;
			vAxis = vAxisTemp;
		}

		// Go through the cross product of each of the axes
		for (int j = 0; j < 3; j++) {

			// Ensure not same
			if (vAxesB[j] != vAxesA[i]) {

				vAxisTemp = vAxesA[i].cross(vAxesB[j]);

				separation = GetAABBOBBSeparation(vAxisTemp, matRelativeRotationAbs, vTranslationAB, vBoxAHV, vBoxBHV);
				if (std::abs(separation) < minSeparationDistance) {
					minSeparationDistance = std::abs(separation);
					penetration = separation;
					vAxis = vAxisTemp;
				}
			}
		}
	}

	vector vNormal = matRotationA * vAxis;
	//vector vNormal = vAxis;

	manifold.AddContactPoint(ptBoxAOrigin, vNormal, penetration, 1);
	
	return manifold;

	// Points vs Face

	// Find the incident vector - Find most negative dot prod
	vector vFaceVector;
	float bestDotProd = 0.0f;
	int bestAxis = 0;
	bool fNegative = false;
	float d;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			vector vTemp = matRotationA * vAxesB[i];

			if(j == 1) {
				vTemp = -1.0f * vTemp;
			}

			d = vAxis.dot(vTemp);

			if (d < bestDotProd) {

				bestDotProd = d;
				bestAxis = i;
				fNegative = (bool)(j);

				vFaceVector = vTemp;
			}
		}
	}

	// Get the incident face given the axis
	BoundingBox::face boxBFace = BoundingBox::face(bestAxis, fNegative, vBoxBHV);

	// TODO: formalize this
	// Apply rotation of B
	boxBFace.ApplyMatrix(matRotationB);

	// Apply A
	boxBFace.ApplyMatrix(matInverseRotationA);
	
	boxBFace.Translate(ptBoxBOrigin - ptBoxBOrigin);


	// Now that we have the face, clip it against our planes
	for (int i = 0; i < 4; i++) {
		float xAbsVal = std::abs(boxBFace.m_points[i].x());
		float yAbsVal = std::abs(boxBFace.m_points[i].y());
		float zAbsVal = std::abs(boxBFace.m_points[i].z());

		if (xAbsVal < vBoxAHV.x() &&
			yAbsVal < vBoxAHV.y() &&
			zAbsVal < vBoxAHV.z()) 
		{
			// point is in there, re-orient
			point ptContact = (matRotationA * boxBFace.m_points[i]) + ptBoxAOrigin;
			
			//point ptContact = (boxBFace.m_points[i]) + ptBoxAOrigin;
			
			penetration = -minSeparationDistance;
			
			vector vNormal = matRotationA * vAxis;

			// This will reverse the vector if needed
			if (vBoxAHV.dot(boxBFace.m_points[i]) < 0.0f) {
				vNormal = -1.0f * vNormal;
			}

			manifold.AddContactPoint(ptContact, vNormal, penetration, 1);
		}
	}

	return manifold;
}

BoundingBox::face::face() {
	// empty
}

BoundingBox::face::face(int axis, bool fNegative, vector vHalfVector) {
	switch (axis) {
		// x axis
		case 0: {
			float xVal = vHalfVector.x();
			if (fNegative)
				xVal *= -1.0f;

			m_points[0] = point(xVal, vHalfVector.y(), vHalfVector.z());
			m_points[1] = point(xVal, -vHalfVector.y(), vHalfVector.z());
			m_points[2] = point(xVal, vHalfVector.y(), -vHalfVector.z());
			m_points[3] = point(xVal, -vHalfVector.y(), -vHalfVector.z());

			if(fNegative)
				m_vNormal = vector::iVector(-1.0f);
			else
				m_vNormal = vector::iVector(1.0f);
		} break;

		// y axis
		case 1: {
			float yVal = vHalfVector.y();
			if (fNegative)
				yVal *= -1.0f;

			m_points[0] = point(-vHalfVector.x(), yVal, vHalfVector.z());
			m_points[1] = point(vHalfVector.x(), yVal, vHalfVector.z());
			m_points[2] = point(-vHalfVector.x(), yVal, -vHalfVector.z());
			m_points[3] = point(vHalfVector.x(), yVal, -vHalfVector.z());

			if (fNegative)
				m_vNormal = vector::jVector(-1.0f);
			else
				m_vNormal = vector::jVector(1.0f);
		} break;

		// z axis
		case 2: {
			float zVal = vHalfVector.z();
			if (fNegative)
				zVal *= -1.0f;

			m_points[0] = point(vHalfVector.x(), vHalfVector.y(), zVal);
			m_points[1] = point(-vHalfVector.x(), vHalfVector.y(), zVal);
			m_points[2] = point(vHalfVector.x(), -vHalfVector.y(), zVal);
			m_points[3] = point(-vHalfVector.x(), -vHalfVector.y(), zVal);

			if (fNegative)
				m_vNormal = vector::kVector(-1.0f);
			else
				m_vNormal = vector::kVector(1.0f);
		} break;
	}
}

RESULT BoundingBox::face::ApplyMatrix(matrix<float, 4, 4> mat) {
	for (int i = 0; i < 4; i++) {
		m_points[i] = mat * m_points[i];
	}

	m_vNormal = mat * m_vNormal;

	return R_PASS;
}

RESULT BoundingBox::face::Translate(vector vTranslation) {
	for (int i = 0; i < 4; i++) 
		m_points[i].translate(vTranslation);

	return R_PASS;
}

CollisionManifold BoundingBox::CollideBruteForce(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// Point vs Face
	// Do for both objects
	// TODO: Assumes OBB - can be optimized for AABB and OBB-AABB certainly
	for (int j = 0; j < 2; j++) {
		// Do this early to improve perf
		BoundingBox *pBoxA = (j == 0) ? this : &(static_cast<BoundingBox>(rhs));
		BoundingBox *pBoxB = (j == 0) ? &(static_cast<BoundingBox>(rhs)) : this;

		point ptBoxAOrigin = pBoxA->GetAbsoluteOrigin();
		quaternion qBoxAOrientation = pBoxA->GetAbsoluteOrientation();

		point ptBoxBOrigin = pBoxB->GetAbsoluteOrigin();
		quaternion qBoxBOrientation = pBoxB->GetAbsoluteOrientation();


		// Point - Face Detection
		for (int i = 0; i < 8; i++) {
			BoundingBox::BoxPoint boxPoint = (BoundingBox::BoxPoint)(i);
			point ptBox = pBoxB->GetBoxPoint(boxPoint);
			ptBox = (point)(inverse(RotationMatrix(qBoxAOrientation)) * (ptBox - ptBoxAOrigin));

			point ptMax = pBoxA->GetHalfVector();
			point ptMin = pBoxA->GetHalfVector() * -1.0f;

			// Early out as quickly as possible
			if ((ptBox <= ptMax && ptBox >= ptMin) == false) {
				continue;
			}

			float closestX = std::max(ptMin.x(), std::min(ptBox.x(), ptMax.x()));
			float closestY = std::max(ptMin.y(), std::min(ptBox.y(), ptMax.y()));
			float closestZ = std::max(ptMin.z(), std::min(ptBox.z(), ptMax.z()));

			point ptClosestPoint = point(closestX, closestY, closestZ);

			// TODO:  There is a bug with this that needs to be resolved
			// Basically, if the verts are exactly equal to the width/height/depth or if
			// they are within the depth threshold (0.001) where the depth of penetration
			// is greater than this offet, then the correct box-face is not identified
			// That can be resolved with appropriate implementation of SAT to determine
			// the correct plane of overlap

			// The EPSILON hack was removed as it was creating arbitrary jumps
			// there should be a separate test done for situations where the penetration is
			// exactly zero 

			BoundingBox::BoxFace boxFace;
			double distanceX = pBoxA->GetHalfVector().x() - std::abs(ptBox.x());
			double distanceY = pBoxA->GetHalfVector().y() - std::abs(ptBox.y());;
			double distanceZ = pBoxA->GetHalfVector().z() - std::abs(ptBox.z());;

			double minDistance = std::numeric_limits<double>::max();

			if ((distanceX < minDistance)) {
				minDistance = distanceX;

				if (ptBox.x() > 0.0f)
					boxFace = BoundingBox::BoxFace::RIGHT;
				else
					boxFace = BoundingBox::BoxFace::LEFT;
			}
			if ((distanceY < minDistance)) {
				minDistance = distanceY;

				if (ptBox.y() > 0.0f)
					boxFace = BoundingBox::BoxFace::TOP;
				else
					boxFace = BoundingBox::BoxFace::BOTTOM;
			}
			if ((distanceZ < minDistance)) {
				minDistance = distanceZ;

				if (ptBox.z() > 0.0f)
					boxFace = BoundingBox::BoxFace::FRONT;
				else
					boxFace = BoundingBox::BoxFace::BACK;
			}

			ptClosestPoint = (RotationMatrix(qBoxAOrientation) * ptClosestPoint) + ptBoxAOrigin;

			vector vNormal = pBoxA->GetBoxFaceNormal(boxFace);
			vNormal.Normalize();

			point ptContact = ptClosestPoint;
			float penetration = std::abs(minDistance);

			// Because we're working with both objects 
			// Need to flip for the second pass
			if (j == 1) {
				vNormal = vNormal * -1.0f;
				penetration *= -1.0f;
			}

			if (vNormal.IsValid() == false) {
				int a = 5;
			}

			manifold.AddContactPoint(ptContact, vNormal, penetration, 1);
		}
	}

	// Skip the rest if we found four points already
	if (manifold.NumContacts() == 4) {
		return manifold;
	}

	//for (int j = 0; j < 2; j++) {
		// Do this early to improve perf
		int j = 0;

		BoundingBox *pBoxA = (j == 0) ? this : &(static_cast<BoundingBox>(rhs));
		BoundingBox *pBoxB = (j == 0) ? &(static_cast<BoundingBox>(rhs)) : this;

		point ptBoxAOrigin = pBoxA->GetAbsoluteOrigin();
		quaternion qBoxAOrientation = pBoxA->GetAbsoluteOrientation();

		point ptBoxBOrigin = pBoxB->GetAbsoluteOrigin();
		quaternion qBoxBOrientation = pBoxB->GetAbsoluteOrientation();

		// Test all edges
		double minDistanceTemp = std::numeric_limits<double>::infinity();
		vector vNormalTemp;
		point ptA, ptB;

		for (int i = 0; i < 12; i++) {
			BoundingBox::BoxEdge boxEdgeB = (BoundingBox::BoxEdge)(i);
			line lineBoxEdgeB = pBoxB->GetBoxEdge(boxEdgeB);

			for (int k = 0; k < 12; k++) {
				BoundingBox::BoxEdge boxEdgeA = (BoundingBox::BoxEdge)(k);
				line lineBoxEdgeA = pBoxA->GetBoxEdge(boxEdgeA);

				double lineDistance = lineBoxEdgeA.Distance(lineBoxEdgeB);

				if (std::abs(lineDistance) < std::abs(minDistanceTemp)) {
					minDistanceTemp = lineDistance;
					vNormalTemp = lineBoxEdgeA.GetVector().cross(lineBoxEdgeB.GetVector());
					vNormalTemp = vNormalTemp * -1.0f;
					vNormalTemp.Normalize();

					// Find the closest respective points
					// TODO: Replace with the code in line.cpp
					// https://en.wikipedia.org/wiki/Skew_lines#Distance_between_two_skew_lines
					vector vNormalA = lineBoxEdgeA.GetVector().cross(vNormalTemp);
					vector vNormalB = lineBoxEdgeB.GetVector().cross(vNormalTemp);

					double dotProdDiffNormalA = (lineBoxEdgeB.a() - lineBoxEdgeA.a()).dot(vNormalB);
					double dotProdDiffNormalB = (lineBoxEdgeA.a() - lineBoxEdgeB.a()).dot(vNormalA);

					double dotProdNormalA = lineBoxEdgeA.GetVector().dot(vNormalB);
					double dotProdNormalB = lineBoxEdgeB.GetVector().dot(vNormalA);

					double tA = dotProdDiffNormalA / dotProdNormalA;
					double tB = dotProdDiffNormalB / dotProdNormalB;

					// Getting the valid point
					ptA = lineBoxEdgeA.a() + lineBoxEdgeA.GetVector() * tA;
					ptB = lineBoxEdgeB.a() + lineBoxEdgeB.GetVector() * tB;
				}
			}
		}

		// TODO: Eliminate the bottom code, replace with the above test
		// TODO: The above code is not always returning the right thing

		// Edge - Edge Detection
		for (int i = 0; i < 12; i++) {
			//BoundingBox::BoxEdge boxEdge = BoundingBox::BoxEdge::LEFT_NEAR;
			BoundingBox::BoxEdge boxEdgeB = (BoundingBox::BoxEdge)(i);
			line lineBoxEdgeB = pBoxB->GetBoxEdge(boxEdgeB);			

			lineBoxEdgeB.Translate(ptBoxAOrigin * -1.0f);
			lineBoxEdgeB.ApplyMatrix(inverse(RotationMatrix(qBoxAOrientation)));
			vector vRay = lineBoxEdgeB.GetVector();

			// We can now test intersection as if it's an AABB
			point ptMax = pBoxA->GetHalfVector();
			point ptMin = pBoxA->GetHalfVector() * -1.0f;

			double tNear = -INFINITY;
			double tFar = INFINITY;
			bool fMiss = false;

			for (int i = 0; i < 3; i++) {

				if (std::abs(vRay(i)) < DREAM_EPSILON) {
					if (ptMin(i) - lineBoxEdgeB.a()(i) > 0 || ptMax(i) - lineBoxEdgeB.a()(i) < 0) {
						fMiss = true;
						continue;
					}
				}
				else {
					double t1 = (ptMin(i) - lineBoxEdgeB.a()(i)) / vRay(i);
					double t2 = (ptMax(i) - lineBoxEdgeB.a()(i)) / vRay(i);

					double tMin = std::min(t1, t2);
					double tMax = std::max(t1, t2);

					if (tMin > tNear)
						tNear = tMin;

					if (tMax < tFar)
						tFar = tMax;

					if (tNear > tFar || tFar < 0) {
						fMiss = true;
						continue;
					}
				}
			}

			if (fMiss)
				continue;

			// NOTE: This is only designed to find an edge edge collision which means that 
			// there should be an entry-exit point
			if ((tNear >= 0 && tNear <= 1) && (tFar >= 0 && tFar <= 1)) {
				point ptEdgeMin = lineBoxEdgeB.a() + (vRay * tNear);
				point ptEdgeMax = lineBoxEdgeB.a() + (vRay * tFar);
				point ptEdgeMid = point::midpoint(ptEdgeMin, ptEdgeMax);

				int weight = 1;

				// Test for situations where the magnitude of min-max is equivalent 
				// to a dimension of one of the boxes
				for (int i = 0; i < 3; i++) {
					auto axisMagnitude = (ptEdgeMax(i) - ptEdgeMin(i));

					if (axisMagnitude - (pBoxA->GetHalfVector()(i) * 2.0f) >= -DREAM_EPSILON) {
						weight = 2;
					}
					else if (axisMagnitude - (pBoxB->GetHalfVector()(i) * 2.0f) >= -DREAM_EPSILON) {
						weight = 2;
					}
				}

				double minDistance1 = std::numeric_limits<double>::max();
				double minDistance2 = std::numeric_limits<double>::max();

				double distanceX = pBoxA->GetHalfVector().x() - std::abs(ptEdgeMid.x());
				double distanceXabs = std::abs(distanceX);

				double distanceY = pBoxA->GetHalfVector().y() - std::abs(ptEdgeMid.y());;
				double distanceYabs = std::abs(distanceY);

				double distanceZ = pBoxA->GetHalfVector().z() - std::abs(ptEdgeMid.z());;
				double distanceZabs = std::abs(distanceZ);

				double penetration = 0.0f;

				// Figure out BoxEdge A
				
				vector vNormal;

				if (distanceXabs > distanceYabs && distanceXabs > distanceZabs && (vRay.z() != 0.0f || vRay.y() != 0.0f)) {
					// We're in the YZ plane
					vNormal = vector(0.0f, -vRay.z(), vRay.y());
					vNormal.Normalize();

					// Calculate penetration
					if (distanceYabs < distanceZabs) 
						penetration = (vNormal.y() > DREAM_EPSILON) ? distanceYabs / vNormal.y() : distanceYabs;
					else 
						penetration = (vNormal.z() > DREAM_EPSILON) ? distanceZabs / vNormal.z() : distanceZabs;
				}
				else if (distanceYabs > distanceXabs && distanceYabs > distanceZabs && (vRay.x() != 0.0f || vRay.z() != 0.0f)) {
					// We're in the XZ plane
					vNormal = vector(-vRay.z(), 0.0f, vRay.x());
					vNormal.Normalize();

					// Calculate penetration
					if (distanceXabs < distanceZabs)
						penetration = (vNormal.x() > DREAM_EPSILON) ? distanceXabs / vNormal.x() : distanceXabs;
					else
						penetration = (vNormal.z() > DREAM_EPSILON) ? distanceZabs / vNormal.z() : distanceZabs;
				}
				else if (distanceZabs > distanceXabs && distanceZabs > distanceYabs && (vRay.x() != 0.0f || vRay.y() != 0.0f)) {
					// We're in the XY plane
					vNormal = vector(vRay.y(), -vRay.x(), 0.0f);
					vNormal.Normalize();

					// Calculate penetration
					if (distanceYabs < distanceXabs)
						penetration = (vNormal.y() > DREAM_EPSILON) ? distanceYabs / vNormal.y() : distanceYabs;
					else
						penetration = (vNormal.x() > DREAM_EPSILON) ? distanceXabs / vNormal.x() : distanceXabs;
				}

				//CBA((vNormal.IsValid()), "Invalid Normal");

				// Invalid normal, must be parallel 
				// TODO: This needs to be handled
				if (vNormal.magnitude() < 1.0f) {
					//vNormal = inverse(RotationMatrix(pBoxA->GetOrientation())) * vAxis;
					//vNormal = vAxis;
					//vNormal.Print("Normal");
					//penetration = std::min(minAxisDistance, std::min(distanceXabs, std::min(distanceYabs, distanceZabs)));
					//penetration = std::min(distanceXabs, std::min(distanceYabs, distanceZabs));
					//break;
				}

				//DEBUG_LINEOUT("%f %f %f", distanceXabs, distanceYabs, distanceZabs);
				//DEBUG_LINEOUT("%f", penetration);

				point ptClosestPoint = (RotationMatrix(qBoxAOrientation) * ptEdgeMid) + ptBoxAOrigin;				

				point ptContact = ptClosestPoint;
				point ptBoxBOriginRefA = inverse(RotationMatrix(qBoxAOrientation)) * (ptBoxBOrigin - ptBoxAOrigin);

				if (vNormal.IsValid() == false) {
					int a = 5;
				}

				vector vNormalOriented = RotationMatrix(qBoxAOrientation) * vNormal;
				//vNormal = vNormal * -1.0f;
				vNormalOriented.Normalize();

				//if (j == 1) {
					vNormalOriented = vNormalOriented * -1.0f;
					penetration *= -1.0f;
				//}

				// This is a bit of a hack, but will ensure normal facing out
				if (vNormalOriented.dot(ptContact - ptBoxAOrigin) < 0) {
					vNormalOriented = vNormalOriented * -1.0f;
				}

				//manifold.Clear();
				if (weight == 2) {
					point ptContactPointA = (RotationMatrix(qBoxAOrientation) * ptEdgeMin) + ptBoxAOrigin;
					point ptContactPointB = (RotationMatrix(qBoxAOrientation) * ptEdgeMax) + ptBoxAOrigin;

					// TODO: THIS IS NOT GENERAL
					//vNormalOriented = vNormalTemp;

					manifold.AddContactPoint(ptContactPointA, vNormalOriented, penetration, 1);
					manifold.AddContactPoint(ptContactPointB, vNormalOriented, penetration, 1);
				}
				else {

					// TODO: THIS IS NOT GENERAL
					//vNormalOriented = vNormalTemp;

					manifold.AddContactPoint(ptContact, vNormalOriented, penetration, 1);
				}
			}
		}

	/*
		if (manifold.NumContacts() > 0) {
			//return manifold;
			//break;
		}
	}*/
	
	return manifold;
}

// https://tavianator.com/fast-branchless-raybounding-box-intersections/
bool BoundingBox::Intersect(const ray& r) {
	double tmin = -INFINITY, tmax = INFINITY;

	// Rotate the ray by the Rotation Matrix
	// Get origin in reference to object
	ray adjRay = r;

	if (m_type == Type::OBB) {
		adjRay.vDirection() = inverse(RotationMatrix(GetAbsoluteOrientation())) * r.GetVector();
		adjRay.ptOrigin() = GetAbsoluteOrigin() - (point)(inverse(RotationMatrix(GetAbsoluteOrientation())) * (GetAbsoluteOrigin() - r.GetOrigin()));
	}

	point ptMin = (GetAbsoluteOrigin() - GetHalfVector());
	point ptMax = (GetAbsoluteOrigin() + GetHalfVector());

	if (adjRay.ptOrigin() < ptMax && adjRay.ptOrigin() > ptMin)
		return true;

	for (int i = 0; i < 3; i++) {
		double t1 = (ptMin(i) - adjRay.ptOrigin()(i)) / adjRay.vDirection()(i);
		double t2 = (ptMax(i) - adjRay.ptOrigin()(i)) / adjRay.vDirection()(i);

		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));
	}

	return (tmax >= tmin) && (tmax >= 0) && (tmin >= 0);
}

CollisionManifold BoundingBox::Collide(const ray &rCast) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, nullptr);
	double tmin = -INFINITY, tmax = INFINITY;

	// Rotate the ray by the Rotation Matrix
	// Get origin in reference to object
	ray adjRay;

	if (m_type == Type::OBB) {
		adjRay.vDirection() = inverse(RotationMatrix(GetAbsoluteOrientation())) * rCast.GetVector();
		adjRay.ptOrigin() = GetAbsoluteOrigin() - (point)(inverse(RotationMatrix(GetAbsoluteOrientation())) * (GetAbsoluteOrigin() - rCast.GetOrigin()));
	}

	point ptMin = (GetAbsoluteOrigin() - GetHalfVector());
	point ptMax = (GetAbsoluteOrigin() + GetHalfVector());

	for (int i = 0; i < 3; i++) {
		double t1 = (ptMin(i) - adjRay.ptOrigin()(i)) / adjRay.vDirection()(i);
		double t2 = (ptMax(i) - adjRay.ptOrigin()(i)) / adjRay.vDirection()(i);

		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));
	}

	// TODO: Do the normals
	if (tmax == tmin) {
		point ptContactMin = rCast.GetOrigin() + rCast.GetVector() * tmin;
		vector vNormal = vector();

		manifold.AddContactPoint(ptContactMin, vNormal, 0.0f, 1);
	}
	else if (tmax > tmin) {
		point ptContactMin = rCast.GetOrigin() + rCast.GetVector() * tmin;
		point ptContactMax = rCast.GetOrigin() + rCast.GetVector() * tmax;
		vector vNormalMin = vector();
		vector vNormalMax = vector();

		manifold.AddContactPoint(ptContactMin, vNormalMin, 0.0f, 1);
		manifold.AddContactPoint(ptContactMax, vNormalMax, 0.0f, 1);
	}

	return manifold;
}

bool BoundingBox::Intersect(const BoundingQuad& rhs) {
	return static_cast<BoundingQuad>(rhs).Intersect(*this);
}

bool BoundingBox::Intersect(const BoundingPlane& rhs) {
	return static_cast<BoundingPlane>(rhs).Intersect(*this);
}

//bool Intersect(const point& pt) {
bool BoundingBox::Intersect(point& pt) {
	point ptMin = GetMinPoint();
	point ptMax = GetMaxPoint();

	if ((pt > ptMin) && (pt < ptMax)) {
		return true;
	}
	else {
		return false;
	}
}

CollisionManifold BoundingBox::Collide(const BoundingQuad& rhs) {
	return static_cast<BoundingQuad>(rhs).Collide(*this);
}

RESULT BoundingBox::SetHalfVector(vector vHalfVector) {
	m_vHalfSize = vHalfVector;
	return R_PASS;
}

RESULT BoundingBox::SetMaxPointFromOrigin(point ptMax) {
	m_vHalfSize = (ptMax - GetOrigin());
	//m_vHalfSize = ptMax;
	return R_PASS;
}

// TODO: Replace with Transform to AABB function - in volume as well
double BoundingBox::GetWidth() {
	if (m_type == Type::AABB && m_pParent != nullptr) {
		//RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double min = std::numeric_limits<double>::max();
		double max = std::numeric_limits<double>::min();

		for (int i = 0; i < 8; i++) {
			//point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			point pt = GetBoxPoint((BoxPoint)(i));

			if (pt.x() > max)
				max = pt.x();
			else if (pt.x() < min)
				min = pt.x();
		}

		double width = (max - min);
		return static_cast<double>(width);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.x() * 2.0f);
}

double BoundingBox::GetHalfVectorWidth(bool fAbsolute) {
	double retVal = m_vHalfSize.x();

	if (fAbsolute) {
		retVal *= GetScale().x();
	}
	
	return retVal;
}

double BoundingBox::GetHalfVectorHeight(bool fAbsolute) {
	double retVal = m_vHalfSize.y();

	if (fAbsolute) {
		retVal *= GetScale().y();
	}

	return retVal;
}

double BoundingBox::GetHalfVectorLength(bool fAbsolute) {
	double retVal = m_vHalfSize.z();

	if (fAbsolute) {
		retVal *= GetScale().z();
	}

	return retVal;
}

double BoundingBox::GetHeight() {
	if (m_type == Type::AABB && m_pParent != nullptr) {
		//RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double min = std::numeric_limits<double>::max();
		double max = std::numeric_limits<double>::min();

		for (int i = 0; i < 8; i++) {
			//point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			point pt = GetBoxPoint((BoxPoint)(i));
			
			if (pt.y() > max)
				max = pt.y();
			else if (pt.y() < min)
				min = pt.y();
		}

		double height = (max - min);
		return static_cast<double>(height);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.y() * 2.0f);
}

double BoundingBox::GetLength() {
	if (m_type == Type::AABB && m_pParent != nullptr) {
		//RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double min = std::numeric_limits<double>::max();
		double max = std::numeric_limits<double>::min();

		for (int i = 0; i < 8; i++) {
			//point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			point pt = GetBoxPoint((BoxPoint)(i));
			
			if (pt.z() > max)
				max = pt.z();
			else if (pt.z() < min)
				min = pt.z();
		}

		double length = (max - min);
		return static_cast<double>(length);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.z() * 2.0f);
}

// TODO: Why do we need to invert the point?
point BoundingBox::GetMinPoint(bool fAbsolute) {
	return (GetOrigin() - GetHalfVector(fAbsolute));
}

point BoundingBox::GetMaxPoint(bool fAbsolute) {
	return (GetOrigin() + GetHalfVector(fAbsolute));
}

point BoundingBox::GetMinPointOriented(bool fAbsolute) {
	if (m_type == Type::AABB) {
		return GetMinPoint();
	}
	else {
		auto aabb = GetBoundingAABB();
		return GetOrigin() - aabb.m_vHalfSize;
	}
}

point BoundingBox::GetMaxPointOriented(bool fAbsolute) {
	if (m_type == Type::AABB) {
		return GetMaxPoint();
	}
	else {
		auto aabb = GetBoundingAABB();
		return GetOrigin() + aabb.m_vHalfSize;
	}
}

// Original reference: http://www.realtimerendering.com/resources/GraphicsGems/gems/TransBox.c
BoundingBox BoundingBox::GetBoundingAABB() {
	point ptMinA = GetMinPoint();
	point ptMaxA = GetMaxPoint();

	point ptMinB = GetOrigin(); 
	point ptMaxB = GetOrigin();

	auto matRot = RotationMatrix(GetOrientation());

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			auto a = matRot(i, j) * ptMinA(j);
			auto b = matRot(i, j) * ptMaxA(j);
			if (a < b) {
				ptMinB(i) += a;
				ptMaxB(i) += b;
			}
			else {
				ptMinB(i) += b;
				ptMaxB(i) += a;
			}
		}
	}

	return BoundingBox(nullptr, Type::AABB, ptMinB, ptMaxB);
}

point BoundingBox::GetBoxPoint(BoxPoint ptType, bool fOriented) {
	point retPoint = point(GetHalfVector());
	switch (ptType) {
		case BoxPoint::TOP_RIGHT_FAR: {
			// nothing 
		} break;

		case BoxPoint::TOP_RIGHT_NEAR: {
			retPoint.z() *= -1.0f;
		} break;

		case BoxPoint::TOP_LEFT_FAR: {
			retPoint.x() *= -1.0f;
		} break;

		case BoxPoint::TOP_LEFT_NEAR: {
			retPoint.x() *= -1.0f; 
			retPoint.z() *= -1.0f;
		} break;

		case BoxPoint::BOTTOM_RIGHT_FAR: {
			retPoint.y() *= -1;
		} break;
		
		case BoxPoint::BOTTOM_RIGHT_NEAR: {
			retPoint.y() *= -1.0f; 
			retPoint.z() *= -1.0f;
		} break;

		case BoxPoint::BOTTOM_LEFT_FAR: {
			retPoint.x() *= -1.0f;
			retPoint.y() *= -1.0f;
		} break;

		case BoxPoint::BOTTOM_LEFT_NEAR: {
			retPoint.x() *= -1.0f;
			retPoint.y() *= -1.0f;
			retPoint.z() *= -1.0f;
		} break;
	}

	// Transform point accordingly
	if (fOriented) {
		retPoint = RotationMatrix(GetAbsoluteOrientation()) * retPoint;
		retPoint = retPoint + GetAbsoluteOrigin();
	}

	return retPoint;
}

BoundingBox::face BoundingBox::GetFace(BoxFace faceType, bool fOriented) {
	BoundingBox::face faceBox;

	faceBox.m_type = faceType;

	switch (faceType) {
		case BoxFace::TOP: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_RIGHT_FAR, fOriented);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_LEFT_FAR, fOriented);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR, fOriented);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::TOP_LEFT_NEAR, fOriented);
		} break;

		case BoxFace::BOTTOM: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR, fOriented);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR, fOriented);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR, fOriented);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR, fOriented);
		} break;

		case BoxFace::LEFT: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_LEFT_NEAR, fOriented);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_LEFT_FAR, fOriented);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR, fOriented);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR, fOriented);
		} break;

		case BoxFace::RIGHT: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR, fOriented);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_RIGHT_FAR, fOriented);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR, fOriented);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR, fOriented);
		} break;

		case BoxFace::FRONT: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_LEFT_NEAR, fOriented);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR, fOriented);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR, fOriented);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR, fOriented);
		} break;

		case BoxFace::BACK: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_LEFT_FAR, fOriented);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_RIGHT_FAR, fOriented);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR, fOriented);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR, fOriented);
		} break;
	}

	faceBox.m_vNormal = GetBoxFaceNormal(faceType, fOriented);

	return faceBox;
}

vector BoundingBox::GetBoxFaceNormal(BoxFace faceType, bool fOriented) {
	vector vNormal;

	switch (faceType) {
		case BoxFace::TOP: vNormal = vector::jVector(1.0f); break;
		case BoxFace::BOTTOM: vNormal = vector::jVector(-1.0f); break;
		case BoxFace::RIGHT: vNormal = vector::iVector(1.0f); break;
		case BoxFace::LEFT: vNormal = vector::iVector(-1.0f); break;
		case BoxFace::FRONT: vNormal = vector::kVector(1.0f); break;
		case BoxFace::BACK: vNormal = vector::kVector(-1.0f); break;
	}

	// Rotate if needed 
	if (fOriented) {
		vNormal = RotationMatrix(GetAbsoluteOrientation()) * vNormal;
		vNormal.Normalize();
	}

	return vNormal;
}

line BoundingBox::GetBoxEdge(BoxEdge edgeType) {
	line lineEdge;

	switch (edgeType) {

		case BoxEdge::TOP_RIGHT: {
			lineEdge = line(GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR), 
							GetBoxPoint(BoxPoint::TOP_RIGHT_FAR));
		} break;

		case BoxEdge::TOP_LEFT: {
			lineEdge = line(GetBoxPoint(BoxPoint::TOP_LEFT_NEAR), 
							GetBoxPoint(BoxPoint::TOP_LEFT_FAR)); 
		} break;
	
		case BoxEdge::TOP_NEAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::TOP_LEFT_NEAR), 
							GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR)); 
		} break;
	
		case BoxEdge::TOP_FAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::TOP_LEFT_FAR), 
							GetBoxPoint(BoxPoint::TOP_RIGHT_FAR)); 
		} break;
	
		case BoxEdge::BOTTOM_RIGHT: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR), 
							GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR)); 
		} break;
	
		case BoxEdge::BOTTOM_LEFT: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR), 
							GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR)); 
		} break;
	
		case BoxEdge::BOTTOM_NEAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR), 
							GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR)); 
		} break;
	
		case BoxEdge::BOTTOM_FAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR), 
							GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR)); 
		} break;
	
		case BoxEdge::LEFT_NEAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR), 
							GetBoxPoint(BoxPoint::TOP_LEFT_NEAR)); 
		} break;

		case BoxEdge::LEFT_FAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR), 
							GetBoxPoint(BoxPoint::TOP_LEFT_FAR)); 
		} break;

		case BoxEdge::RIGHT_NEAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR), 
							GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR)); 
		} break;

		case BoxEdge::RIGHT_FAR: {
			lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR), 
							GetBoxPoint(BoxPoint::TOP_RIGHT_FAR)); 
		} break;

	}

	return lineEdge;
}