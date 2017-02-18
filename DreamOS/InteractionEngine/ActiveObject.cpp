#include "ActiveObject.h"

ActiveObject::ActiveObject(VirtualObj *pObject) :
	m_pObject(pObject),
	m_state(ActiveObject::state::NOT_INTERSECTED)
{
	// empty
}

RESULT ActiveObject::UpdateObject(const point &ptIntersection, const vector &vNormal, ActiveObject::state newState) {
	m_ptIntersection = ptIntersection;
	m_vNormal = vNormal;
	m_state = newState;

	return R_PASS;
}

ActiveObject::state ActiveObject::GetState() {
	return m_state;
}

point ActiveObject::GetIntersectionPoint() {
	return m_ptIntersection;
}

vector ActiveObject::GetIntersectionNormal() {
	return m_vNormal;
}

VirtualObj* ActiveObject::GetObject() {
	return m_pObject;
}