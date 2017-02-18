#include "InteractionEngine.h"

#include "Scene/ObjectStore.h"

InteractionEngine::InteractionEngine() :
	m_pCollisionDetector(nullptr),
	m_pCollisionResolver(nullptr)
{
	// empty
}

RESULT InteractionEngine::Initialize() {
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

Error:
	return r;
}

std::unique_ptr<InteractionEngine> InteractionEngine::MakeEngine() {
	RESULT r = R_PASS;
	auto pInteractionEngine = std::unique_ptr<InteractionEngine>(new InteractionEngine());
	CNM(pInteractionEngine, "Failed to allocate new interaction engine");

	CRM(pInteractionEngine->Initialize(), "Failed to initialize interaction engine");

	//Success:
	return pInteractionEngine;

Error:
	return nullptr;
}

RESULT InteractionEngine::SetInteractionGraph(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	//CR(m_pPhysicsIntegrator->SetPhysicsStore(pObjectStore));

//Error:
	return r;
}

// TODO: This is deprecated
RESULT InteractionEngine::Update() {
	RESULT r = R_PASS;

	// TODO: add the cascade here

//Error:
	return r;
}

// TODO: This is temporary
RESULT InteractionEngine::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	// Update current states
	//CR(m_pPhysicsIntegrator->Update());

	// Detect Collisions
	CR(m_pCollisionDetector->UpdateObjectStore(pObjectStore));

	// Resolver will be automatically notified per the pub-sub design
	// TODO: We might want to rethink this for performance reasons

Error:
	return r;
}


RESULT InteractionEngine::RegisterSubscriber(CollisionGroupEventType collisionGroupEvent, Subscriber<CollisionGroupEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	CR(m_pCollisionDetector->RegisterSubscriber(collisionGroupEvent, pCollisionDetectorSubscriber));

Error:
	return r;
}

RESULT InteractionEngine::RegisterObjectCollisionSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	CR(m_pCollisionDetector->RegisterObjectAndSubscriber(pVirtualObject, pCollisionDetectorSubscriber));

Error:
	return r;
}