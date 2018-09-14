#include "AirResistanceGenerator.h"
#include "Primitives/ObjectState.h"

RESULT AirResistanceGenerator::Update() {
	RESULT r = R_SUCCESS;

	// do stuff

	//Error:
	return r;
}

vector AirResistanceGenerator::GenerateForce(ObjectState *pObjectState, double startTime, double deltaTime) {
	// F = kv^2

	vector vVelocity = pObjectState->GetVelocity();
	double airResistanceMagnitude = m_kConstant * vVelocity.magnitudeSquared();
	vector airResistanceForce = vector(-airResistanceMagnitude, -airResistanceMagnitude, -airResistanceMagnitude) * vVelocity;
	DEBUG_LINEOUT("Velocity: X:%0.8f Y:%0.8f Z:%0.8f", vVelocity.x(), vVelocity.y(), vVelocity.z());
	DEBUG_LINEOUT("ResistanceMag: %0.8f", airResistanceMagnitude);

	return airResistanceForce;
}
