#include "AirResistanceGenerator.h"

#include "core/primitives/ObjectState.h"

RESULT AirResistanceGenerator::Update() {
	RESULT r = R_SUCCESS;

	// do stuff

	//Error:
	return r;
}

vector AirResistanceGenerator::GenerateForce(ObjectState *pObjectState, double startTime, double deltaTime) {
	// F = kv^2

	vector vVelocity = pObjectState->GetVelocity();
	float airResistanceMagnitude = m_kConstant * vVelocity.magnitudeSquared() + m_minConstant;
	vector airResistanceForce = (vector)(-airResistanceMagnitude * vVelocity);
	//airResistanceForce.Print("air resistance force");
	//DEBUG_LINEOUT("Velocity: X:%0.8f Y:%0.8f Z:%0.8f", vVelocity.x(), vVelocity.y(), vVelocity.z());
	//DEBUG_LINEOUT("ResistanceMag: %0.8f", airResistanceMagnitude);

	return airResistanceForce;
}
