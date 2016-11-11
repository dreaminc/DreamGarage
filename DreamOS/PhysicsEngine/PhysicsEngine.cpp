#include "PhysicsEngine.h"

#include "Scene/ObjectStore.h"

PhysicsEngine::PhysicsEngine() :
	m_pCollisionDetector(nullptr),
	m_pCollisionResolver(nullptr)
{
	 // empty
}

RESULT PhysicsEngine::Initialize() {
	RESULT r = R_PASS;

	// Collision Detector
	m_pCollisionDetector = std::unique_ptr<CollisionDetector>(new CollisionDetector());
	CNM(m_pCollisionDetector, "Failed to create collision detector module");
	
	CRM(m_pCollisionDetector->Initialize(), "Failed to initialize Collision Detector");

	// Collision Resolver
	m_pCollisionResolver = std::unique_ptr<CollisionResolver>(new CollisionResolver());
	CNM(m_pCollisionResolver, "Failed to create collision resolver module");

	CRM(m_pCollisionResolver->Initialize(), "Failed to initialize Collision Resolver");

	// TODO: Physics integrator

Error:
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

	CR(m_pCollisionDetector->UpdateObjectStore(pObjectStore));

	// TODO: Resolver, integrator 

Error:
	return r;
}