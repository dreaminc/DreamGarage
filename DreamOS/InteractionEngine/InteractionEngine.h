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
//#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"
#include "Sense/SenseController.h"
#include "Sense/SenseMouse.h"
#include "Sense/SenseKeyboard.h"

#include <vector>
#include <functional>

#include "InteractionObjectEvent.h"
#include "ActiveObject.h"
#include <chrono>

#include "Primitives/Multipublisher.h"

#define DEFAULT_INTERACTION_DIFF_THRESHOLD 0.025f

class ObjectStore;

struct AnimationFlags;
class AnimationQueue;
enum class AnimationCurveType;
class CollisionManifold;

class SandboxApp;

class InteractionEngineProxy : public Subscriber<SenseKeyboardEvent> {
public:
	// Animation functions
	virtual RESULT PushAnimationItem(VirtualObj *pObj,
		point ptPosition,
		quaternion qRotation,
		vector vScale,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) = 0;
	virtual RESULT CancelAnimation(VirtualObj *pObj) = 0;

	// Keyboard manual collision functions
	virtual std::shared_ptr<ActiveObject> FindActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr) = 0;
	virtual std::shared_ptr<ActiveObject> AddActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr) = 0;

	virtual RESULT SetAllActiveObjectStates(ActiveObject::state newState, VirtualObj *pInteractionObject = nullptr) = 0;

	virtual RESULT Notify(SenseKeyboardEvent *pEvent) = 0;

	//virtual point GetInteractionRayOrigin() = 0;
};


class InteractionEngine : public valid,
	public InteractionEngineProxy,
	//public Publisher<InteractionEventType, InteractionObjectEvent>,
	public Multipublisher<VirtualObj*, InteractionEventType, InteractionObjectEvent>,
	public Subscriber<SenseControllerEvent>,
	public Subscriber<SenseMouseEvent>,
	//public Subscriber<SenseKeyboardEvent>,		// TODO: This is redundant, both can be one event
	public Subscriber<SenseTypingEvent>
{

public:
	static std::unique_ptr<InteractionEngine> MakeEngine(SandboxApp *pSandbox);

private:
	InteractionEngine(SandboxApp *pSandbox);

	RESULT Initialize();

	//RESULT UpdateObjectStoreRay(ObjectStore *pObjectStore, VirtualObj *pInteractionObject);
	RESULT UpdateObjectStore(ObjectStore *pObjectStore, VirtualObj *pInteractionObject);
	RESULT UpdateActiveObject(VirtualObj *pInteractionObject, VirtualObj *pObject, CollisionManifold manifold, bool fRay);

public:
	RESULT Update();
	RESULT UpdateObjectStore(ObjectStore *pObjectStore);

	RESULT UpdateAnimationQueue();
	RESULT SetInteractionGraph(ObjectStore *pObjectStore);

	/*
	RESULT UpdateInteractionPrimitive(const ray &r);
	virtual point GetInteractionRayOrigin() override;
	RESULT UpdateInteractionRay();
	*/

	RESULT AddInteractionObject(VirtualObj *pInteractionObject);
	RESULT RemoveInteractionObject(VirtualObj *pInteractionObject);
	RESULT ClearInteractionObjects();
	VirtualObj *FindInteractionObject(VirtualObj *pInteractionObject);

	RESULT SetInteractionDiffThreshold(double thresh);

	//RESULT RegisterSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	// TODO: Register element events etc

	// Active Objects
public:
	RESULT ClearActiveObjects(VirtualObj *pInteractionObject = nullptr);
	virtual RESULT SetAllActiveObjectStates(ActiveObject::state newState, VirtualObj *pInteractionObject = nullptr) override;

	virtual std::shared_ptr<ActiveObject> AddActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr) override;
	virtual std::shared_ptr<ActiveObject> FindActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr) override;
	std::shared_ptr<ActiveObject> FindActiveObject(std::shared_ptr<ActiveObject> pActiveObject, VirtualObj *pInteractionObject = nullptr);
	std::vector<std::shared_ptr<ActiveObject>> FindActiveObjectsWithState(ActiveObject::state state, VirtualObj *pInteractionObject = nullptr);

	RESULT RemoveActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr);
	RESULT RemoveActiveObject(std::shared_ptr<ActiveObject> pActiveObject, VirtualObj *pInteractionObject = nullptr);

	ActiveObject::state GetActiveObjectState(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject);
	
	virtual RESULT PushAnimationItem(VirtualObj *pObj,
		point ptPosition,
		quaternion qRotation,
		vector vScale,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) override;

	virtual RESULT CancelAnimation(VirtualObj *pObj) override;

	virtual RESULT Notify(SenseControllerEvent *pEvent) override;
	virtual RESULT Notify(SenseMouseEvent *pEvent) override;
	virtual RESULT Notify(SenseKeyboardEvent *pEvent) override;
	virtual RESULT Notify(SenseTypingEvent *pEvent) override;

	RESULT RegisterSenseController(SenseController *pSenseController);
	RESULT RegisterSenseMouse();
	RESULT RegisterSenseKeyboard();

	InteractionEngineProxy *GetInteractionEngineProxy();

private:
	//std::shared_ptr<ray> m_pInteractionRay = nullptr;
	std::vector<VirtualObj*> m_interactionObjects;
	//std::list<std::shared_ptr<ActiveObject>> m_activeObjects;
	std::map<VirtualObj*, std::list<std::shared_ptr<ActiveObject>>> m_activeObjects;

	AnimationQueue* m_pObjectQueue;

private:
	double m_diffThreshold = DEFAULT_INTERACTION_DIFF_THRESHOLD;

	SandboxApp *m_pSandbox = nullptr;
	double m_interactionPadAccumulator = 0.0f;

private:
	UID m_uid;
};

#endif	// ! PHYSICS_ENGINE_H_
