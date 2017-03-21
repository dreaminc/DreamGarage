#include "Integrator.h"

#include "Scene/ObjectStore.h"

Integrator::Integrator() :
	m_pObjectStore(nullptr)
{
	// empty
}

RESULT Integrator::Initialize() {
	RESULT r = R_PASS;

	m_lastUpdateTime = std::chrono::high_resolution_clock::now();

//Error:
	return r;
}

// This actually integrates the time step
RESULT Integrator::Update() {
	RESULT r = R_PASS;

	auto timeNow = std::chrono::high_resolution_clock::now();
	auto timeDelta = std::chrono::duration<double>(timeNow - m_lastUpdateTime).count();
	m_lastUpdateTime = timeNow;

	m_elapsedTime += timeDelta;

	if (m_elapsedTime >= m_sTimeStep) {
		//m_elapsedTime = m_elapsedTime - m_sTimeStep;

		ObjectStoreImp *pObjectStoreImp = m_pObjectStore->GetSceneGraphStore();
		VirtualObj *pVirtualObj = nullptr;

		m_pObjectStore->Reset();
		while ((pVirtualObj = pObjectStoreImp->GetNextObject()) != nullptr) {
			double adjTimestep = m_sTimeStep / 1.0f;	// This allows us to slow/speed it up without frame rate issues
			CR(UpdateObject(pVirtualObj, adjTimestep));
		}

		m_elapsedTime = 0.0f;
	}

Error:
	return r;
}

RESULT Integrator::SetObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	CB((m_pObjectStore == nullptr));
	CN(pObjectStore);

	m_pObjectStore = pObjectStore;

Error:
	return r;
}

RESULT Integrator::SetTimeStep(double msTimeStep) {
	RESULT r = R_PASS;

	CBM((msTimeStep > MINIMUM_TIME_STEP), "Cannot set time step below minimum %f", (float)(MINIMUM_TIME_STEP));

	// Convert to ms
	m_sTimeStep = (msTimeStep / 1000.0f);

Error:
	return r;
}