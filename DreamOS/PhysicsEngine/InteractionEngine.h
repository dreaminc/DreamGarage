#ifndef INTERACTION_ENGINE_H_
#define INTERACTION_ENGINE_H_

#include "RESULT/EHM.h"

// Dream Interaction Engine 
// This class combines the various components required for a 
// functional interaction engine, such as collision detection and resolution as well
// as integration of animation and the relevant queue

#include <memory>

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#include "CollisionDetector.h"
#include "CollisionResolver.h"
//#include "PhysicsIntegrator.h"

class ObjectStore;

class InteractionEngine : public valid {
public:
	static std::unique_ptr<InteractionEngine> MakeEngine();

private:
	InteractionEngine();

	RESULT Initialize();

public:
	RESULT Update();
	RESULT UpdateObjectStore(ObjectStore *pObjectStore);
	RESULT SetInteractionGraph(ObjectStore *pObjectStore);

	RESULT RegisterSubscriber(CollisionGroupEventType collisionGroupEvent, Subscriber<CollisionGroupEvent>* pCollisionDetectorSubscriber);
	RESULT RegisterObjectCollisionSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber);

private:
	std::unique_ptr<CollisionDetector> m_pCollisionDetector;
	std::unique_ptr<CollisionResolver> m_pCollisionResolver;
	//std::unique_ptr<PhysicsIntegrator> m_pPhysicsIntegrator;

private:
	UID m_uid;
};

#endif	// ! PHYSICS_ENGINE_H_