#include "ObjectState.h"
#include "ObjectDerivative.h"
#include "PhysicsEngine/ForceGenerator.h"

ObjectState::ObjectState() :
	m_ptOrigin(),
	m_vVelocity(),
	m_qRotation(),
	m_qAngularMomentum()
{
	/*empty*/
}

ObjectState::ObjectState(point ptOrigin) :
	m_ptOrigin(ptOrigin),
	m_vVelocity(),
	m_qRotation(),
	m_qAngularMomentum()
{
	/*empty*/
}

RESULT ObjectState::Clear() {
	m_ptOrigin.clear();
	m_vVelocity.clear();
	m_qRotation.clear();
	m_qAngularMomentum.clear();
	return R_SUCCESS;
}

// p = mv or v = p/m
RESULT ObjectState::Recalculate() {
	m_vVelocity = m_vMomentum * m_inverseMass;
	return R_SUCCESS;
}

RESULT ObjectState::SetMass(double kgMass) {
	// Negative mass not allowed
	if (kgMass < 0.0f)
		return R_FAIL;

	m_kgMass = kgMass;
	m_inverseMass = 1.0f / kgMass;
	Recalculate();

	return R_SUCCESS;
}

const double ObjectState::GetMass() {
	return m_kgMass;
}

const double ObjectState::GetInverseMass() {
	return m_inverseMass;
}

const point ObjectState::GetOrigin() { 
	return m_ptOrigin; 
}

RESULT ObjectState::SetVelocity(vector vVelocity) {
	// Actually this sets the momentum p = mv
	m_vMomentum = vVelocity * m_kgMass;
	Recalculate();

	return R_SUCCESS;
}

RESULT ObjectState::AddMomentumImpulse(vector vImplulse) {
	// Actually this sets the momentum p = mv
	m_vMomentum += vImplulse;
	Recalculate();

	return R_SUCCESS;
}

RESULT ObjectState::AddPendingMomentumImpulse(vector vImplulse) {
	m_pendingMomentumVectors.push_back(vImplulse);
	return R_SUCCESS;
}

RESULT ObjectState::CommitPendingMomentum() {
	if (m_pendingMomentumVectors.size() == 0)
		return R_SUCCESS;

	vector vMomentumAccumulator = vector();
	for (auto &vMomentum : m_pendingMomentumVectors) {
		vMomentumAccumulator += vMomentum;
	}

	m_pendingMomentumVectors.clear();

	return AddMomentumImpulse(vMomentumAccumulator);
}

RESULT ObjectState::AddPendingTranslation(vector vTranslation) {
	m_pendingTranslationVectors.push_back(vTranslation);
	return R_SUCCESS;
}

RESULT ObjectState::CommitPendingTranslation() {
	if (m_pendingTranslationVectors.size() == 0)
		return R_SUCCESS;

	vector vTranslationAccumulator = vector();
	for (auto &vTranslation : m_pendingTranslationVectors) {
		vTranslationAccumulator += vTranslation;
	}

	m_pendingTranslationVectors.clear();

	return Translate(vTranslationAccumulator);
}

RESULT ObjectState::Translate(vector vTranslation) {
	m_ptOrigin += vTranslation;
	return R_SUCCESS;
}

const vector ObjectState::GetVelocity() { 
	return m_vVelocity; 
}

const vector ObjectState::GetMomentum() {
	return m_vMomentum;
}

const quaternion ObjectState::GetRotation() { 
	return m_qRotation; 
}

const quaternion ObjectState::GetAngularMoment() { 
	return m_qAngularMomentum; 
}

// This will evaluate the derivative to this state, and update the derivative
ObjectDerivative ObjectState::Evaluate(float timeStart, float timeDelta, const ObjectDerivative &objectDerivative, const std::list<ForceGenerator*> &externalForceGenerators) {
	ObjectDerivative derivativeOutput;

	if (timeDelta > 0.0f)
		derivativeOutput.m_vRateOfChangeOrigin = m_vVelocity + (objectDerivative.m_vForce * m_inverseMass) * timeDelta;
	else
		derivativeOutput.m_vRateOfChangeOrigin = m_vVelocity;

	// TODO: Add force generators etc
	//TODO: derivativeOutput.m_vRateOfChangeVelocity = 
	// TEMP: This is just to test a static gravity
	//derivativeOutput.m_vRateOfChangeVelocity = vector(0.0f, -9.8f, 0.0f);
	//derivativeOutput.m_vForce = vector(0.0f, -9.8f, 0.0f) * m_kgMass;

	//const float k = 100;
	//const float b = 1;
	//derivativeOutput.m_vForce  = -k * m_ptOrigin - b * derivativeOutput.m_vRateOfChangeOrigin;

	// External Forces
	if (externalForceGenerators.size() > 0) {
		for (auto &forceGenerator : externalForceGenerators) {
			derivativeOutput.m_vForce += forceGenerator->GenerateForce(this, timeStart, timeDelta);
		}
	}

	// Internal Forces
	if (m_forceGenerators.size() > 0) {
		for (auto &forceGenerator : m_forceGenerators) {
			derivativeOutput.m_vForce += forceGenerator->GenerateForce(this, timeStart, timeDelta);
		}
	}
	
	//output.dv = acceleration(state, t + dt);
	return derivativeOutput;
}


// This is the core of the RK4 integration method for object state - we might want to have alternative ways
// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
template <>
RESULT ObjectState::Integrate<ObjectState::IntegrationType::RK4>(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators) {
	RESULT r = R_SUCCESS;

	ObjectDerivative derivativeA, derivativeB, derivativeC, derivativeD;

	// Sample the derivative
	derivativeA = Evaluate(timeStart, timeDelta * 0.0f, ObjectDerivative(), externalForceGenerators);
	derivativeB = Evaluate(timeStart, timeDelta * 0.5f, derivativeA, externalForceGenerators);
	derivativeC = Evaluate(timeStart, timeDelta * 0.5f, derivativeB, externalForceGenerators);
	derivativeD = Evaluate(timeStart, timeDelta * 1.0f, derivativeC, externalForceGenerators);

	// Get the effective rate of change by using a weighted sum of the derivatives using the Taylor series expansion
	//vector rateOfChangeOrigin = (1.0f / 6.0f) * (derivativeA.m_vRateOfChangeOrigin + 2.0f * (derivativeB.m_vRateOfChangeOrigin + derivativeC.m_vRateOfChangeOrigin) + derivativeD.m_vRateOfChangeOrigin);
	vector force = (1.0f / 6.0f) * (derivativeA.m_vForce + 2.0f * (derivativeB.m_vForce + derivativeC.m_vForce) + derivativeD.m_vForce);

	//Clear();
	m_vMomentum += force * timeDelta;
	Recalculate();

	m_ptOrigin += m_vVelocity * timeDelta;
	//m_ptOrigin += rateOfChangeOrigin * timeDelta;
	//m_vVelocity += (force * m_inverseMass) * timeDelta;

// Error:
	return r;
}

// The Euclid integration 
template <>
RESULT ObjectState::Integrate<ObjectState::IntegrationType::EUCLID>(float timeStart, float timeDelta, const std::list<ForceGenerator*> &externalForceGenerators) {
	RESULT r = R_SUCCESS;

	ObjectDerivative objDerivative;

	// Sample the derivative
	objDerivative = Evaluate(timeStart, 0.0f, ObjectDerivative(), externalForceGenerators);

	m_vMomentum += objDerivative.m_vForce * timeDelta;
	Recalculate();

	m_ptOrigin += m_vVelocity * timeDelta;

	//m_ptOrigin += objDerivative.m_vRateOfChangeOrigin * timeDelta;
	//m_vVelocity += objDerivative.m_vRateOfChangeVelocity * timeDelta;

	Recalculate();

	// Error:
	return r;
}

RESULT ObjectState::AddForceGenerator(ForceGenerator *pForceGenerator) {
	RESULT r = R_SUCCESS;

	CN(pForceGenerator);

	m_forceGenerators.push_back(pForceGenerator);
	//CB((pForceGenerator == nullptr));

Error:
	return r;
}

RESULT ObjectState::ClearForceGenerators() {
	RESULT r = R_SUCCESS;

	for (auto &pForceGenerator : m_forceGenerators) {
		if (pForceGenerator != nullptr) {
			delete pForceGenerator;
			pForceGenerator = nullptr;
		}
	}

	m_forceGenerators.clear();

//Error:
	return r;
}