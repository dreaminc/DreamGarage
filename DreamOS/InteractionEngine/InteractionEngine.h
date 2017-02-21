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

//#include "CollisionDetector.h"
//#include "CollisionResolver.h"
//#include "PhysicsIntegrator.h"

#include "Primitives/VirtualObj.h"
#include "Primitives/Publisher.h"
#include <vector>

#include "InteractionObjectEvent.h"
#include "ActiveObject.h"

class ObjectStore;

/*
class InteractionObject {

};
*/

class InteractionEngine : public valid, public Publisher<InteractionEventType, InteractionObjectEvent> {
public:
	static std::unique_ptr<InteractionEngine> MakeEngine();

private:
	InteractionEngine();

	RESULT Initialize();

public:
	RESULT Update();
	RESULT UpdateObjectStore(ObjectStore *pObjectStore);
	RESULT SetInteractionGraph(ObjectStore *pObjectStore);

	RESULT UpdateInteractionPrimitive(const ray &r);

	//RESULT RegisterSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	// TODO: Register element events etc

public:
	// Active Objects
	RESULT ClearActiveObjects();
	RESULT AddActiveObject(VirtualObj *pVirtualObject);
	RESULT RemoveActiveObject(VirtualObj *pVirtualObject);
	std::shared_ptr<ActiveObject> FindActiveObject(VirtualObj *pVirtualObject);
	ActiveObject::state GetActiveObjectState(VirtualObj *pVirtualObject);

private:
	std::shared_ptr<ray> m_pInteractionRay = nullptr;
	std::list<std::shared_ptr<ActiveObject>> m_activeObjects;

private:
	UID m_uid;
};

#endif	// ! PHYSICS_ENGINE_H_