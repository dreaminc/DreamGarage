#include "InteractionEngine.h"

#include "Scene/ObjectStore.h"

#include "PhysicsEngine/CollisionManifold.h"

InteractionEngine::InteractionEngine() {
	// empty
}

RESULT InteractionEngine::Initialize() {
	RESULT r = R_PASS;

	m_pInteractionRay = std::make_shared<ray>();
	CN(m_pInteractionRay);

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

RESULT InteractionEngine::UpdateInteractionPrimitive(const ray &rCast) {
	RESULT r = R_PASS;

	CN(m_pInteractionRay);

	// Copy the ray in
	m_pInteractionRay = std::make_shared<ray>(rCast);
	CN(m_pInteractionRay);

Error:
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

	std::vector<std::shared_ptr<ActiveObject>> activeObjectsToRemove;

	// Check interaction primitives against object store
	/*
	if (IsEventRegistered(ELEMENT_INTERSECT_BEGAN)) {
		for (auto &objCollisionGroup : pObjectStore->GetSceneGraphStore()->GetObjectCollisionGroups()) {
			CollisionGroupEvent collisionGroupEvent(OBJECT_GROUP_COLLISION, objCollisionGroup);
			Publisher<CollisionGroupEventType, CollisionGroupEvent>::NotifySubscribers(OBJECT_GROUP_COLLISION, &collisionGroupEvent);
		}
	}
	*/

	// TODO: First pass (no state tracking yet)
	// Set all objects to non-intersected - below will set it to intersected, then remaining
	// non-intersected objects are clearly no longer in the active set
	CR(SetAllActiveObjectStates(ActiveObject::state::NOT_INTERSECTED));

	if (m_pInteractionRay != nullptr) {
		for (auto &pObject : pObjectStore->GetObjects()) {
			DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);

			if (pDimObj->Intersect(*m_pInteractionRay.get())) {
				CollisionManifold manifold = pDimObj->Collide(*m_pInteractionRay.get());
				int numContacts = 0;
				VirtualObj *pObj = nullptr;

				if ((numContacts = manifold.NumContacts()) > 0) {

					/*
					InteractionObjectEvent interactionEvent(ELEMENT_INTERSECT_BEGAN, m_pInteractionRay, pObject);
					for (int i = 0; i < numContacts; i++)
						interactionEvent.AddPoint(manifold.GetContactPoint(i));

					CR(NotifySubscribers(ELEMENT_INTERSECT_BEGAN, &interactionEvent));
					*/
					
					// Manifold should return only one object, one will be nullptr
					pObj = manifold.GetObjectA();
					if(pObj == nullptr)
						pObj = manifold.GetObjectB();
					CN(pObj);

					// Check for active object
					auto pActiveObject = FindActiveObject(pObj);

					if (pActiveObject == nullptr) {
						pActiveObject = AddActiveObject(manifold.GetObjectA());
						CN(pActiveObject)
						// Notify element intersect begin
					}
					
					switch (pActiveObject->GetState()) {
						case ActiveObject::state::NOT_INTERSECTED: {
							// Notify start
							pActiveObject->SetState(ActiveObject::state::INTERSECTED);
						} break;

						case ActiveObject::state::INTERSECTED: {
							// Notify continue
							pActiveObject->SetState(ActiveObject::state::INTERSECTED);
						} break;
					}
					
				}
			}
		}
	}
	
	
	for (auto &pActiveObject : m_activeObjects) {
		if (pActiveObject->GetState() == ActiveObject::state::NOT_INTERSECTED) {
			// Add to remove list
			activeObjectsToRemove.push_back(pActiveObject);
		}
	}

	for (auto &pActiveObject : activeObjectsToRemove) {
		// Notify no longer intersected
		CR(RemoveActiveObject(pActiveObject));
	}
	

Error:
	return r;
}

RESULT InteractionEngine::SetAllActiveObjectStates(ActiveObject::state newState) {
	RESULT r = R_PASS;

	for (auto &pActiveObject : m_activeObjects) {
		CR(pActiveObject->SetState(newState));
	}

Error:
	return r;
}

// Active Objects
RESULT InteractionEngine::ClearActiveObjects() {
	m_activeObjects.clear();
	return R_PASS;
}

std::shared_ptr<ActiveObject>  InteractionEngine::AddActiveObject(VirtualObj *pVirtualObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = nullptr;

	CBM((FindActiveObject(pVirtualObject) == nullptr), "Active Object already active");

	pNewActiveObject = std::make_shared<ActiveObject>(pVirtualObject);
	CN(pNewActiveObject);
	pNewActiveObject->SetState(ActiveObject::state::NOT_INTERSECTED);

	m_activeObjects.push_back(pNewActiveObject);

// Success:
	return pNewActiveObject;

Error:
	return nullptr;
}

RESULT InteractionEngine::RemoveActiveObject(std::shared_ptr<ActiveObject> pActiveObject) {
	RESULT r = R_PASS;

	std::shared_ptr<ActiveObject> pNewActiveObject = FindActiveObject(pActiveObject);
	CNM(pNewActiveObject, "Active object not found");

	m_activeObjects.remove(pNewActiveObject);

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

std::shared_ptr<ActiveObject> InteractionEngine::FindActiveObject(std::shared_ptr<ActiveObject> pActiveObject) {
	for (auto it = m_activeObjects.begin(); it != m_activeObjects.end(); it++) {
		if ((*it) == pActiveObject) {
			return (*it);
		}
	}

	return nullptr;
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
