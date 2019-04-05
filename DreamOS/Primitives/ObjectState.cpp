#include "ObjectState.h"
#include "ObjectDerivative.h"
#include "PhysicsEngine/ForceGenerator.h"

#include "DimObj.h"
#include "BoundingVolume.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingQuad.h"

ObjectState::ObjectState(VirtualObj *pParentObj) :
	m_pParentObj(pParentObj),
	m_ptOrigin(),
	m_vVelocity(),
	//m_qRotation(0, vector::jVector(1.0f)),
	m_vTorque(),
	m_vAngularMomentum(),
	m_ptCenterOfMass(),
	m_massDistributionType(ObjectState::MassDistributionType::INVALID)
{
	/*empty*/
}

ObjectState::ObjectState(VirtualObj *pParentObj, point ptOrigin) :
	m_pParentObj(pParentObj),
	m_ptOrigin(ptOrigin),
	m_vVelocity(),
	//m_qRotation(0, vector::jVector(1.0f)),
	m_vTorque(),
	m_vAngularMomentum(),
	m_ptCenterOfMass(),
	m_massDistributionType(ObjectState::MassDistributionType::INVALID)
{
	/*empty*/
}

RESULT ObjectState::Clear() {
	m_ptOrigin.clear();
	m_vVelocity.clear();
	m_qRotation.clear();
	m_vAngularMomentum.clear();
	return R_SUCCESS;
}

// p = mv or v = p/m
RESULT ObjectState::RecalculateLinearVelocity() {
	//m_vVelocity = m_vMomentum * m_inverseMass;
	m_vVelocity = m_vMomentum * (1.0f / m_kgMass);
	return R_SUCCESS;
}

RESULT ObjectState::RecalculateAngularVelocity() {
	m_vAngularVelocity = m_matInverseInertiaTensor * m_vAngularMomentum;

	m_qRotation.Normalize();

	quaternion_precision vals[] = {
		0.0f,
		m_vAngularVelocity.x(),
		m_vAngularVelocity.y(),
		m_vAngularVelocity.z()
	};

	quaternion qW = quaternion(vals);

	// Try this out TODO: Move to recalc of rotational quantities, move to eval func
	m_qSpin = qW * m_qRotation;
	m_qSpin = m_qSpin * 0.5f;

	return R_SUCCESS;
}

RESULT ObjectState::SetMass(double kgMass) {
	// Negative mass not allowed
	if (kgMass < 0.0f)
		return R_FAIL;

	m_kgMass = kgMass;
	m_inverseMass = 1.0f / kgMass;
	
	RecalculateLinearVelocity();
	RecalculateInertialTensor();

	return R_SUCCESS;
}

const double ObjectState::GetMass() {
	if (m_fImmovable == false)
		return m_kgMass;
	else
		return std::numeric_limits<double>::infinity();
}

const double ObjectState::GetInverseMass() {
	if (m_fImmovable == false)
		return m_inverseMass;
	else
		return 0.0f;
}

const point ObjectState::GetOrigin() { 
	return m_ptOrigin; 
}

RESULT ObjectState::SetImmovable(bool fImmovable) {
	m_fImmovable = fImmovable;
	return R_SUCCESS;
}

bool ObjectState::IsImmovable() {
	return m_fImmovable;
}

RESULT ObjectState::SetRotationalVelocity(vector vRotationalVelocity) {
	//m_vAngularVelocity = vRotationalVelocity;
	m_vAngularMomentum = m_matInertiaTensor * vRotationalVelocity;

	RecalculateAngularVelocity();
	return R_SUCCESS;
}

vector ObjectState::GetRotationalVelocity() {
	return m_vAngularVelocity;
}

RESULT ObjectState::SetVelocity(vector vVelocity) {
	// Actually this sets the momentum p = mv
	m_vMomentum = vVelocity * m_kgMass;
	RecalculateLinearVelocity();

	return R_SUCCESS;
}

RESULT ObjectState::AddMomentumImpulse(vector vImplulse) {
	// Actually this sets the momentum p = mv
	if (m_fImmovable == false) {
		m_vMomentum += vImplulse;
		RecalculateLinearVelocity();
	}

	return R_SUCCESS;
}

RESULT ObjectState::AddTorqueImpulse(vector vTorque) {
	if (m_fImmovable == false) {
		m_vAngularMomentum += m_matInertiaTensor * vTorque;
		//m_vAngularMomentum += vTorque;
		RecalculateAngularVelocity();
	}

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

RESULT ObjectState::AddPendingTorque(vector vTorque) {
	m_pendingTorqueVectors.push_back(vTorque);
	return R_SUCCESS;
}

RESULT ObjectState::CommitPendingTorque() {
	if (m_pendingTorqueVectors.size() == 0)
		return R_SUCCESS;

	vector vTorqueAccumulator = vector();
	for (auto &vTorque : m_pendingTorqueVectors) {
		vTorqueAccumulator += vTorque;
	}

	m_pendingTorqueVectors.clear();

	return AddTorqueImpulse(vTorqueAccumulator);
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

RESULT ObjectState::SetMassDistributionType(ObjectState::MassDistributionType type) {
	RESULT r = R_SUCCESS;

	m_massDistributionType = type;

	if (m_kgMass > 0.0f) {
		CR(RecalculateInertialTensor());
	}
	
Error:
	return r;
}

RESULT ObjectState::SetInertiaTensor(MassDistributionType type, const matrix<point_precision, 3, 3> &matInertiaTensor) {
	m_massDistributionType = type;

	auto matInverseIntertiaTensor3x3 = inverse(matInertiaTensor);

	m_matInertiaTensor.identity(1.0f);
	m_matInverseInertiaTensor.identity(1.0f);

	// Convert to an identity 4x4 matrix to make transforms easy with our data types
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			m_matInertiaTensor.element(i, j) = matInertiaTensor.element(i, j);
			m_matInverseInertiaTensor.element(i, j) = matInverseIntertiaTensor3x3.element(i, j);
		}
	}

	return R_SUCCESS;
}

RESULT ObjectState::RecalculateInertialTensor() {
	RESULT r = R_SUCCESS;

	CB((m_massDistributionType != ObjectState::MassDistributionType::INVALID));
	CN(m_pParentObj);
	DimObj *pDimObj = dynamic_cast<DimObj*>(m_pParentObj);
	CN(pDimObj);

	switch (m_massDistributionType) {
		case ObjectState::MassDistributionType::VOLUME: {
			BoundingBox *pBoundingBox = dynamic_cast<BoundingBox*>(pDimObj->GetBoundingVolume().get());
			CN(pBoundingBox);

			return SetInertiaTensorVolume(pBoundingBox->GetWidth(), pBoundingBox->GetHeight(), pBoundingBox->GetLength());
		} break;

		case ObjectState::MassDistributionType::SPHERE: {
			BoundingSphere *pBoundingSphere = dynamic_cast<BoundingSphere*>(pDimObj->GetBoundingVolume().get());
			CN(pBoundingSphere);

			return SetInertiaTensorSphere(pBoundingSphere->GetRadius());
		} break;

		case ObjectState::MassDistributionType::QUAD: {
			BoundingQuad *pBoundingQuad = dynamic_cast<BoundingQuad*>(pDimObj->GetBoundingVolume().get());
			CN(pBoundingQuad);

			return SetInertiaTensorQuad(pBoundingQuad->GetWidth(), pBoundingQuad->GetHeight());
		} break;

		case ObjectState::MassDistributionType::CUSTOM: {
			// TODO: ?
		} break;
	}

Error:
	return r;
}

RESULT ObjectState::SetInertiaTensorSphere(point_precision radius) {
	RESULT r = R_SUCCESS;

	matrix<point_precision, 3, 3> matInertiaTensor;
	matInertiaTensor.clear();

	point_precision momentOfIntertia = (2.0f / 5.0f)* m_kgMass * radius;

	matInertiaTensor.element(0, 0) = momentOfIntertia;
	matInertiaTensor.element(1, 1) = momentOfIntertia;
	matInertiaTensor.element(2, 2) = momentOfIntertia;

	CR(SetInertiaTensor(ObjectState::MassDistributionType::SPHERE, matInertiaTensor));

Error:
	return r;
}

RESULT ObjectState::SetInertiaTensorVolume(point_precision width, point_precision height, point_precision depth) {
	RESULT r = R_SUCCESS;

	matrix<point_precision, 3, 3> matInertiaTensor;
	matInertiaTensor.clear();

	matInertiaTensor.element(0, 0) = (1.0f / 12.0f) * m_kgMass * ((height * height) + (depth * depth));
	matInertiaTensor.element(1, 1) = (1.0f / 12.0f) * m_kgMass * ((width * width) + (depth * depth));
	matInertiaTensor.element(2, 2) = (1.0f / 12.0f) * m_kgMass * ((width * width) + (height * height));

	CR(SetInertiaTensor(ObjectState::MassDistributionType::VOLUME, matInertiaTensor));

Error:
	return r;
}

RESULT ObjectState::SetInertiaTensorQuad(point_precision width, point_precision height) {
	RESULT r = R_SUCCESS;

	matrix<point_precision, 3, 3> matInertiaTensor;
	matInertiaTensor.clear();

	matInertiaTensor.element(0, 0) = (1.0f / 12.0f) * m_kgMass * (height * height);
	matInertiaTensor.element(1, 1) = (1.0f / 12.0f) * m_kgMass * (width * width);
	matInertiaTensor.element(2, 2) = (1.0f / 12.0f) * m_kgMass * ((width * width) + (height * height));

	CR(SetInertiaTensor(ObjectState::MassDistributionType::QUAD, matInertiaTensor));

Error:
	return r;
}

RESULT ObjectState::Translate(vector vTranslation) {
	if(m_fImmovable == false)
		m_ptOrigin += vTranslation;

	return R_SUCCESS;
}

RESULT ObjectState::translate(vector v) {
	if (m_fImmovable == false)
		m_ptPreviousOrigin = m_ptOrigin;
		m_ptOrigin.translate(v);

	return R_SUCCESS;
}

const vector ObjectState::GetVelocity() { 
	return m_vVelocity; 
}

const vector ObjectState::GetInstantVelocity() {
	return vector(m_ptOrigin - m_ptPreviousOrigin);
}

const vector ObjectState::GetAngularVelocity() {
	return m_vAngularVelocity;
}

const vector ObjectState::GetVelocityAtPoint(point ptRefObj) {
	vector vRefObjCenterOfMass = ptRefObj - m_ptCenterOfMass;
	vector vAtPoint = m_vVelocity + m_vAngularVelocity.cross(vRefObjCenterOfMass);
	return vAtPoint;
}

const vector ObjectState::GetMomentum() {
	return m_vMomentum;
}

const quaternion ObjectState::GetRotation() { 
	return m_qRotation; 
}

const vector ObjectState::GetAngularMomentum()
{ 
	return m_vAngularMomentum; 
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

	// Forces will not get applied to immovable objects
	if (m_fImmovable == false) {

		// External Forces
		if (externalForceGenerators.size() > 0) {
			for (auto &forceGenerator : externalForceGenerators) {
				if(forceGenerator->IsEnabled())
					derivativeOutput.m_vForce += forceGenerator->GenerateForce(this, timeStart, timeDelta);
			}
		}

		// Internal Forces
		if (m_forceGenerators.size() > 0) {
			for (auto &forceGenerator : m_forceGenerators) {
				if (forceGenerator->IsEnabled())
					derivativeOutput.m_vForce += forceGenerator->GenerateForce(this, timeStart, timeDelta);
			}
		}
	}
	
	//output.dv = acceleration(state, t + dt);
	return derivativeOutput;
}


RESULT ObjectState::ApplyForceAtPoint(vector vForce, point ptRefObj, double msDeltaTime) {
	vector vRefObjCenterOfMass = ptRefObj - m_ptCenterOfMass;

	m_vMomentum += vForce * msDeltaTime;
	m_vAngularMomentum += vForce.cross(vRefObjCenterOfMass) * msDeltaTime;
	
	RecalculateLinearVelocity();
	RecalculateAngularVelocity();

	return R_SUCCESS;
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
	// Position
	m_vMomentum += force * timeDelta;
	RecalculateLinearVelocity();

	m_ptOrigin += m_vVelocity * timeDelta;
	
	// Angular
	// TODO: Add rotational momentum
	
	RecalculateAngularVelocity();
	quaternion qSpin = (m_qSpin * timeDelta);
	m_qRotation = m_qRotation + qSpin;
	m_qRotation.Normalize();

	if (force.IsZero() == false || m_vVelocity.IsZero() == false || m_qSpin.IsZero() == false) {
		SetDirty();
	}

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
	RecalculateLinearVelocity();

	m_ptOrigin += m_vVelocity * timeDelta;

	if (!objDerivative.m_vForce.IsZero() || !m_vVelocity.IsZero()) {
		SetDirty();
	}

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
