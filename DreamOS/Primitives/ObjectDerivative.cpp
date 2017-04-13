#include "ObjectDerivative.h"

ObjectDerivative::ObjectDerivative() :
	m_vRateOfChangeOrigin(),
	m_qRateOfChangeRotation(),
	m_qRateOfChangeAngularMomentum()
{
	// empty
}


RESULT ObjectDerivative::Clear() {
	m_vRateOfChangeOrigin.clear();
	m_qRateOfChangeRotation.clear();
	m_qRateOfChangeAngularMomentum.clear();
	return R_SUCCESS;
}

const vector ObjectDerivative::GetRateOfChangeOrigin() {
	return m_vRateOfChangeOrigin;
}

const quaternion ObjectDerivative::GetRateOfChangeRotation() {
	return m_qRateOfChangeRotation;
}

const quaternion ObjectDerivative::GetRateOfChangeAngularMomentun() {
	return m_qRateOfChangeAngularMomentum;
}