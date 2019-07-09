#include "PhysicsIntegrator.h"

#include "Scene/ObjectStore.h"
#include "ForceGeneratorFactory.h"

PhysicsIntegrator::PhysicsIntegrator() :
	Integrator()
{
	// empty
}

RESULT PhysicsIntegrator::UpdateObject(VirtualObj *pVirtualObj, double msTimeStep) {
	RESULT r = R_PASS;

	// Handle Children
	//*
	DimObj *pDimObj = dynamic_cast<DimObj*>(pVirtualObj);

	if (pDimObj != nullptr && pDimObj->HasChildren()) {
		CR(pVirtualObj->IntegrateState<ObjectState::IntegrationType::RK4>(0.0f, msTimeStep, m_globalForceGenerators));

		for (auto &pVirtualChildObj : pDimObj->GetChildren()) {
			CR(UpdateObject(pVirtualChildObj.get(), msTimeStep));
		}
	}
	else {
		if (pDimObj->GetParent() != nullptr) {
			// Null out force generators for children
			CR(pVirtualObj->IntegrateState<ObjectState::IntegrationType::RK4>(0.0f, msTimeStep, std::list<ForceGenerator*>()));
		}
		else {
			CR(pVirtualObj->IntegrateState<ObjectState::IntegrationType::RK4>(0.0f, msTimeStep, m_globalForceGenerators));
		}
	}
	//*/

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