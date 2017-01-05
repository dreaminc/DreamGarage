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


RESULT ObjectDerivative::Clear() {
	m_vRateOfChangeOrigin.clear();
	m_vRateOfChangeVelocity.clear();
	m_vRateOfChangeAcceleration.clear();
	m_qRateOfChangeRotation.clear();
	m_qRateOfChangeAngularMomentum.clear();
	return R_SUCCESS;
}

const vector ObjectDerivative::GetRateOfChangeOrigin() {
	return m_vRateOfChangeOrigin;
}

const vector ObjectDerivative::GetRateOfChangeVelocity() {
	return m_vRateOfChangeVelocity;
}

const vector ObjectDerivative::GetRateOfChangeAcceleration() {
	return m_vRateOfChangeAcceleration;
}

const quaternion ObjectDerivative::GetRateOfChangeRotation() {
	return m_qRateOfChangeRotation;
}

const quaternion ObjectDerivative::GetRateOfChangeAngularMomentun() {
	return m_qRateOfChangeAngularMomentum;
}