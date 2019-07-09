#include "ContactPoint.h"

ContactPoint::ContactPoint() :
	m_ptContact(),
	m_vNormal(),
	m_penetrationDepth(),
	m_weight(1)
{
	// empty 
}

ContactPoint::ContactPoint(point ptContact, vector vNormal, double penetrationDepth, int weight) :
	m_ptContact(ptContact), 
	m_vNormal(vNormal), 
	m_penetrationDepth(penetrationDepth),
	m_weight(weight)
{
	// empty 
}

vector ContactPoint::GetNormal() const {
	return m_vNormal;
}

point ContactPoint::GetPoint() const {
	return m_ptContact;
}

int ContactPoint::GetWeight() const {
	return m_weight;
}

double ContactPoint::GetPenetration() const {
	return m_penetrationDepth;
}