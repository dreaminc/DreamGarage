#include "ObjectState.h"

ObjectState::ObjectState() :
	m_ptOrigin(),
	m_vVelocity(),
	m_vAcceleration(),
	m_qRotation(),
	m_qAngularMomentum()
{
	/*empty*/
}

ObjectState::ObjectState(point ptOrigin) :
	m_ptOrigin(ptOrigin),
	m_vVelocity(),
	m_vAcceleration(),
	m_qRotation(),
	m_qAngularMomentum()
{
	/*empty*/
}


point ObjectState::GetOrigin() { 
	return m_ptOrigin; 
}

vector ObjectState::GetVelocity() { 
	return m_vVelocity; 
}

vector ObjectState::GetAcceleration() { 
	return m_vAcceleration; 
}

quaternion ObjectState::GetRotation() { 
	return m_qRotation; 
}

quaternion ObjectState::GetAngularMoment() { 
	return m_qAngularMomentum; 
}