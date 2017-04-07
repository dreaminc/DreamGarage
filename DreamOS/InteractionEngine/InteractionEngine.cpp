#include "InteractionEngine.h"
#include "Sandbox/SandboxApp.h"

#include "Scene/ObjectStore.h"
#include "AnimationQueue.h"

#include "PhysicsEngine/CollisionManifold.h"

InteractionEngine::InteractionEngine(SandboxApp *pSandbox) :
	m_pSandbox(pSandbox)
{
	// empty
}

RESULT InteractionEngine::Initialize() {
	RESULT r = R_PASS;

	// Register Events
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		RegisterEvent((InteractionEventType)(i));
	}

	// Ray
	m_pInteractionRay = std::make_shared<ray>();
	CN(m_pInteractionRay);

	m_pObjectQueue = new AnimationQueue();

	CR(RegisterSenseMouse());

Error:
	return r;
}

// TODO: Move to sandbox implementation
RESULT InteractionEngine::RegisterSenseController(SenseController* pSenseController) {
	RESULT r = R_PASS;

	CN(pSenseController);

	CR(pSenseController->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_MENU_UP, this));
	CR(pSenseController->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_TRIGGER_UP, this));
	CR(pSenseController->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_TRIGGER_DOWN, this));
	CR(pSenseController->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_PAD_MOVE, this));

Error:
	return r;
}

RESULT InteractionEngine::RegisterSenseMouse() {
	RESULT r = R_PASS;

	CN(m_pSandbox);

	CR(m_pSandbox->RegisterSubscriber(SENSE_MOUSE_MOVE, this));
	CR(m_pSandbox->RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_UP, this));
	CR(m_pSandbox->RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_DOWN, this));
	CR(m_pSandbox->RegisterSubscriber(SENSE_MOUSE_WHEEL, this));

Error:
	return r;
}

std::unique_ptr<InteractionEngine> InteractionEngine::MakeEngine(SandboxApp *pSandbox) {
	RESULT r = R_PASS;

	CN(pSandbox);

	{

		auto pInteractionEngine = std::unique_ptr<InteractionEngine>(new InteractionEngine(pSandbox));

		CNM(pInteractionEngine, "Failed to allocate new interaction engine");
		CRM(pInteractionEngine->Initialize(), "Failed to initialize interaction engine");

		//Success:
		return pInteractionEngine;
	}

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

RESULT InteractionEngine::SetInteractionDiffThreshold(double thresh) {
	RESULT r = R_PASS;

	CRANGE(thresh, 0.0f, 1.0f);
	m_diffThreshold = thresh;

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

RESULT InteractionEngine::UpdateAnimationQueue() {
	RESULT r = R_PASS;

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	m_pObjectQueue->Update(msNow);
//Error:
	return r;
}

AnimationQueue* InteractionEngine::GetAnimationQueue() {
	return m_pObjectQueue;
}

RESULT InteractionEngine::PushAnimationItem(VirtualObj *pObj,
	point ptPosition,
	vector vScale,
	double duration,
	AnimationItem::AnimationFlags flags) {

	RESULT r = R_PASS;

	AnimationState endState;
	endState.ptPosition = ptPosition;
	endState.vScale = vScale;

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	CR(m_pObjectQueue->PushAnimationItem(pObj, endState, msNow, duration, flags));

Error:
	return r;
}

RESULT InteractionEngine::CancelAnimation(VirtualObj *pObj) {
	RESULT r = R_PASS;

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	CR(m_pObjectQueue->CancelAnimation(pObj, msNow));
Error:
	return r;
}

// TODO: Tidy up / test this code
RESULT InteractionEngine::UpdateInteractionRay() {
	RESULT r = R_PASS;

	ray rCast;
	hand *pRightHand = m_pSandbox->GetHand(hand::HAND_TYPE::HAND_RIGHT);

	if (pRightHand != nullptr) {
		point ptHand = pRightHand->GetPosition();

		//GetLookVector
		quaternion qHand = pRightHand->GetHandState().qOrientation;
		qHand.Normalize();

		/*
		if (m_pTestRayController != nullptr) {
		m_pTestRayController->SetPosition(ptHand);
		m_pTestRayController->SetOrientation(qHand);
		}
		//*/

		//TODO: investigate how to properly get look vector for controllers
		//vector vHandLook = qHand.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();

		vector vHandLook = RotationMatrix(qHand) * vector(0.0f, 0.0f, -1.0f);
		vHandLook.Normalize();

		vector vCast = vector(-vHandLook.x(), -vHandLook.y(), vHandLook.z());
		/*
		if (m_pTestRayController != nullptr) {
		m_pTestRayLookV->SetPosition(ptHand);
		m_pTestRayLookV->SetOrientation(quaternion(vHandLook));
		}
		//*/
		// Accommodate for composite collision bug
		//ptHand = ptHand + point(-10.0f * vCast);
		//rCast = ray(ptHand, vCast);
		//rCast = m_pTestRayController->GetRayFromVerts();

		rCast = ray(ptHand, vHandLook);
	}
	/*
	// This is handled by mouse move events
	else {
		CR(m_pSandbox->GetMouseRay(rCast, 0.0f));
	}
	*/
	
	CR(UpdateInteractionPrimitive(rCast));

// Success:
	return r;

Error:
	UpdateInteractionPrimitive(ray(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, 0.0f)));
	return r;
}

// TODO: This is temporary
RESULT InteractionEngine::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<ActiveObject>> activeObjectsToRemove;


	// TODO: This should move up into a generic update call
	CR(UpdateInteractionRay());

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

					// Manifold should return only one object, one will be nullptr
					pObj = manifold.GetObjectA();
					if(pObj == nullptr)
						pObj = manifold.GetObjectB();
					CN(pObj);

					// Check for active object
					auto pActiveObject = FindActiveObject(pObj);

					if (pActiveObject == nullptr) {
						pActiveObject = AddActiveObject(manifold.GetObjectA());
						CN(pActiveObject);

						pActiveObject->SetContactPoint(manifold.GetContactPoint(0));

						// Notify element intersect begin
						InteractionObjectEvent interactionEvent(InteractionEventType::ELEMENT_INTERSECT_BEGAN, m_pInteractionRay, pObj);
						for(int i = 0; i < manifold.NumContacts(); i++)
							interactionEvent.AddPoint(manifold.GetContactPoint(i));
						NotifySubscribers(InteractionEventType::ELEMENT_INTERSECT_BEGAN, &interactionEvent);
					}
					else {
						vector vDiff = manifold.GetContactPoint(0).GetPoint() - pActiveObject->GetIntersectionPoint();

						if (vDiff.magnitude() > m_diffThreshold) {
							pActiveObject->SetContactPoint(manifold.GetContactPoint(0));

							// Notify element intersect continue
							InteractionObjectEvent interactionEvent(InteractionEventType::ELEMENT_INTERSECT_MOVED, m_pInteractionRay, pObj);
							for (int i = 0; i < manifold.NumContacts(); i++)
								interactionEvent.AddPoint(manifold.GetContactPoint(i));
							NotifySubscribers(InteractionEventType::ELEMENT_INTERSECT_MOVED, &interactionEvent);
						}
					}

					pActiveObject->SetState(ActiveObject::state::INTERSECTED);
				}
			}
		}
	}


	for (auto &pActiveObject : m_activeObjects) {
		// Add to remove list if not intersected in current frame
		if (pActiveObject->GetState() == ActiveObject::state::NOT_INTERSECTED) {
			activeObjectsToRemove.push_back(pActiveObject);
		}
	}

	for (auto &pActiveObject : activeObjectsToRemove) {
		// Notify no longer intersected
		CR(RemoveActiveObject(pActiveObject));

		// Notify element intersect continue
		// This uses the last available point
		// TODO: Add projection , find exit point, do we need that?
		InteractionObjectEvent interactionEvent(InteractionEventType::ELEMENT_INTERSECT_ENDED, m_pInteractionRay, pActiveObject->GetObject());
		interactionEvent.AddPoint(pActiveObject->GetIntersectionPoint(), pActiveObject->GetIntersectionNormal());
		NotifySubscribers(InteractionEventType::ELEMENT_INTERSECT_ENDED, &interactionEvent);
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

std::shared_ptr<ActiveObject> InteractionEngine::AddActiveObject(VirtualObj *pVirtualObject) {
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

RESULT InteractionEngine::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	//TODO:  Expand this to accommodate for left controller
	if(pEvent->state.type == CONTROLLER_RIGHT) {
		switch (pEvent->type) {
			case SENSE_CONTROLLER_TRIGGER_UP: {
				for (auto &pObject : m_activeObjects) {
					if (pObject->GetState() == ActiveObject::state::INTERSECTED) {
						InteractionEventType type = INTERACTION_EVENT_SELECT_UP;
						InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
						CR(NotifySubscribers(type, &interactionEvent));
					}
				}
			} break;

			case SENSE_CONTROLLER_TRIGGER_DOWN: {
				for (auto &pObject : m_activeObjects) {
					if (pObject->GetState() == ActiveObject::state::INTERSECTED) {
						InteractionEventType type = INTERACTION_EVENT_SELECT_DOWN;
						InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
						CR(NotifySubscribers(type, &interactionEvent));
					}
				}
			} break;

			case SENSE_CONTROLLER_MENU_UP: {
				for (auto &pObject : m_activeObjects) {
					if (pObject->GetState() == ActiveObject::state::INTERSECTED) {
						InteractionEventType type = INTERACTION_EVENT_MENU;
						InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
						CR(NotifySubscribers(type, &interactionEvent));
					}
				}
			} break;

			case SENSE_CONTROLLER_PAD_MOVE: {
				// Keeps decimal in accumulator, moves value into touchY
				m_interactionPadAccumulator += pEvent->state.ptTouchpad.y();
				double touchY = m_interactionPadAccumulator;
				m_interactionPadAccumulator = std::modf(m_interactionPadAccumulator, &touchY);

				for (auto &pObject : m_activeObjects) {
					//ray rCast;
					//m_pSandbox->GetMouseRay(rCast, 0.0f);
					//CR(UpdateInteractionPrimitive(rCast));

					InteractionEventType type = INTERACTION_EVENT_WHEEL;
					InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
					interactionEvent.SetValue((int)(touchY));

					CR(NotifySubscribers(type, &interactionEvent));
				}
			} break;
		}
	}

Error:
	return r;
}


RESULT InteractionEngine::Notify(SenseMouseEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->EventType) {
		case SENSE_MOUSE_MOVE: {
			ray rCast;
			m_pSandbox->GetMouseRay(rCast, 0.0f);
			CR(UpdateInteractionPrimitive(rCast));
		} break;

		case SENSE_MOUSE_LEFT_BUTTON_UP: {
			for (auto &pObject : m_activeObjects) {
				//ray rCast;
				//m_pSandbox->GetMouseRay(rCast, 0.0f);
				//CR(UpdateInteractionPrimitive(rCast));

				InteractionEventType type = INTERACTION_EVENT_SELECT_UP;
				InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
				CR(NotifySubscribers(type, &interactionEvent));
			}
		} break;

		case SENSE_MOUSE_LEFT_BUTTON_DOWN: {
			for (auto &pObject : m_activeObjects) {
				//ray rCast;
				//m_pSandbox->GetMouseRay(rCast, 0.0f);
				//CR(UpdateInteractionPrimitive(rCast));

				InteractionEventType type = INTERACTION_EVENT_SELECT_DOWN;
				InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
				CR(NotifySubscribers(type, &interactionEvent));
			}
		} break;

		case SENSE_MOUSE_WHEEL: {
			for (auto &pObject : m_activeObjects) {
				//ray rCast;
				//m_pSandbox->GetMouseRay(rCast, 0.0f);
				//CR(UpdateInteractionPrimitive(rCast));

				InteractionEventType type = INTERACTION_EVENT_WHEEL;
				InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
				interactionEvent.SetValue(pEvent->state * 10);

				CR(NotifySubscribers(type, &interactionEvent));
			}
		} break;
	}

Error:
	return r;
}
