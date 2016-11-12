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

Error:
	return r;
}

RESULT CollisionDetector::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	for (auto &objCollisionGroup : pObjectStore->GetSceneGraphStore()->GetObjectCollisionGroups()) {
		// Handle group collisions
		CollisionGroupEvent event(OBJECT_GROUP_COLLISION, objCollisionGroup);

		if (Publisher<CollisionGroupEventType, CollisionGroupEvent > ::IsEventRegistered(OBJECT_GROUP_COLLISION)) {
			Publisher<CollisionGroupEventType, CollisionGroupEvent>::NotifySubscribers(OBJECT_GROUP_COLLISION, &event);
		}
	}
	
	//Error:
	return r;
}

RESULT CollisionDetector::RegisterSubscriber(CollisionGroupEventType collisionGroupEvent, Subscriber<CollisionGroupEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	r = Publisher<CollisionGroupEventType, CollisionGroupEvent>::RegisterSubscriber(collisionGroupEvent, pCollisionDetectorSubscriber);
	CR(r);

Error:
	return r;
}

RESULT CollisionDetector::RegisterSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	r = Publisher<VirtualObj*, CollisionObjectEvent>::RegisterSubscriber(pVirtualObject, pCollisionDetectorSubscriber);
	CR(r);

Error:
	return r;
}