#include "ActiveObject.h"
#include "PhysicsEngine/ContactPoint.h"

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

RESULT ActiveObject::SetState(ActiveObject::state newState) {
	m_state = newState;
	return R_PASS;
}

RESULT ActiveObject::AddState(ActiveObject::state newState) {
	if (newState != state::NOT_INTERSECTED) {
		m_state = (ActiveObject::state)(static_cast<uint8_t>(m_state) | static_cast<uint8_t>(newState));
	}
	/*
	else {
		m_state = state::NOT_INTERSECTED;
	}
	*/

	return R_PASS;
}

RESULT ActiveObject::RemoveState(ActiveObject::state newState) {
	m_state = (ActiveObject::state)(static_cast<uint8_t>(m_state) & ~static_cast<uint8_t>(newState));
	return R_PASS;
}

bool ActiveObject::HasState(ActiveObject::state newState) {
	if ((static_cast<uint8_t>(m_state) & static_cast<uint8_t>(newState)) != 0)
		return true;

	return false;
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

RESULT ActiveObject::SetInteractionPoint(point ptIntersection) {
	m_ptIntersection = ptIntersection;
	return R_PASS;
}

RESULT ActiveObject::SetIntersectionNormal(vector vNormal) {
	m_vNormal = vNormal;
	return R_PASS;
}

RESULT ActiveObject::SetContactPoint(ContactPoint contactPoint) {
	m_ptIntersection = contactPoint.GetPoint();
	m_vNormal = contactPoint.GetNormal();

	return R_PASS;
}

ActiveObject::state operator&(ActiveObject::state lhs, ActiveObject::state rhs) {
	return (ActiveObject::state)(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

ActiveObject::state operator|(ActiveObject::state lhs, ActiveObject::state rhs) {
	return (ActiveObject::state)(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}