#include "ObjectDerivative.h"

ObjectDerivative::ObjectDerivative() :
	m_vRateOfChangeOrigin(),
	m_vRateOfChangeVelocity(),
	m_vRateOfChangeAcceleration(),
	m_qRateOfChangeRotation(),
	m_qRateOfChangeAngularMomentum()
{
	// empty
}


vector ObjectDerivative::GetRateOfChangeOrigin() {
	return m_vRateOfChangeOrigin;
}

vector ObjectDerivative::GetRateOfChangeVelocity() {
	return m_vRateOfChangeVelocity;
}

vector ObjectDerivative::GetRateOfChangeAcceleration() {
	return m_vRateOfChangeAcceleration;
}

quaternion ObjectDerivative::GetRateOfChangeRotation() {
	return m_qRateOfChangeRotation;
}

quaternion ObjectDerivative::GetRateOfChangeAngularMomentun() {
	return m_qRateOfChangeAngularMomentum;
}