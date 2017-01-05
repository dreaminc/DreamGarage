#include "ObjectState.h"
#include "ObjectDerivative.h"

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

RESULT ObjectState::Clear() {
	m_ptOrigin.clear();
	m_vVelocity.clear();
	m_vAcceleration.clear();
	m_qRotation.clear();
	m_qAngularMomentum.clear();
	return R_SUCCESS;
}

const point ObjectState::GetOrigin() { 
	return m_ptOrigin; 
}

const vector ObjectState::GetVelocity() { 
	return m_vVelocity; 
}

const vector ObjectState::GetAcceleration() { 
	return m_vAcceleration; 
}

const quaternion ObjectState::GetRotation() { 
	return m_qRotation; 
}

const quaternion ObjectState::GetAngularMoment() { 
	return m_qAngularMomentum; 
}

// This will evaluate the derivative to this state, and update the derivative
ObjectDerivative ObjectState::Evaluate(float timeStart, float timeDelta, const ObjectDerivative &objectDerivative) {
	ObjectDerivative derivativeOutput;

	if (timeDelta > 0.0f)
		derivativeOutput.m_vRateOfChangeOrigin = m_vVelocity + objectDerivative.m_vRateOfChangeVelocity * timeDelta;
	else
		derivativeOutput.m_vRateOfChangeOrigin = m_vVelocity;

	// TODO: Add force generators etc
	//TODO: derivativeOutput.m_vRateOfChangeVelocity = 
	// TEMP: This is just to test a static gravity
	derivativeOutput.m_vRateOfChangeVelocity = vector(0.0f, -9.8f, 0.0f);
	
	//output.dv = acceleration(state, t + dt);
	return derivativeOutput;
}


// This is the core of the RK4 integration method for object state - we might want to have alternative ways
// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
template <>
RESULT ObjectState::Integrate<ObjectState::IntegrationType::RK4>(float timeStart, float timeDelta) {
	RESULT r = R_SUCCESS;

	ObjectDerivative derivativeA, derivativeB, derivativeC, derivativeD;

	// Sample the derivative
	derivativeA = Evaluate(timeStart, timeDelta * 0.0f, ObjectDerivative());
	derivativeB = Evaluate(timeStart, timeDelta * 0.5f, derivativeA);
	derivativeC = Evaluate(timeStart, timeDelta * 0.5f, derivativeB);
	derivativeD = Evaluate(timeStart, timeDelta * 1.0f, derivativeC);

	// Get the effective rate of change by using a weighted sum of the derivatives using the Taylor series expansion
	vector rateOfChangeOrigin = (1.0f / 6.0f) * (derivativeA.m_vRateOfChangeOrigin + 2.0f * (derivativeB.m_vRateOfChangeOrigin + derivativeC.m_vRateOfChangeOrigin) + derivativeD.m_vRateOfChangeOrigin);
	vector rateOfChangeVelocity = (1.0f / 6.0f) * (derivativeA.m_vRateOfChangeVelocity + 2.0f * (derivativeB.m_vRateOfChangeVelocity + derivativeC.m_vRateOfChangeVelocity) + derivativeD.m_vRateOfChangeVelocity);

	//Clear();
	m_ptOrigin += rateOfChangeOrigin * timeDelta;
	m_vVelocity += rateOfChangeVelocity * timeDelta;

// Error:
	return r;
}

// The Euclid integration 
template <>
RESULT ObjectState::Integrate<ObjectState::IntegrationType::EUCLID>(float timeStart, float timeDelta) {
	RESULT r = R_SUCCESS;

	ObjectDerivative objDerivative;

	// Sample the derivative
	objDerivative = Evaluate(timeStart, 0.0f, ObjectDerivative());

	m_ptOrigin += objDerivative.m_vRateOfChangeOrigin * timeDelta;
	m_vVelocity += objDerivative.m_vRateOfChangeVelocity * timeDelta;

	// Error:
	return r;
}