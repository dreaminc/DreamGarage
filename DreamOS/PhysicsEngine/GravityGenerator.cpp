#include "GravityGenerator.h"
#include "Primitives/ObjectState.h"

RESULT GravityGenerator::Update() {
	RESULT r = R_SUCCESS;

	// do stuff

//Error:
	return r;
}

vector GravityGenerator::GenerateForce(ObjectState *pObjectState, double startTime, double deltaTime) {
	vector gravityForce = vector(0.0f, m_gravityAcceleration, 0.0f) * const_cast<ObjectState *>(pObjectState)->GetMass();
	return gravityForce;
}

double GravityGenerator::GetGravityAcceleration() {
	return m_gravityAcceleration;
}

RESULT GravityGenerator::SetGravityAcceleration(double gravityAcceleration) {
	m_gravityAcceleration = gravityAcceleration;
	return R_PASS;
}