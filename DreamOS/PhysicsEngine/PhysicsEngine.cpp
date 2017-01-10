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

	// Register the collisions for the resolver
	// TODO: Do we want this over hard coding?
	CR(m_pCollisionDetector->RegisterSubscriber(OBJECT_GROUP_COLLISION, m_pCollisionResolver.get()));

	// Physics integrator
	m_pPhysicsIntegrator = std::unique_ptr<PhysicsIntegrator>(new PhysicsIntegrator());
	CNM(m_pPhysicsIntegrator, "Failed to create phystics integrator module");

	CRM(m_pPhysicsIntegrator->Initialize(), "Failed to initialize Physics Integrator");

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

RESULT PhysicsEngine::SetPhysicsGraph(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	CR(m_pPhysicsIntegrator->SetPhysicsStore(pObjectStore));

Error:
	return r;
}

// TODO: This is deprecated
RESULT PhysicsEngine::Update() {
	RESULT r = R_PASS;
	
	// TODO: add the cascade here

	CR(m_pPhysicsIntegrator->Update());

Error:
	return r;
}

// TODO: This is temporary
RESULT PhysicsEngine::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	// Detect Collisions
	CR(m_pCollisionDetector->UpdateObjectStore(pObjectStore));

	// Update current states
	CR(m_pPhysicsIntegrator->Update());

	// Resolver will be automatically notified per the pub-sub design
	// TODO: We might want to rethink this for performance reasons

Error:
	return r;
}


RESULT PhysicsEngine::RegisterSubscriber(CollisionGroupEventType collisionGroupEvent, Subscriber<CollisionGroupEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	CR(m_pCollisionDetector->RegisterSubscriber(collisionGroupEvent, pCollisionDetectorSubscriber));

Error:
	return r;
}

RESULT PhysicsEngine::RegisterObjectCollisionSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	CR(m_pCollisionDetector->RegisterObjectAndSubscriber(pVirtualObject, pCollisionDetectorSubscriber));

Error:
	return r;
}