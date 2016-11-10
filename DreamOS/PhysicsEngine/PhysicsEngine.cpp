#include "PhysicsEngine.h"

#include "Scene/ObjectStore.h"

PhysicsEngine::PhysicsEngine() :
	m_collisionDetector(nullptr),
	m_collisionResolver(nullptr)
{
	 // empty
}

RESULT PhysicsEngine::Initialize() {
	RESULT r = R_PASS;

//Error:
	return r;
}

std::unique_ptr<PhysicsEngine> PhysicsEngine::MakePhysicsEngine() {
	RESULT r = R_PASS;
	auto pPhysicsEngine = std::unique_ptr<PhysicsEngine>(new PhysicsEngine());
	CNM(pPhysicsEngine, "Failed to allocate new physics engine");

	CRM(pPhysicsEngine->Initialize(), "Failed to initialize physics engine");

//Success:
	return pPhysicsEngine;

Error:
	return nullptr;
}

// TODO: This is temporary
RESULT PhysicsEngine::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	for (auto &pObject : pObjectStore->GetObjects()) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);

		if (pDimObj != nullptr) {
			pDimObj->SetColor(color(COLOR_WHITE));
		}
	}

	for (auto &objCollisionGroup : pObjectStore->GetSceneGraphStore()->GetObjectCollisionGroups()) {
		// Handle collisions

		for (auto &pObject : objCollisionGroup) {
			DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
			pDimObj->SetColor(color(COLOR_RED));
		}
	}


//Error:
	return r;
}