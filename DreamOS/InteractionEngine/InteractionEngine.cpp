#include "InteractionEngine.h"
#include "Sandbox/SandboxApp.h"

#include "Scene/ObjectStore.h"
#include "AnimationQueue.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "Primitives/color.h"

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
	//m_pInteractionRay = std::make_shared<ray>();
	//CN(m_pInteractionRay);

	m_pObjectQueue = new AnimationQueue();

	CR(RegisterSenseMouse());
	CR(RegisterSenseKeyboard());

	CR(InitializeActiveObjectQueues());

Error:
	return r;
}

RESULT InteractionEngine::InitializeActiveObjectQueues() {
	RESULT r = R_PASS;

	for(int it = 0; it != static_cast<int>(ActiveObject::type::INVALID); it++) {
		ActiveObject::type activeObjectQueueType = static_cast<ActiveObject::type>(it);
		m_activeObjectQueues[activeObjectQueueType] = ActiveObjectQueue(activeObjectQueueType);
	}

//Error:
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

RESULT InteractionEngine::RegisterSenseKeyboard() {
	RESULT r = R_PASS;

	CN(m_pSandbox);

	// Register Dream Console to keyboard events
	CR(m_pSandbox->RegisterSubscriber(SVK_ALL, this));
	CR(m_pSandbox->RegisterSubscriber(CHARACTER_TYPING, this));
	
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

/*
RESULT InteractionEngine::UpdateInteractionPrimitive(const ray &rCast) {
	RESULT r = R_PASS;

	CN(m_pInteractionRay);

	// Copy the ray in
	m_pInteractionRay = std::make_shared<ray>(rCast);
	CN(m_pInteractionRay);

Error:
	return r;
}
*/

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

RESULT InteractionEngine::PushAnimationItem(DimObj *pObj,
	point ptPosition,
	quaternion qRotation,
	vector vScale,
	color cColor,
	double duration,
	AnimationCurveType curve,
	AnimationFlags flags,
	std::function<RESULT(void*)> startCallback,
	std::function<RESULT(void*)> endCallback,
	void* callbackContext) {

	RESULT r = R_PASS;

	AnimationState endState;
	endState.ptPosition = ptPosition;
	endState.qRotation = qRotation;
	endState.vScale = vScale;
	endState.cColor = cColor;

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	CR(m_pObjectQueue->PushAnimationItem(pObj, endState, msNow, duration, curve, flags, startCallback, endCallback, callbackContext));

Error:
	return r;
}


RESULT InteractionEngine::PushAnimationItem(DimObj *pObj,
	point ptPosition,
	quaternion qRotation,
	vector vScale,
	double duration,
	AnimationCurveType curve,
	AnimationFlags flags,
	std::function<RESULT(void*)> startCallback,
	std::function<RESULT(void*)> endCallback,
	void* callbackContext) {

	RESULT r = R_PASS;

	AnimationState endState;
	endState.ptPosition = ptPosition;
	endState.qRotation = qRotation;
	endState.vScale = vScale;
	endState.cColor = pObj->GetMaterial()->GetDiffuseColor();

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	CR(m_pObjectQueue->PushAnimationItem(pObj, endState, msNow, duration, curve, flags, startCallback, endCallback, callbackContext));

Error:
	return r;
}

RESULT InteractionEngine::PushAnimationItem(DimObj *pObj,
	color cColor,
	double duration,
	AnimationCurveType curve,
	AnimationFlags flags,
	std::function<RESULT(void*)> startCallback,
	std::function<RESULT(void*)> endCallback,
	void* callbackContext) {
	
	RESULT r = R_PASS;

	AnimationState endState;
	endState.ptPosition = pObj->GetPosition();
	endState.qRotation = pObj->GetOrientation();
	endState.vScale = pObj->GetScale();
	endState.cColor = cColor;

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	CR(m_pObjectQueue->PushAnimationItem(pObj, endState, msNow, duration, curve, flags, startCallback, endCallback, callbackContext));

Error:
	return r;
}

RESULT InteractionEngine::CancelAnimation(DimObj *pObj) {
	RESULT r = R_PASS;

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;

	CR(m_pObjectQueue->CancelAnimation(pObj, msNow));
Error:
	return r;
}

bool InteractionEngine::IsAnimating(DimObj *pobj) {
	return m_pObjectQueue->IsAnimating(pobj);
}

RESULT InteractionEngine::RemoveAnimationObject(DimObj *pObj) {
	return m_pObjectQueue->RemoveAnimationObject(pObj);
}

RESULT InteractionEngine::RemoveAllObjects() {
	return m_pObjectQueue->RemoveAllObjects();
}

/*
point InteractionEngine::GetInteractionRayOrigin() {
	return m_pInteractionRay->GetOrigin();
}
*/

// TODO: Tidy up / test this code
/*
RESULT InteractionEngine::UpdateInteractionRay() {
	RESULT r = R_PASS;

	ray rCast;
	hand *pRightHand = m_pSandbox->GetHand(hand::HAND_TYPE::HAND_RIGHT);

	if (pRightHand != nullptr) {
		point ptHand = pRightHand->GetPosition();

		//GetLookVector
		quaternion qHand = pRightHand->GetHandState().qOrientation;
		qHand.Normalize();

		vector vHandLook = RotationMatrix(qHand) * vector(0.0f, 0.0f, -1.0f);
		vHandLook.Normalize();

		//vector vCast = vector(-vHandLook.x(), -vHandLook.y(), vHandLook.z());

		rCast = ray(ptHand, vHandLook);
		CR(UpdateInteractionPrimitive(rCast));
	}
	
	// This is handled by mouse move events
	else {
		CR(m_pSandbox->GetMouseRay(rCast, 0.0f));
		CR(UpdateInteractionPrimitive(rCast));
	}

// Success:
	return r;

Error:
	UpdateInteractionPrimitive(ray(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, 0.0f)));
	return r;
}
*/

RESULT InteractionEngine::CaptureObject(VirtualObj *pObject, VirtualObj *pInteractionObject, point ptContact, vector vDirection, vector vSurface, float threshold) {
	RESULT r = R_PASS;

	plane planeContext(pObject->GetPosition(true), vDirection);

	CapturedObj *cObj = new CapturedObj(
		pObject,
		threshold,
		planeContext,
		point(pInteractionObject->GetPosition(true) - pObject->GetPosition(true)),
		pObject->GetPosition(), 
		vSurface);
	CN(cObj);

	m_objectsToCapture.emplace_back(std::pair<VirtualObj*,CapturedObj*>(pInteractionObject, cObj));

Error:
	return r;
}

RESULT InteractionEngine::ResetObject(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) {
	RESULT r = R_PASS;

	CBR(HasCapturedObjects(pInteractionObject), R_OBJECT_NOT_FOUND);
	for (auto& cObj : m_capturedObjects[pInteractionObject]) {
		if (cObj->GetObject() == pCapturedObj) {
			cObj->GetObject()->SetPosition(cObj->GetOrigin());
		}
	}

Error:
	return r;
}

RESULT InteractionEngine::ResetObjects(VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	CBR(HasCapturedObjects(pInteractionObject), R_OBJECT_NOT_FOUND);
	for (auto& cObj : m_capturedObjects[pInteractionObject]) {
		cObj->GetObject()->SetPosition(cObj->GetOrigin());
	}

Error:
	return r;
}

RESULT InteractionEngine::ReleaseObject(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) {
	RESULT r = R_PASS;

	CBR(HasCapturedObjects(pInteractionObject), R_OBJECT_NOT_FOUND);
	for (auto& cObj : m_capturedObjects[pInteractionObject]) {
		if (cObj->GetObject() == pCapturedObj) {
			m_capturedObjectsToRelease.emplace_back(std::pair<VirtualObj*,CapturedObj*>(pInteractionObject, cObj));
		}
	}

Error:
	return r;
}

RESULT InteractionEngine::ReleaseObjects(VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	CBR(HasCapturedObjects(pInteractionObject), R_OBJECT_NOT_FOUND);
	for (auto cObj : m_capturedObjects[pInteractionObject]) {
		m_capturedObjectsToRelease.emplace_back(std::pair<VirtualObj*,CapturedObj*>(pInteractionObject, cObj));
	}

Error:
	return r;
}

bool InteractionEngine::HasCapturedObjects(VirtualObj *pInteractionObject) {
	return m_capturedObjects.count(pInteractionObject) > 0;
}

bool InteractionEngine::IsObjectCaptured(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) {
	if (HasCapturedObjects(pInteractionObject)) {
		for (auto& cObj : m_capturedObjects[pInteractionObject]) {
			if (cObj->GetObject() == pCapturedObj) {
				return true; 
			}
		}
	}
	return false;
}

std::vector<CapturedObj*> InteractionEngine::GetCapturedObjects(VirtualObj *pInteractionObject) {
	if (HasCapturedObjects(pInteractionObject)) {
		return m_capturedObjects[pInteractionObject];
	}
	return {};
}

RESULT InteractionEngine::AddInteractionObject(VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	CBR((FindInteractionObject(pInteractionObject) == nullptr), R_SKIPPED);
	m_interactionObjects.push_back(pInteractionObject);

	// Add new list 
	//m_activeObjects[pInteractionObject] = std::list<std::shared_ptr<ActiveObject>>();
	CR(m_activeObjectQueues[ActiveObject::type::INTERSECT].AddInteractionObjectList(pInteractionObject));
	CR(m_activeObjectQueues[ActiveObject::type::COLLIDE].AddInteractionObjectList(pInteractionObject));

Error:
	return r;
}

RESULT InteractionEngine::RemoveInteractionObject(VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	//CNR((FindInteractionObject(pInteractionObject) == nullptr), R_NOT_FOUND);
	
	auto pInteractionObjectIterator = std::find(m_interactionObjects.begin(), m_interactionObjects.end(), pInteractionObject);
	CBR(pInteractionObjectIterator != m_interactionObjects.end(), R_NOT_FOUND);

	m_interactionObjects.erase(pInteractionObjectIterator);

	// Remove the list
	//m_activeObjects.erase(pInteractionObject);
	CR(m_activeObjectQueues[ActiveObject::type::INTERSECT].RemoveInteractionObjectList(pInteractionObject));
	CR(m_activeObjectQueues[ActiveObject::type::COLLIDE].RemoveInteractionObjectList(pInteractionObject));

Error:
	return r;
}

RESULT InteractionEngine::ClearInteractionObjects() {
	RESULT r = R_PASS;

	m_interactionObjects.clear();

	CR(m_activeObjectQueues[ActiveObject::type::INTERSECT].ClearActiveObjects());
	CR(m_activeObjectQueues[ActiveObject::type::COLLIDE].ClearActiveObjects());

Error:
	return r;
}

VirtualObj* InteractionEngine::FindInteractionObject(VirtualObj *pInteractionObject) {
	auto pInteractionObjectIterator = std::find(m_interactionObjects.begin(), m_interactionObjects.end(), pInteractionObject);\

	if (pInteractionObjectIterator != m_interactionObjects.end()) {
		return *pInteractionObjectIterator;
	}

	return nullptr;
}

// TODO: not quite true to the name of the function, this only sets intersection but won't un intersect
InteractionEventType InteractionEngine::UpdateActiveObject(ActiveObject::type activeObjectType, VirtualObj *pInteractionObject, CollisionManifold manifold, VirtualObj *pEventObject) {
	RESULT r = R_PASS;

	InteractionEventType eventType = InteractionEventType::INTERACTION_EVENT_INVALID;
	VirtualObj *pObject = nullptr;
	std::shared_ptr<ActiveObject> pActiveObject = nullptr;

	enum EventState {
		BEGAN,
		MOVED,
		NONE
	} eventState = EventState::NONE;

	int numContacts = manifold.NumContacts();

	// Ensure more than one contact found
	CB((numContacts > 0));

	// Manifold should return only one object, one will be nullptr
	pObject = manifold.GetObjectA();

	if (pObject == nullptr) {
		pObject = manifold.GetObjectB();
	}
	
	// Ensure object present
	CN(pObject);

	// Check for active object
	pActiveObject = m_activeObjectQueues[activeObjectType].FindActiveObject(pObject, pInteractionObject);

	if (pActiveObject == nullptr) {
		bool fEventObjectFound = false;

		// Lets cross check against duplicate event objects
		// This will change eventState to moved vs began
		// This will prevent sending repro start / move events
		if (m_activeObjectQueues[activeObjectType].HasActiveEventObject(pInteractionObject, pEventObject) == true) {
			fEventObjectFound = true;
		}

		pActiveObject = m_activeObjectQueues[activeObjectType].AddActiveObject(pObject, pInteractionObject);
		CN(pActiveObject);

		// Set event object (for composites, events might be registered to composite but event is triggered on a different object
		// per the queue)
		if (pEventObject != pObject) {
			pActiveObject->SetEventObject(pEventObject);
		}

		pActiveObject->SetContactPoint(manifold.GetContactPoint(0));

		// If event object already found, then report a moved event
		if(fEventObjectFound)
			eventState = EventState::MOVED;
		else
			eventState = EventState::BEGAN;
	}
	else {
		vector vDiff = manifold.GetContactPoint(0).GetPoint() - pActiveObject->GetIntersectionPoint();

		if (vDiff.magnitude() > m_diffThreshold) {
			pActiveObject->SetContactPoint(manifold.GetContactPoint(0));

			// Notify element intersect continue
			//eventType = InteractionEventType::ELEMENT_INTERSECT_MOVED;
			eventState = EventState::MOVED;
		}
	}

	if (activeObjectType == ActiveObject::type::INTERSECT) {
		pActiveObject->AddState(ActiveObject::state::RAY_INTERSECTED);

		if (eventState == EventState::BEGAN) {
			eventType = InteractionEventType::ELEMENT_INTERSECT_BEGAN;
		}
		else if (eventState == EventState::MOVED) {
			eventType = InteractionEventType::ELEMENT_INTERSECT_MOVED;
		}
	}
	else if (activeObjectType == ActiveObject::type::COLLIDE) {
		pActiveObject->AddState(ActiveObject::state::OBJ_INTERSECTED);

		if (eventState == EventState::BEGAN) {
			eventType = InteractionEventType::ELEMENT_COLLIDE_BEGAN;
		}
		else if (eventState == EventState::MOVED) {
			eventType = InteractionEventType::ELEMENT_COLLIDE_MOVED;
		}
	}

	return eventType;

Error:
	return InteractionEventType::INTERACTION_EVENT_INVALID;
}

RESULT InteractionEngine::UpdateObjectStore(ActiveObject::type activeObjectType, ObjectStore *pObjectStore, VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	for (auto &pEventObject : pObjectStore->GetObjects()) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pEventObject);
		InteractionObjectEvent interactionEvent;
		CollisionManifold manifold;
		bool fIntersect = false;

		if (IsObjectCaptured(pInteractionObject, pEventObject)) {
			continue;
		}

		// Acquire manifold accordingly
		if (activeObjectType == ActiveObject::type::INTERSECT) {
			interactionEvent.m_interactionRay = pInteractionObject->GetRay(true);
			
			if (pDimObj->Intersect(interactionEvent.m_interactionRay)) {
				manifold = pDimObj->Collide(interactionEvent.m_interactionRay);
				fIntersect = true;
			}
		}
		else if (activeObjectType == ActiveObject::type::COLLIDE) {
			if (pDimObj->Intersect(pInteractionObject)) {
				manifold = pDimObj->Collide(pInteractionObject);
				fIntersect = true;
			}
		}
			
		if(fIntersect) {
			InteractionEventType eventType;
			eventType = UpdateActiveObject(activeObjectType, pInteractionObject, manifold, pEventObject);
			
			if (eventType != InteractionEventType::INTERACTION_EVENT_INVALID) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					interactionEvent.AddPoint(manifold.GetContactPoint(i));
				}

				VirtualObj *pObject = manifold.GetObjectA();
				if (pObject == nullptr) {
					pObject = manifold.GetObjectB();
				}

				interactionEvent.m_eventType = eventType;
				interactionEvent.m_pObject = pObject;
				interactionEvent.m_pEventObject = pEventObject;
				interactionEvent.m_pInteractionObject = pInteractionObject;
				interactionEvent.m_activeState = m_activeObjectQueues[activeObjectType].GetActiveObjectState(pObject, pInteractionObject);

				// Note this will go to all composite subs
				CR(NotifySubscribers(pEventObject, interactionEvent.m_eventType, &interactionEvent));
			}
		}

		/*
		// Object Collision
		if (pDimObj->Intersect(pInteractionObject)) {
			CollisionManifold objManifold = pDimObj->Collide(pInteractionObject);
			InteractionEventType eventTypeObj;

			eventTypeObj = UpdateActiveObject(ActiveObject::type::COLLIDE, pInteractionObject, objManifold);

			if (eventTypeObj != InteractionEventType::INTERACTION_EVENT_INVALID) {
				for (int i = 0; i < objManifold.NumContacts(); i++) {
					interactionEvent.AddPoint(objManifold.GetContactPoint(i));
				}

				interactionEvent.m_eventType = eventTypeObj;
			}

			if (interactionEvent.m_eventType != InteractionEventType::INTERACTION_EVENT_INVALID) {

				interactionEvent.m_pObject = pObj;
				interactionEvent.m_pInteractionObject = pInteractionObject;
				interactionEvent.m_activeState = m_activeObjectQueues[ActiveObject::type::COLLIDE].GetActiveObjectState(pObj, pInteractionObject);

				CR(NotifySubscribers(pObject, interactionEvent.m_eventType, &interactionEvent));
			}
		}
		*/
	}
	

Error:
	return r;
}

RESULT InteractionEngine::UpdateCapturedObjects(VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	if (HasCapturedObjects(pInteractionObject), R_OBJECT_NOT_FOUND) {

		for (auto &pCaptureObj : m_capturedObjects[pInteractionObject]) {

			point ptObj = pInteractionObject->GetPosition(true) - pCaptureObj->GetOffset();
			point ptCap = pCaptureObj->GetObject()->GetPosition(true);

			// project difference of this frame along the movement vector
			vector vDiff = ptObj - ptCap;
			vector vDirection = pCaptureObj->GetPlaneContext().GetNormal();
			float vDot = vDiff.dot(vDirection);

			// project the difference between the current position and the original position
			// along the movement vector.  if this is negative, release the object
			vector vOriginDiff = ptObj - pCaptureObj->GetPlaneContext().GetPosition();
			float vOriginDot = vOriginDiff.dot(vDirection);

			if (vOriginDot < 0.0f) {
				// clamp to original position
				pCaptureObj->GetObject()->SetPosition(pCaptureObj->GetOrigin());
				//capturedObjectsToRemove.push_back(pInteractionObject);
				ReleaseObject(pInteractionObject, pCaptureObj->GetObject());
			}
			else {
				// move captured object along the local surface vector
				vector vProj = pCaptureObj->GetRelativeSurfaceNormal() * (vDot);
				//vector vProj = vDirection * (vDot);

				pCaptureObj->GetObject()->SetPosition(pCaptureObj->GetObject()->GetPosition() + vProj);

				plane planeCapture = pCaptureObj->GetPlaneContext();
				vector vDistance = pCaptureObj->GetObject()->GetPosition(true) - planeCapture.GetPosition();

				if (vDistance.magnitude() > (pCaptureObj->GetThreshold())) {
					// clamp to maximum position
					pCaptureObj->GetObject()->SetPosition(pCaptureObj->GetOrigin() + (pCaptureObj->GetRelativeSurfaceNormal() * pCaptureObj->GetThreshold()));

					InteractionObjectEvent interactionEvent(ELEMENT_COLLIDE_TRIGGER, pCaptureObj->GetObject(), pInteractionObject);
					CR(NotifySubscribers(pCaptureObj->GetObject(), ELEMENT_COLLIDE_TRIGGER, &interactionEvent));
				}
			}
		}
	}

Error:
	return r;
}

RESULT InteractionEngine::UpdateCapturedObjectStore() {

	for (auto pObjects : m_objectsToCapture) {
		m_capturedObjects[pObjects.first].emplace_back(pObjects.second);
	}
	m_objectsToCapture.clear();

	for (auto& objPair : m_capturedObjectsToRelease) {
		std::swap(objPair.second, m_capturedObjects[objPair.first].back());
		m_capturedObjects[objPair.first].pop_back();
	}
	m_capturedObjectsToRelease.clear();

	return R_PASS;
}

RESULT InteractionEngine::RemoveActiveObjects(std::map<VirtualObj*, std::vector<std::shared_ptr<ActiveObject>>> activeObjectsToRemove, std::pair<ActiveObject::type, ActiveObjectQueue*> activeObjectQueuePair, VirtualObj *pInteractionObject) {
	RESULT r = R_PASS;

	for (auto &pActiveObject : activeObjectsToRemove[pInteractionObject]) {
		// Notify no longer intersected
		CR(activeObjectQueuePair.second->RemoveActiveObject(pActiveObject, pInteractionObject));

		InteractionObjectEvent interactionEvent;

		// Notify element intersect continue
		// This uses the last available point
		// TODO: Add projection , find exit point, do we need that?

		if (activeObjectQueuePair.first == ActiveObject::type::INTERSECT) {
			interactionEvent.m_eventType = InteractionEventType::ELEMENT_INTERSECT_ENDED;
			interactionEvent.m_interactionRay = pInteractionObject->GetRay();
			interactionEvent.m_pInteractionObject = pInteractionObject;
		}
		else if (activeObjectQueuePair.first == ActiveObject::type::COLLIDE) {
			interactionEvent.m_eventType = InteractionEventType::ELEMENT_COLLIDE_ENDED;
			interactionEvent.m_pInteractionObject = pInteractionObject;
		}

		interactionEvent.m_pObject = pActiveObject->GetObject();
		interactionEvent.m_pEventObject = pActiveObject->GetEventObject();
		interactionEvent.AddPoint(pActiveObject->GetIntersectionPoint(), pActiveObject->GetIntersectionNormal());
		interactionEvent.m_activeState = pActiveObject->GetState();

		// Lets cross check against active event objects to ensure against double releases 
		// This will prevent the end event
		if(activeObjectQueuePair.second->HasActiveEventObject(pInteractionObject, pActiveObject->GetEventObject()) == false) {
			CR(NotifySubscribers(pActiveObject->GetEventObject(), interactionEvent.m_eventType, &interactionEvent));
		}
	}

Error:
	return r;
}

RESULT InteractionEngine::RemoveObject(VirtualObj *pEventObject, ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	for (auto &activeObjectQueuePair : m_activeObjectQueues) {
		std::map<VirtualObj*, std::vector<std::shared_ptr<ActiveObject>>> activeObjectsToRemove;
		for (auto &pInteractionObject : m_interactionObjects) {
			for (auto &pActiveObject : activeObjectQueuePair.second[pInteractionObject]) {
				// Add to remove list if not intersected in current frame
				if (pActiveObject->GetEventObject() == pEventObject) {
					activeObjectsToRemove[pInteractionObject].push_back(pActiveObject);
				}
			}
			auto removePair = std::pair<ActiveObject::type, ActiveObjectQueue*>(activeObjectQueuePair.first, &activeObjectQueuePair.second);
			CR(RemoveActiveObjects(activeObjectsToRemove, removePair, pInteractionObject));
		}
	}

	for (auto& capturePair : m_capturedObjects) {
		for (auto& capturedObj : capturePair.second) {
			if (capturedObj->GetObject() == pEventObject) {
				m_capturedObjectsToRelease.emplace_back(std::pair<VirtualObj*, CapturedObj*>(capturePair.first, capturedObj));
			}
		}
	}

	UpdateCapturedObjectStore();

Error:
	return r;
}

// TODO: This is temporary
RESULT InteractionEngine::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	// turning off time step for now

	//auto timeNow = std::chrono::high_resolution_clock::now();
	//auto timeDelta = std::chrono::duration<double>(timeNow - m_lastUpdateTime).count();
	//m_lastUpdateTime = timeNow;
	//
	//m_elapsedTime += timeDelta;
	//
	//if (m_elapsedTime >= m_sTimeStep) {
	//
	//	//m_elapsedTime = m_elapsedTime - m_sTimeStep;
	//	m_elapsedTime = 0;

		//TODO: this should be called activeObjectQueuePair, because only the second value is the activeObjectQueue
		for (auto &activeObjectQueue : m_activeObjectQueues) {

			std::vector<VirtualObj*> capturedObjectsToRemove;
			std::map<VirtualObj*, std::vector<std::shared_ptr<ActiveObject>>> activeObjectsToRemove;

			//TODO: extend capture object implementation to handle multiple captured objects

			// TODO: First pass (no state tracking yet)
			// Set all objects to non-intersected - below will set it to intersected, then remaining
			// non-intersected objects are clearly no longer in the active set
			CR(activeObjectQueue.second.SetAllActiveObjectStates(ActiveObject::state::NOT_INTERSECTED));

			CR(UpdateCapturedObjectStore());

			for (auto &pInteractionObject : m_interactionObjects) {
				//CR(UpdateObjectStoreRay(pObjectStore, pInteractionObject));
				CR(UpdateObjectStore(activeObjectQueue.first, pObjectStore, pInteractionObject));

				UpdateCapturedObjects(pInteractionObject);

				for (auto &pActiveObject : activeObjectQueue.second[pInteractionObject]) {
					// Add to remove list if not intersected in current frame
					if (pActiveObject->GetState() == ActiveObject::state::NOT_INTERSECTED) {
						activeObjectsToRemove[pInteractionObject].push_back(pActiveObject);
					}
				}
				auto removePair = std::pair<ActiveObject::type, ActiveObjectQueue*>(activeObjectQueue.first, &activeObjectQueue.second);
				CR(RemoveActiveObjects(activeObjectsToRemove, removePair, pInteractionObject));
			}
			CR(UpdateCapturedObjectStore());
		}

	//}

Error:
	return r;
}

RESULT InteractionEngine::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->type) {
		case SENSE_CONTROLLER_TRIGGER_UP: {
			for(auto &pActiveObject : m_activeObjectQueues[ActiveObject::type::INTERSECT].FindActiveObjectsWithState(ActiveObject::state::RAY_INTERSECTED | ActiveObject::state::OBJ_INTERSECTED)) {
				VirtualObj *pObject = pActiveObject->GetObject();

				InteractionEventType type = INTERACTION_EVENT_SELECT_UP;
				InteractionObjectEvent interactionEvent(type, pObject);

				CR(NotifySubscribers(pObject, type, &interactionEvent));
			}
		} break;

		case SENSE_CONTROLLER_TRIGGER_DOWN: {
			for (auto &pActiveObject : m_activeObjectQueues[ActiveObject::type::INTERSECT].FindActiveObjectsWithState(ActiveObject::state::RAY_INTERSECTED | ActiveObject::state::OBJ_INTERSECTED)) {
				VirtualObj *pObject = pActiveObject->GetObject();

				InteractionEventType type = INTERACTION_EVENT_SELECT_DOWN;
				InteractionObjectEvent interactionEvent(type, pObject);

				CR(NotifySubscribers(pObject, type, &interactionEvent));
			}
		} break;

		case SENSE_CONTROLLER_MENU_UP: {
			InteractionEventType type = INTERACTION_EVENT_MENU;
			InteractionObjectEvent interactionEvent(type);

			CR(NotifySubscribers(type, &interactionEvent));
		} break;

		case SENSE_CONTROLLER_PAD_MOVE: {
			// Keeps decimal in accumulator, moves value into touchY
			m_interactionPadAccumulator += pEvent->state.ptTouchpad.y();
			double touchY = m_interactionPadAccumulator;
			m_interactionPadAccumulator = std::modf(m_interactionPadAccumulator, &touchY);

			for (auto &pActiveObject : m_activeObjectQueues[ActiveObject::type::INTERSECT].FindActiveObjectsWithState(ActiveObject::state::RAY_INTERSECTED | ActiveObject::state::OBJ_INTERSECTED)) {
				VirtualObj *pObject = pActiveObject->GetObject();

				InteractionEventType type = INTERACTION_EVENT_WHEEL;
				InteractionObjectEvent interactionEvent(type, pObject);
				interactionEvent.SetValue((int)(touchY));

				CR(NotifySubscribers(type, &interactionEvent));
			}
		} break;
	}

Error:
	return r;
}

RESULT InteractionEngine::Notify(SenseKeyboardEvent *pEvent) {
	RESULT r = R_PASS;

	// Pass through keyboard input

	//TODO: hack to get around active object issues
	//
	///*
//	for (auto &pObject : m_activeObjects) {
		InteractionEventType type;

		if (pEvent->KeyState == 0)
			type = INTERACTION_EVENT_KEY_UP;
		else
			type = INTERACTION_EVENT_KEY_DOWN;

		InteractionObjectEvent interactionEvent(type);
		interactionEvent.SetValue((int)(pEvent->KeyCode));

		CR(NotifySubscribers(type, &interactionEvent));
//	}
	//*/
	CR(r);

Error:
	return r;
}

RESULT InteractionEngine::Notify(SenseTypingEvent *pEvent) {
	RESULT r = R_PASS;

	// Pass through keyboard input
	/*
	for (auto &pObject : m_activeObjects) {
		InteractionEventType type; 

		if (pEvent->KeyState == 0)
			type = INTERACTION_EVENT_KEY_UP;
		else
			type = INTERACTION_EVENT_KEY_DOWN;

		InteractionObjectEvent interactionEvent(type, m_pInteractionRay, pObject->GetObject());
		interactionEvent.SetValue((int)(pEvent->KeyCode));

		CR(NotifySubscribers(type, &interactionEvent));
	}
	//*/
	CR(r);

Error:
	return r;
}


RESULT InteractionEngine::Notify(SenseMouseEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->EventType) {
		case SENSE_MOUSE_MOVE: {
			ray rCast;
			m_pSandbox->GetMouseRay(rCast, 0.0f);
			
			//CR(UpdateInteractionPrimitive(rCast));

		} break;

		case SENSE_MOUSE_LEFT_BUTTON_UP: {
			//for (auto &activeObjects : m_activeObjectQueues[ActiveObject::type::INTERSECT]) {
			for (auto &pInteractionObject : m_interactionObjects) {
				//VirtualObj *pObject = activeObjects.first;

				//ray rCast;
				//m_pSandbox->GetMouseRay(rCast, 0.0f);
				//CR(UpdateInteractionPrimitive(rCast));

				InteractionEventType type = INTERACTION_EVENT_SELECT_UP;
				//InteractionObjectEvent interactionEvent(type, nullptr, pInteractionObject);
				InteractionObjectEvent interactionEvent(type, nullptr, pInteractionObject);
				CR(NotifySubscribers(type, &interactionEvent));
			}
		} break;

		case SENSE_MOUSE_LEFT_BUTTON_DOWN: {
			//for (auto &activeObjects : m_activeObjects) {
			for (auto &pInteractionObject : m_interactionObjects) {
				//VirtualObj *pObject = activeObjects.first;

				//ray rCast;
				//m_pSandbox->GetMouseRay(rCast, 0.0f);
				//CR(UpdateInteractionPrimitive(rCast));

				InteractionEventType type = INTERACTION_EVENT_SELECT_DOWN;
				//InteractionObjectEvent interactionEvent(type, pObject);
				InteractionObjectEvent interactionEvent(type, nullptr, pInteractionObject);
				CR(NotifySubscribers(type, &interactionEvent));
			}
		} break;

		case SENSE_MOUSE_WHEEL: {
			//for (auto &activeObjects : m_activeObjects) {
			for (auto &pInteractionObject : m_interactionObjects) {
				//VirtualObj *pObject = activeObjects.first;

				//ray rCast;
				//m_pSandbox->GetMouseRay(rCast, 0.0f);
				//CR(UpdateInteractionPrimitive(rCast));

				InteractionEventType type = INTERACTION_EVENT_WHEEL;
				//InteractionObjectEvent interactionEvent(type, pObject);
				InteractionObjectEvent interactionEvent(type, nullptr, pInteractionObject);
				interactionEvent.SetValue(pEvent->state * 10);

				CR(NotifySubscribers(type, &interactionEvent));
			}
		} break;
	}

Error:
	return r;
}

InteractionEngineProxy *InteractionEngine::GetInteractionEngineProxy() {
	return (InteractionEngineProxy*)(this);
}
