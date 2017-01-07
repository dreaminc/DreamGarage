#include "ContactPoint.h"

ContactPoint::ContactPoint() :
	m_ptContact(),
	m_vNormal(),
	m_penetrationDepth()
{
	// empty 
}

ContactPoint::ContactPoint(point ptContact, vector vNormal, double penetrationDepth) :
	m_ptContact(ptContact), 
	m_vNormal(vNormal), 
	m_penetrationDepth(penetrationDepth)
{
	// empty 
}

vector ContactPoint::GetNormal() {
	return m_vNormal;
}