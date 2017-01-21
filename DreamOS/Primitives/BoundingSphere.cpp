#include "BoundingSphere.h"
#include "BoundingBox.h"
#include "BoundingQuad.h"

#include "PhysicsEngine/CollisionManifold.h"

BoundingSphere::BoundingSphere(VirtualObj *pParentObject) :
	BoundingVolume(pParentObject),
	m_radius(1.0f)
{
	// Empty
}

BoundingSphere::BoundingSphere(VirtualObj *pParentObject, point ptOrigin, float radius) :
	BoundingVolume(pParentObject, ptOrigin),
	m_radius(radius)
{
	// Empty
}

bool BoundingSphere::Intersect(const BoundingSphere& rhs) {
	float distance = (const_cast<BoundingSphere&>(rhs).GetOrigin() - GetOrigin()).magnitude();

	if (abs(distance) <= (rhs.m_radius + m_radius))
		return true;
	else
		return false;
}

// TODO: Propagate this up to BoundingVolume instead?
bool BoundingSphere::Intersect(const BoundingBox& rhs) {
	return static_cast<BoundingBox>(rhs).Intersect(*this);
}

bool BoundingSphere::Intersect(const BoundingQuad& rhs) {
	return static_cast<BoundingQuad>(rhs).Intersect(*this);
}

bool BoundingSphere::Intersect(point& pt) {
	float distance = (pt - GetOrigin()).magnitude();

	if (abs(distance) <= m_radius)
		return true;
	else
		return false;
}

// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
bool BoundingSphere::Intersect(line& ln) {
	vector vLine = ln.GetVector();
	vector vLineCircle = ln.a() - GetOrigin();

	vector_precision dotLineCircleValueSq = pow(vLine.dot(vLineCircle), 2.0f);
	vector_precision lineCircleMagnitudeSq = pow(vLineCircle.magnitude(), 2.0f);
	vector_precision sqRootComponent = dotLineCircleValueSq - lineCircleMagnitudeSq + pow(m_radius, 2.0f);

	if (sqRootComponent < 0)
		return false;
	else
		return true;
}

// https://capnramses.github.io//opengl/raycasting.html
bool BoundingSphere::Intersect(const ray &r) {
	vector vRayCircle = static_cast<ray>(r).ptOrigin() - GetOrigin();
	
	float bValue = static_cast<ray>(r).vDirection().dot(vRayCircle);
	float cValue = vRayCircle.dot(vRayCircle) - pow(m_radius, 2.0f);
	float resultValue = pow(bValue, 2.0f) - cValue;

	if (resultValue < 0.0f) {
		return false;
	}
	else if (resultValue == 0.0f) {
		// TODO: return single intersection pt
		return true;
	}
	else if (resultValue > 0.0f) {
		// TODO: return double intersection pt
		return true;
	}

	return false;
}

CollisionManifold BoundingSphere::Collide(const BoundingSphere& rhs) {
	vector vMidLine = (const_cast<BoundingSphere&>(rhs).GetOrigin() - GetOrigin());
	float distance = vMidLine.magnitude();

	CollisionManifold manifold = CollisionManifold(this->m_pParent, rhs.GetParentObject());

	if (abs(distance) <= (rhs.m_radius + m_radius)) {
		// Find the contact point and normal
		vector vNormal = vMidLine.Normal();
		point ptContact = const_cast<BoundingSphere&>(rhs).GetOrigin() + (vMidLine * 0.5f);
		double penetration = (rhs.m_radius + m_radius) - abs(distance);

		manifold.AddContactPoint(ptContact, vNormal, penetration, 1);

		// TODO: Friction / Restitution?
	}

	return manifold;
}

CollisionManifold BoundingSphere::Collide(const BoundingBox& rhs) {
	return static_cast<BoundingBox>(rhs).Collide(*this);
}

CollisionManifold BoundingSphere::Collide(const BoundingQuad& rhs) {
	return static_cast<BoundingQuad>(rhs).Collide(*this);
}

RESULT BoundingSphere::SetMaxPointFromOrigin(point ptMax) {
	vector vDiff = (ptMax - GetOrigin());
	//m_radius = vDiff.magnitude();
	
	m_radius = ptMax.x();
	
	if (ptMax.y() > m_radius)
		m_radius = ptMax.y();

	if (ptMax.z() > m_radius)
		m_radius = ptMax.z();
	return R_PASS;
}