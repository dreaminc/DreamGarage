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
			double adjTimestep = m_sTimeStep / 1.0f;	// This allows us to slow/speed it up without frame rate issues
			CR(UpdateObject(pVirtualObj, adjTimestep));
		}

		m_elapsedTime = 0.0f;
	}

Error:
	return r;
}

RESULT PhysicsIntegrator::UpdateObject(VirtualObj *pVirtualObj, double msTimeStep) {
	RESULT r = R_PASS;

	// Handle Children
	DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

	if (pDimObj != nullptr && pDimObj->HasChildren()) {
		for (auto &pVirtualChildObj : pDimObj->GetChildren()) {
			CR(UpdateObject(pVirtualChildObj.get(), msTimeStep));
		}
	}
	else {
		pVirtualObj->IntegrateState<ObjectState::IntegrationType::RK4>(0.0f, msTimeStep, m_globalForceGenerators);
	}

Error:
	return r;
}

// TODO: Expose gravity to DreamOS
RESULT PhysicsIntegrator::Initialize() {
	RESULT r = R_PASS;

	m_lastUpdateTime = std::chrono::high_resolution_clock::now();

	// Set up Gravity by default TODO: Move to engine?
	//CR(AddGlobalForceGenerator(ForceGeneratorFactory::MakeForceGenerator(FORCE_GENERATOR_GRAVITY)));

//Error:
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

RESULT PhysicsIntegrator::AddGlobalForceGenerator(ForceGenerator *pForceGenerator) {
	RESULT r = R_SUCCESS;

	CN(pForceGenerator);

	m_globalForceGenerators.push_back(pForceGenerator);
	//CB((pForceGenerator == nullptr));

Error:
	return r;
}

RESULT PhysicsIntegrator::ClearForceGenerators() {
	RESULT r = R_SUCCESS;

	for (auto &pForceGenerator : m_globalForceGenerators) {
		if (pForceGenerator != nullptr) {
			delete pForceGenerator;
			pForceGenerator = nullptr;
		}
	}

	m_globalForceGenerators.clear();

	//Error:
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