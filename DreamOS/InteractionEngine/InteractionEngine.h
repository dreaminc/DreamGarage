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

#include "ActiveObject.h"

class ObjectStore;

typedef enum InteractionEventType {
	ELEMENT_INTERSECT_BEGAN,
	ELEMENT_INTERSECT_MOVED,
	ELEMENT_INTERSECT_ENDED,
	ELEMENT_INTERSECT_CANCELLED,
	INTERACTION_EVENT_INVALID
} INTERACTION_EVENT_TYPE;

// Object Interaction Event
typedef struct InteractionObjectEvent {

	InteractionEventType m_eventType;
	VirtualObj *m_pInteractionObject;
	VirtualObj *m_pObject;
	point m_ptContact;
	vector m_vNormal;

	// TODO: Add time of collision

	InteractionObjectEvent(InteractionEventType eventType, VirtualObj *pInteractionObject, VirtualObj *pObject, point ptContact, vector vNormal) :
		m_eventType(eventType),
		m_pInteractionObject(pInteractionObject),
		m_pObject(pObject),
		m_ptContact(ptContact),
		m_vNormal(vNormal)
	{
		// empty
	}

} INTERACTION_OBJECT_EVENT;

/*
class InteractionObject {

};
*/

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

	RESULT RegisterSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);

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