#ifndef COLLISION_DETECTOR_H_
#define COLLISION_DETECTOR_H_

#include "RESULT/EHM.h"

// Dream Collision Detector 
// This class is responsible for detecting and reporting collisions
// to dependent classes or subscribers

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#include "Primitives/VirtualObj.h"
#include "Primitives/Publisher.h"
#include <vector>

class ObjectStore;

// Collision Group Event
typedef enum CollisionGroupEventType {
	ALL_COLLISIONS,
	OBJECT_GROUP_COLLISION,
	INVALID
} COLLISION_GROUP_EVENT_TYPE;

typedef struct CollisionGroupEvent {
	CollisionGroupEventType EventType;
	
	std::vector<VirtualObj*> m_collisionGroup;

	// TODO: Add time of collision

	CollisionGroupEvent(CollisionGroupEventType eventType, std::vector<VirtualObj*> collisionGroup) :
		EventType(eventType), 
		m_collisionGroup(collisionGroup)
	{
		// empty
	}

} COLLISION_GROUP_EVENT;

// Object Collision Event
typedef struct CollisionObjectEvent {
	
	VirtualObj *m_pCollisionObject;
	std::vector<VirtualObj*> m_collisionGroup;

	// TODO: Add time of collision

	CollisionObjectEvent(VirtualObj *pCollisionObject, std::vector<VirtualObj*> collisionGroup) :
		m_pCollisionObject(pCollisionObject),
		m_collisionGroup(collisionGroup)
	{
		// empty
	}

} COLLISION_OBJECT_EVENT;

class CollisionDetector : public Publisher<CollisionGroupEventType, CollisionGroupEvent>, public Publisher<VirtualObj*, CollisionObjectEvent>, public valid {
	friend class PhysicsEngine;

private:
	CollisionDetector();

protected:
	RESULT Initialize();
	RESULT UpdateObjectStore(ObjectStore *pObjectStore);

public:
	RESULT RegisterSubscriber(CollisionGroupEventType collisionGroupEvent, Subscriber<CollisionGroupEvent>* pCollisionDetectorSubscriber);
	RESULT RegisterObjectAndSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber);

private:
	UID m_uid;
};

#endif	// ! COLLISION_DETECTOR_H_