#include "PhysicsIntegrator.h"

#include "Scene/ObjectStore.h"
#include "ForceGeneratorFactory.h"

PhysicsIntegrator::PhysicsIntegrator() :
	m_pPhysicsObjectStore(nullptr)
{
	// empty
}


// This actually integrates the time step
RESULT PhysicsIntegrator::Update() {
	RESULT r = R_PASS;

	auto timeNow = std::chrono::high_resolution_clock::now();
	auto timeDelta = std::chrono::duration<double>(timeNow - m_lastUpdateTime).count();
	m_lastUpdateTime = timeNow;

	m_elapsedTime += timeDelta;

	if (m_elapsedTime >= m_sTimeStep) {
		//m_elapsedTime = m_elapsedTime - m_sTimeStep;

		ObjectStoreImp *pObjectStoreImp = m_pPhysicsObjectStore->GetSceneGraphStore();
		VirtualObj *pVirtualObj = nullptr;

		m_pPhysicsObjectStore->Reset();
		while ((pVirtualObj = pObjectStoreImp->GetNextObject()) != nullptr) {
			//DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

			// Euler Integration 
			// TODO: switch to RK4)
			vector vPositionInc = static_cast<float>(m_sTimeStep) * (pVirtualObj->m_vVelocity);
			vector vAccelInc = static_cast<float>(m_sTimeStep) * (pVirtualObj->m_vAcceleration);

			// TODO: Legitimize gravity force generator later
			// TODO: Add drag too
			vAccelInc += static_cast<float>(m_sTimeStep) * (vector(0.0f, DEFAULT_GRAVITY_ACCEL, 0.0f));

			point ptNewPosition = pVirtualObj->m_ptOrigin + vPositionInc;
			vector vNewVelocity = pVirtualObj->m_vVelocity + vAccelInc;

			pVirtualObj->SetPosition(ptNewPosition);
			pVirtualObj->SetVelocity(vNewVelocity);
		}

		m_elapsedTime = 0.0f;
	}

//Error:
	return r;
}

RESULT PhysicsIntegrator::Initialize() {
	RESULT r = R_PASS;

	m_lastUpdateTime = std::chrono::high_resolution_clock::now();

	// Set up Gravity by default
	CR(AddGlobalForceGenerator(ForceGeneratorFactory::MakeForceGenerator(FORCE_GENERATOR_GRAVITY)));

Error:
	return r;
}

RESULT PhysicsIntegrator::SetTimeStep(double msTimeStep) {
	RESULT r = R_PASS;

	CBM((msTimeStep > MINIMUM_TIME_STEP), "Cannot set time step below minimum %f", (float)(MINIMUM_TIME_STEP));

	// Convert to ms
	m_sTimeStep = (msTimeStep / 1000.0f);

Error:
	return r;
}

RESULT PhysicsIntegrator::AddGlobalForceGenerator(std::unique_ptr<ForceGenerator> pForceGenerator) {
	RESULT r = R_SUCCESS;

	m_globalForceGenerators.push_back(std::move(pForceGenerator));
	CB((pForceGenerator == nullptr));

Error:
	return r;
}

RESULT PhysicsIntegrator::ClearForceGenerators() {
	m_globalForceGenerators.clear();
	return R_SUCCESS;
}

RESULT PhysicsIntegrator::SetPhysicsStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	CB((m_pPhysicsObjectStore == nullptr));
	CN(pObjectStore);

	m_pPhysicsObjectStore = pObjectStore;

Error:
	return r;
}