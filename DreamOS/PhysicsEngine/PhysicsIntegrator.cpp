#include "PhysicsIntegrator.h"

#include "Scene/ObjectStore.h"

PhysicsIntegrator::PhysicsIntegrator() :
	m_pPhysicsObjectStore(nullptr)
{
	// empty
}

RESULT PhysicsIntegrator::Update() {
	RESULT r = R_PASS;

	auto timeNow = std::chrono::high_resolution_clock::now();
	auto timeDelta = std::chrono::duration<double>(timeNow - m_lastUpdateTime).count();
	m_lastUpdateTime = timeNow;

	m_elapsedTime += timeDelta;

	while (m_elapsedTime >= m_timeStep) {
		m_elapsedTime = m_elapsedTime - m_timeStep;

		// TODO: Evaluate
	}

//Error:
	return r;
}

RESULT PhysicsIntegrator::Initialize() {
	RESULT r = R_PASS;

	// TODO: 

//Error:
	return r;
}

RESULT PhysicsIntegrator::SetTimeStep(double msTimeStep) {
	RESULT r = R_PASS;

	CBM((msTimeStep > MINIMUM_TIME_STEP), "Cannot set time step below minimum %f", (float)(MINIMUM_TIME_STEP));

	// Convert to ms
	m_timeStep = (msTimeStep / 1000.0f);

Error:
	return r;
}

RESULT PhysicsIntegrator::SetPhysicsStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	CB((m_pPhysicsObjectStore == nullptr));
	CN(pObjectStore);

	m_pPhysicsObjectStore = pObjectStore;

Error:
	return r;
}