#include "InteractionEngine.h"

#include "Scene/ObjectStore.h"

InteractionEngine::InteractionEngine() {
	// empty
}

RESULT InteractionEngine::Initialize() {
	RESULT r = R_PASS;

//Error:
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

//Error:
	return r;
}


RESULT InteractionEngine::RegisterSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	//CR(m_pCollisionDetector->RegisterSubscriber(collisionGroupEvent, pCollisionDetectorSubscriber));

//Error:
	return r;
}

// Active Objects
RESULT InteractionEngine::ClearActiveObjects() {
	m_activeObjects.clear();
	return R_PASS;
}

RESULT InteractionEngine::AddActiveObject(VirtualObj *pVirtualObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = nullptr;

	CBM((FindActiveObject(pVirtualObject) == nullptr), "Active Object already active");

	pNewActiveObject = std::make_shared<ActiveObject>(pVirtualObject);
	CN(pNewActiveObject);

	m_activeObjects.push_back(pNewActiveObject);

Error:
	return r;
}

RESULT InteractionEngine::RemoveActiveObject(VirtualObj *pVirtualObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = FindActiveObject(pVirtualObject);
	CNM(pNewActiveObject, "Active object not found");

	m_activeObjects.remove(pNewActiveObject);

Error:
	return r;
}

std::shared_ptr<ActiveObject> InteractionEngine::FindActiveObject(VirtualObj *pVirtualObject) {
	for (auto it = m_activeObjects.begin(); it != m_activeObjects.end(); it++) {
		if ((*it)->GetObject() == pVirtualObject) {
			return (*it);
		}
	}

	return nullptr;
}

ActiveObject::state InteractionEngine::GetActiveObjectState(VirtualObj *pVirtualObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = FindActiveObject(pVirtualObject);
	CNM(pNewActiveObject, "Active object not found");

	return pNewActiveObject->GetState();

Error:
	return ActiveObject::state::INVALID;
}