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
#include "Primitives/Subscriber.h"
#include "Sense/SenseController.h"

#include <vector>

#include "InteractionObjectEvent.h"
#include "ActiveObject.h"
#include <chrono>

#define DEFAULT_INTERACTION_DIFF_THRESHOLD 0.025f

class ObjectStore;
class AnimationQueue;
//class AnimationState;
#include "AnimationItem.h"
/*
class InteractionObject {

};
*/

class InteractionEngine : public valid, public Publisher<InteractionEventType, InteractionObjectEvent>, public Subscriber<SenseControllerEvent> {
public:
	static std::unique_ptr<InteractionEngine> MakeEngine();

private:
	InteractionEngine();

	RESULT Initialize();

public:
	RESULT Update();
	RESULT UpdateObjectStore(ObjectStore *pObjectStore);
	RESULT UpdateAnimationQueue(); 
	RESULT SetInteractionGraph(ObjectStore *pObjectStore);

	RESULT UpdateInteractionPrimitive(const ray &r);

	RESULT SetInteractionDiffThreshold(double thresh);

	//RESULT RegisterSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	// TODO: Register element events etc

	// Active Objects
public:
	RESULT ClearActiveObjects();
	std::shared_ptr<ActiveObject> AddActiveObject(VirtualObj *pVirtualObject);
	RESULT SetAllActiveObjectStates(ActiveObject::state newState);
	RESULT RemoveActiveObject(VirtualObj *pVirtualObject);
	RESULT RemoveActiveObject(std::shared_ptr<ActiveObject> pActiveObject);
	std::shared_ptr<ActiveObject> FindActiveObject(VirtualObj *pVirtualObject);
	std::shared_ptr<ActiveObject> FindActiveObject(std::shared_ptr<ActiveObject> pActiveObject);
	ActiveObject::state GetActiveObjectState(VirtualObj *pVirtualObject);
	AnimationQueue* GetAnimationQueue();
	RESULT PushAnimationItem(VirtualObj *pObj,
		point ptPosition,
		vector vScale,
		double duration,
		AnimationItem::AnimationFlags flags = AnimationItem::AnimationFlags());

	RESULT CancelAnimation(VirtualObj *pObj);

	virtual RESULT Notify(SenseControllerEvent *event) override;

	RESULT RegisterSenseController(SenseController *pSenseController);

private:
	std::shared_ptr<ray> m_pInteractionRay = nullptr;
	std::list<std::shared_ptr<ActiveObject>> m_activeObjects;
	
	AnimationQueue* m_pObjectQueue;

/*private:
	// Animation Queue
public:
	RESULT ClearAnimationQueue();

	std::map<VirtualObj *pObject, std::shared_ptr<ActiveObject>> m_activeObjects;
	*/

private:
	double m_diffThreshold = DEFAULT_INTERACTION_DIFF_THRESHOLD;

private:
	UID m_uid;
};

#endif	// ! PHYSICS_ENGINE_H_