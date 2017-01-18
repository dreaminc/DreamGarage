#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingQuad.h"
#include <algorithm>

#include "VirtualObj.h"
#include "PhysicsEngine/CollisionManifold.h"

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

bool BoundingBox::Intersect(const BoundingSphere& rhs) {

	point ptSphereOrigin = static_cast<BoundingSphere>(rhs).GetOrigin();
	point ptMax = GetMaxPoint();
	point ptMin = GetMinPoint();

	if (m_type == Type::OBB) {
		//ptSphereOrigin = GetOrigin() - (point)(inverse(RotationMatrix(GetOrientation())) * (GetOrigin() - ptSphereOrigin));
		ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation())) * (ptSphereOrigin - GetOrigin()));
		ptMax = m_vHalfSize;
		ptMin = m_vHalfSize * -1.0f;
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

bool BoundingBox::Intersect(const BoundingBox& rhs) {
	if (m_type == Type::AABB) {
		point ptMaxA = GetOrigin() + GetHalfVector();
		point ptMinA = GetOrigin() - GetHalfVector();

		point ptMaxB = const_cast<BoundingBox&>(rhs).GetOrigin() + static_cast<BoundingBox>(rhs).GetHalfVector();
		point ptMinB = const_cast<BoundingBox&>(rhs).GetOrigin() - static_cast<BoundingBox>(rhs).GetHalfVector();

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

CollisionManifold BoundingBox::Collide(const BoundingBox& rhs) {
	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	// Point vs Face
	// Do for both objects
	// TODO: Assumes OBB - can be optimized for AABB and OBB-AABB certainly
	for (int j = 0; j < 2; j++) {
		// Do this early to improve perf
		BoundingBox *pBoxA = (j == 0) ? this : &(static_cast<BoundingBox>(rhs));
		BoundingBox *pBoxB = (j == 0) ? &(static_cast<BoundingBox>(rhs)) : this;

		// Point - Face Detection
		for (int i = 0; i < 8; i++) {
			BoundingBox::BoxPoint boxPoint = (BoundingBox::BoxPoint)(i);
			point ptBox = pBoxB->GetBoxPoint(boxPoint);
			ptBox = (point)(inverse(RotationMatrix(pBoxA->GetOrientation())) * (ptBox - pBoxA->GetOrigin()));

			point ptMax = pBoxA->m_vHalfSize;
			point ptMin = pBoxA->m_vHalfSize * -1.0f;

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

			// The EPSILON hack below helps a bit - as it will disqualify a distance
			// too low to be reasonable within the current constraints

			BoundingBox::BoxFace boxFace;
			double distanceX = pBoxA->m_vHalfSize.x() - std::abs(ptBox.x());
			double distanceY = pBoxA->m_vHalfSize.y() - std::abs(ptBox.y());;
			double distanceZ = pBoxA->m_vHalfSize.z() - std::abs(ptBox.z());;

			double minDistance = std::numeric_limits<double>::max();

			if ((distanceX < minDistance) && distanceX > DREAM_EPSILON) {
				minDistance = distanceX;

				if (ptBox.x() > 0.0f)
					boxFace = BoundingBox::BoxFace::RIGHT;
				else
					boxFace = BoundingBox::BoxFace::LEFT;
			}
			if ((distanceY < minDistance) && distanceY > DREAM_EPSILON) {
				minDistance = distanceY;

				if (ptBox.y() > 0.0f)
					boxFace = BoundingBox::BoxFace::TOP;
				else
					boxFace = BoundingBox::BoxFace::BOTTOM;
			}
			if ((distanceZ < minDistance) && distanceZ > DREAM_EPSILON) {
				minDistance = distanceZ;

				if (ptBox.z() > 0.0f)
					boxFace = BoundingBox::BoxFace::FRONT;
				else
					boxFace = BoundingBox::BoxFace::BACK;
			}

			ptClosestPoint = (RotationMatrix(pBoxA->GetOrientation()) * ptClosestPoint) + pBoxA->GetOrigin();

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

			manifold.AddContactPoint(ptContact, vNormal, penetration);
		}
	}

	if (manifold.NumContacts() > 0)
		return manifold;

	for (int j = 0; j < 2; j++) {
		// Do this early to improve perf
		BoundingBox *pBoxA = (j == 0) ? this : &(static_cast<BoundingBox>(rhs));
		BoundingBox *pBoxB = (j == 0) ? &(static_cast<BoundingBox>(rhs)) : this;

		// Edge - Edge Detection
		for (int i = 0; i < 12; i++) {
			//BoundingBox::BoxEdge boxEdge = BoundingBox::BoxEdge::LEFT_NEAR;
			BoundingBox::BoxEdge boxEdge = (BoundingBox::BoxEdge)(i);
			line lineBoxEdge = pBoxB->GetBoxEdge(boxEdge);

			lineBoxEdge.Translate(pBoxA->GetOrigin() * -1.0f);
			lineBoxEdge.ApplyMatrix(inverse(RotationMatrix(pBoxA->GetOrientation())));
			vector vRay = lineBoxEdge.GetVector();

			// We can now test intersection as if it's an AABB
			point ptMax = pBoxA->m_vHalfSize;
			point ptMin = pBoxA->m_vHalfSize * -1.0f;

			double tNear = -INFINITY;
			double tFar = INFINITY;
			bool fMiss = false;

			for (int i = 0; i < 3; i++) {
				double t1 = (ptMin(i) - lineBoxEdge.a()(i)) / vRay(i);
				double t2 = (ptMax(i) - lineBoxEdge.a()(i)) / vRay(i);

				double tMin = std::min(t1, t2);
				double tMax = std::max(t1, t2);

				if (tMin > tNear)
					tNear = tMin;

				if (tMax < tFar)
					tFar = tMax;

				if (tNear > tFar || tFar < 0)
					fMiss = true;
			}

			if (fMiss)
				continue;

			// NOTE: This is only designed to find an edge edge collision which means that 
			// there should be an entry-exit point
			if ((tNear >= 0 && tNear <= 1) && (tFar >= 0 && tFar <= 1)) {
				point ptEdgeMin = lineBoxEdge.a() + (vRay * tNear);
				point ptEdgeMax = lineBoxEdge.a() + (vRay * tFar);
				point ptEdgeMid = point::midpoint(ptEdgeMin, ptEdgeMax);

				double minDistance1 = std::numeric_limits<double>::max();
				double minDistance2 = std::numeric_limits<double>::max();

				double distanceX = m_vHalfSize.x() - std::abs(ptEdgeMid.x());
				double distanceY = m_vHalfSize.y() - std::abs(ptEdgeMid.y());;
				double distanceZ = m_vHalfSize.z() - std::abs(ptEdgeMid.z());;

				if (distanceX < minDistance1) {
					minDistance2 = minDistance1;
					minDistance1 = distanceX;
				}
				else if (distanceX < minDistance2) {
					minDistance2 = distanceX;
				}

				if (distanceY < minDistance1) {
					minDistance2 = minDistance1;
					minDistance1 = distanceY;
				}
				else if (distanceY < minDistance2) {
					minDistance2 = distanceY;
				}

				if (distanceZ < minDistance1) {
					minDistance2 = minDistance1;
					minDistance1 = distanceZ;
				}
				else if (distanceZ < minDistance2) {
					minDistance2 = distanceZ;
				}

				point ptClosestPoint = (RotationMatrix(pBoxA->GetOrientation()) * ptEdgeMid) + pBoxA->GetOrigin();

				vector vNormal = vector(ptEdgeMid);
				vNormal = RotationMatrix(pBoxA->GetOrientation()) * vNormal;
				vNormal.Normalize();

				point ptContact = ptClosestPoint;

				// TODO: this is wrong
				float penetration = std::sqrt((minDistance1 * minDistance1) + (minDistance2 * minDistance2));

				if (j == 1) {
					vNormal = vNormal * -1.0f;
					penetration *= -1.0f;
				}

				manifold.AddContactPoint(ptContact, vNormal, penetration);
			}
		}

		if (manifold.NumContacts() > 0)
			return manifold;
	}
	
	return manifold;
}

// https://tavianator.com/fast-branchless-raybounding-box-intersections/
bool BoundingBox::Intersect(const ray& r) {
	double tmin = -INFINITY, tmax = INFINITY;

	// Rotate the ray by the Rotation Matrix
	// Get origin in reference to object
	ray adjRay;

	if (m_type == Type::OBB) {
		adjRay.vDirection() = inverse(RotationMatrix(GetOrientation())) * r.GetVector();
		adjRay.ptOrigin() = GetOrigin() - (point)(inverse(RotationMatrix(GetOrientation())) * (GetOrigin() - r.GetOrigin()));
	}

	point ptMin = GetMinPoint();
	point ptMax = GetMaxPoint();

	for (int i = 0; i < 3; i++) {
		double t1 = (ptMin(i) - adjRay.ptOrigin()(i)) / adjRay.vDirection()(i);
		double t2 = (ptMax(i) - adjRay.ptOrigin()(i)) / adjRay.vDirection()(i);

		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));
	}

	return (tmax >= tmin);
}

vector BoundingBox::GetBoxFaceNormal(BoxFace faceType) {
	vector vNormal;

	switch (faceType) {
	case BoxFace::TOP: vNormal = vector::jVector(1.0f); break;
	case BoxFace::BOTTOM: vNormal = vector::jVector(-1.0f); break;
	case BoxFace::RIGHT: vNormal = vector::iVector(1.0f); break;
	case BoxFace::LEFT: vNormal = vector::iVector(-1.0f); break;
	case BoxFace::FRONT: vNormal = vector::kVector(1.0f); break;
	case BoxFace::BACK: vNormal = vector::kVector(-1.0f); break;
	}

	vNormal = RotationMatrix(GetOrientation()) * vNormal;
	vNormal.Normalize();

	return vNormal;
}

CollisionManifold BoundingBox::Collide(const BoundingSphere& rhs) {
	point ptSphereOrigin = static_cast<BoundingSphere>(rhs).GetOrigin();
	point ptBoxOrigin = GetOrigin();
	point ptMax = GetMaxPoint();
	point ptMin = GetMinPoint();

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());
	//CollisionManifold manifold = CollisionManifold(rhs.GetParentObject(), this->m_pParent);

	if (m_type == Type::OBB) {
		//point ptRelativeOrigin = GetOrigin() - ptSphereOrigin;
		//ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation())) * (GetOrigin() - ptSphereOrigin));
		//ptSphereOrigin = (point)(inverse(this->m_pParent->GetModelMatrix()) * ptSphereOrigin);
		ptSphereOrigin = (point)(inverse(RotationMatrix(GetOrientation())) * (ptSphereOrigin - GetOrigin()));
		ptMax = m_vHalfSize;
		ptMin = m_vHalfSize * -1.0f;
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
			ptClosestPoint = (RotationMatrix(GetOrientation()) * ptClosestPoint) + GetOrigin();
		}
		//ptClosestPoint = (this->m_pParent->GetModelMatrix() * ptClosestPoint);

		vector vNormal = static_cast<BoundingSphere>(rhs).GetOrigin() - ptClosestPoint;
		vNormal.Normalize();

		point ptContact = ptClosestPoint;
		float penetration = static_cast<BoundingSphere>(rhs).GetRadius() - std::sqrt(distanceSquared);

		manifold.AddContactPoint(ptContact, vNormal, -penetration);
	}

	return manifold;
}

bool BoundingBox::Intersect(const BoundingQuad& rhs) {
	return static_cast<BoundingQuad>(rhs).Intersect(*this);
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

RESULT BoundingBox::SetMaxPointFromOrigin(point ptMax) {
	m_vHalfSize = (ptMax - GetOrigin());
	return R_PASS;
}

// http://www.willperone.net/Code/coderr.php
vector BoundingBox::GetHalfVector() {
	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

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

		return vector(width, height, length);
	}
	
	// Otherwise it's OBB
	return m_vHalfSize;
}

bool BoundingBox::OverlapOnAxis(const BoundingBox& rhs, const vector &vAxis) {
	// Project the half-size of one onto axis
	double selfProject = TransformToAxis(vAxis);
	double rhsProject = static_cast<BoundingBox>(rhs).TransformToAxis(vAxis);

	vector vToCenter = static_cast<BoundingBox>(rhs).GetOrigin() - GetOrigin();

	double distance = std::abs(vToCenter.dot(vAxis));

	return (distance <= (selfProject + rhsProject));
}

// Project half size onto vector axis
double BoundingBox::TransformToAxis(const vector &vAxis) {
	double retVal = 0.0f;

	retVal += GetHalfVector().x() * std::abs(vAxis.dot(GetAxis(BoxAxis::X_AXIS)));
	retVal += GetHalfVector().y() * std::abs(vAxis.dot(GetAxis(BoxAxis::Y_AXIS)));
	retVal += GetHalfVector().z() * std::abs(vAxis.dot(GetAxis(BoxAxis::Z_AXIS)));

	return retVal;
}

vector BoundingBox::GetAxis(BoxAxis boxAxis) {
	vector retVector = vector(0.0f, 0.0f, 0.0f);

	switch (boxAxis) {
		case BoxAxis::X_AXIS: retVector = vector::iVector(1.0f); break;
		case BoxAxis::Y_AXIS: retVector = vector::jVector(1.0f); break;
		case BoxAxis::Z_AXIS: retVector = vector::kVector(1.0f); break;
	}

	// Rotate by OBB if so
	if (m_type == Type::OBB) {
		retVector = RotationMatrix(GetOrientation()) * retVector;
		retVector.Normalize();
	}

	return retVector;
}

double BoundingBox::GetWidth() {
	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double width = 0.0f;
		for (int i = 0; i < 8; i++) {
			point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			if (pt.x() > width)
				width = pt.x();
		}

		return static_cast<double>(width * 2.0f);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.x() * 2.0f);
}

double BoundingBox::GetHeight() {
	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double height = 0.0f;
		for (int i = 0; i < 8; i++) {
			point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			if (pt.y() > height)
				height = pt.y();
		}

		return static_cast<double>(height * 2.0f);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.y() * 2.0f);
}

double BoundingBox::GetLength() {
	if (m_type == Type::AABB) {
		RotationMatrix rotMat = RotationMatrix(GetOrientation());	// .GetEulerAngles(&phi, &theta, &psi);

		double length = 0.0f;
		for (int i = 0; i < 8; i++) {
			point pt = rotMat * GetBoxPoint((BoxPoint)(i));
			if (pt.z() > length)
				length = pt.z();
		}

		return static_cast<double>(length * 2.0f);
	}
	
	// Otherwise it's OBB
	return static_cast<double>(m_vHalfSize.z() * 2.0f);
}

// TODO: Why do we need to invert the point?
point BoundingBox::GetMinPoint() {
	return (GetOrigin() - GetHalfVector());
}

point BoundingBox::GetMaxPoint() {
	return (GetOrigin() + GetHalfVector());
}

point BoundingBox::GetBoxPoint(BoxPoint ptType) {
	point retPoint = point(m_vHalfSize);
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
	retPoint = RotationMatrix(GetOrientation()) * retPoint;
	retPoint = retPoint + GetOrigin();

	return retPoint;
}

BoundingBox::face BoundingBox::GetFace(BoxFace faceType) {
	BoundingBox::face faceBox;
	faceBox.m_type = faceType;

	switch (faceType) {
		case BoxFace::TOP: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_RIGHT_FAR);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_LEFT_FAR);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::TOP_LEFT_NEAR);
		} break;

		case BoxFace::BOTTOM: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR);
		} break;

		case BoxFace::LEFT: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_LEFT_NEAR);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_LEFT_FAR);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR);
		} break;

		case BoxFace::RIGHT: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_RIGHT_FAR);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR);
		} break;

		case BoxFace::FRONT: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_LEFT_NEAR);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR);
		} break;

		case BoxFace::BACK: {
			faceBox.m_points[0] = GetBoxPoint(BoxPoint::TOP_LEFT_FAR);
			faceBox.m_points[1] = GetBoxPoint(BoxPoint::TOP_RIGHT_FAR);
			faceBox.m_points[2] = GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR);
			faceBox.m_points[3] = GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR);
		} break;
	}

	return faceBox;
}

line BoundingBox::GetBoxEdge(BoxEdge edgeType) {
	line lineEdge;

	switch (edgeType) {
	case BoxEdge::TOP_RIGHT: lineEdge = line(GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR), GetBoxPoint(BoxPoint::TOP_RIGHT_FAR)); break;
	case BoxEdge::TOP_LEFT: lineEdge = line(GetBoxPoint(BoxPoint::TOP_LEFT_NEAR), GetBoxPoint(BoxPoint::TOP_LEFT_FAR)); break;
	case BoxEdge::TOP_NEAR: lineEdge = line(GetBoxPoint(BoxPoint::TOP_LEFT_NEAR), GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR)); break;
	case BoxEdge::TOP_FAR: lineEdge = line(GetBoxPoint(BoxPoint::TOP_LEFT_FAR), GetBoxPoint(BoxPoint::TOP_RIGHT_FAR)); break;
	case BoxEdge::BOTTOM_RIGHT: lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR), GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR)); break;
	case BoxEdge::BOTTOM_LEFT: lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR), GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR)); break;
	case BoxEdge::BOTTOM_NEAR: lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR), GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR)); break;
	case BoxEdge::BOTTOM_FAR: lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR), GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR)); break;
	case BoxEdge::LEFT_NEAR: lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_NEAR), GetBoxPoint(BoxPoint::TOP_LEFT_NEAR)); break;
	case BoxEdge::LEFT_FAR: lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_LEFT_FAR), GetBoxPoint(BoxPoint::TOP_LEFT_FAR)); break;
	case BoxEdge::RIGHT_NEAR:lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_RIGHT_NEAR), GetBoxPoint(BoxPoint::TOP_RIGHT_NEAR)); break;
	case BoxEdge::RIGHT_FAR:lineEdge = line(GetBoxPoint(BoxPoint::BOTTOM_RIGHT_FAR), GetBoxPoint(BoxPoint::TOP_RIGHT_FAR)); break;
	}

	return lineEdge;
}