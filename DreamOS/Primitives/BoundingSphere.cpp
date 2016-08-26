#include "BoundingSphere.h"
#include "BoundingBox.h"

BoundingSphere::BoundingSphere() :
	BoundingVolume(),
	m_radius(1.0f)
{
	// Empty
}

BoundingSphere::BoundingSphere(point ptOrigin, float radius) :
	BoundingVolume(ptOrigin),
	m_radius(radius)
{
	// Empty
}

bool BoundingSphere::Intersect(const BoundingSphere& rhs) {
	float distance = (rhs.m_ptOrigin - m_ptOrigin).magnitude();

	if (abs(distance) <= (rhs.m_radius + m_radius))
		return true;
	else
		return false;
}

// TODO: Propagate this up to BoundingVolume src instead?
bool BoundingSphere::Intersect(const BoundingBox& rhs) {
	return static_cast<BoundingBox>(rhs).Intersect(*this);
}

bool BoundingSphere::Intersect(point& pt) {
	float distance = (pt - m_ptOrigin).magnitude();

	if (abs(distance) <= m_radius)
		return true;
	else
		return false;
}

// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
bool BoundingSphere::Intersect(line& ln) {
	vector vLine = ln.GetVector();
	vector vLineCircle = ln.a() - m_ptOrigin;

	vector_precision dotLineCircleValueSq = pow(vLine.dot(vLineCircle), 2.0f);
	vector_precision lineCircleMagnitudeSq = pow(vLineCircle.magnitude, 2.0f);
	vector_precision sqRootComponent = dotLineCircleValueSq - lineCircleMagnitudeSq + pow(m_radius, 2.0f);

	if (sqRootComponent < 0)
		return false;
	else
		return true;
}