#include "CollisionDetector.h"

#include "Scene/ObjectStore.h"

#include <ctime>
#include <chrono>

CollisionDetector::CollisionDetector() {
	// empty
}

RESULT CollisionDetector::Initialize() {
	RESULT r = R_PASS;

	r = Publisher<CollisionGroupEventType, CollisionGroupEvent>::RegisterEvent(OBJECT_GROUP_COLLISION);
	CR(r);

	/*
	r = Publisher<CollisionGroupEventType, CollisionGroupEvent>::RegisterEvent(ALL_COLLISIONS);
	CR(r);
	*/

Error:
	return r;
}

RESULT CollisionDetector::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	// Handle group collisions if registered 
	if (Publisher<CollisionGroupEventType, CollisionGroupEvent>::IsEventRegistered(OBJECT_GROUP_COLLISION)) {
		for (auto &objCollisionGroup : pObjectStore->GetSceneGraphStore()->GetObjectCollisionGroups()) {
			CollisionGroupEvent collisionGroupEvent(OBJECT_GROUP_COLLISION, objCollisionGroup);
			Publisher<CollisionGroupEventType, CollisionGroupEvent>::NotifySubscribers(OBJECT_GROUP_COLLISION, &collisionGroupEvent);
		}
	}

	// Handle object collisions
	for (auto &objectEvent : Publisher<VirtualObj*, CollisionObjectEvent>::GetEvents()) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(objectEvent.first);
		auto objCollisionGroup = pObjectStore->GetSceneGraphStore()->GetObjects(reinterpret_cast<DimObj*>(pDimObj));
		
		if (objCollisionGroup.size() > 0) {
			CollisionObjectEvent collisionObjectEvent(objectEvent.first, objCollisionGroup);
			Publisher<VirtualObj*, CollisionObjectEvent>::NotifySubscribers(objectEvent.first, &collisionObjectEvent);
		}
	}

	// All events should have fired 
	// TODO: This isn't too great performance wise - should add "dirty bit" somewhere
	CR(pObjectStore->CommitObjects());
	
Error:
	return r;
}

RESULT CollisionDetector::RegisterSubscriber(CollisionGroupEventType collisionGroupEvent, Subscriber<CollisionGroupEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	r = Publisher<CollisionGroupEventType, CollisionGroupEvent>::RegisterSubscriber(collisionGroupEvent, pCollisionDetectorSubscriber);
	CR(r);

Error:
	return r;
}

RESULT CollisionDetector::RegisterObjectAndSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	if (Publisher<VirtualObj*, CollisionObjectEvent>::IsEventRegistered(pVirtualObject) == false) {
		r = Publisher<VirtualObj*, CollisionObjectEvent>::RegisterEvent(pVirtualObject);
		CR(r);
	}

	r = Publisher<VirtualObj*, CollisionObjectEvent>::RegisterSubscriber(pVirtualObject, pCollisionDetectorSubscriber);
	CR(r);

Error:
	return r;
}