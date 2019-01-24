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

#include "Primitives/Subscriber.h"
#include "Sense/SenseController.h"
#include "Sense/SenseMouse.h"
#include "Sense/SenseKeyboard.h"

#include <vector>
#include <functional>

#include "InteractionObjectEvent.h"
#include "ActiveObject.h"
#include "ActiveObjectQueue.h"
#include <chrono>

#include "Primitives/Multipublisher.h"
#include "Primitives/CapturedObj.h"

#define DEFAULT_INTERACTION_DIFF_THRESHOLD 0.005f
#define FRAME_MS (1000.0/90.0)

class ObjectStore;

struct AnimationFlags;
class AnimationQueue;
enum class AnimationCurveType;
class CollisionManifold;

class SandboxApp;

class DimObj;
class color;
class plane;

class InteractionEngineProxy : public Subscriber<SenseKeyboardEvent> {
public:
	// Animation functions
	virtual RESULT PushAnimationItem(DimObj *pObj,
		point ptPosition,
		quaternion qRotation,
		vector vScale,
		color cColor,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) = 0;

	virtual RESULT PushAnimationItem(DimObj *pObj,
		point ptPosition,
		quaternion qRotation,
		vector vScale,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) = 0;

	virtual RESULT PushAnimationItem(DimObj *pObj,
		color cColor,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) = 0;

	virtual RESULT CancelAnimation(DimObj *pObj) = 0;
	virtual bool IsAnimating(DimObj *pobj) = 0;
	virtual RESULT RemoveAnimationObject(DimObj *pObj) = 0;
	virtual RESULT RemoveAllObjects() = 0;

	// Keyboard manual collision functions
	//virtual std::shared_ptr<ActiveObject> FindActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr) = 0;
	//virtual std::shared_ptr<ActiveObject> AddActiveObject(VirtualObj *pVirtualObject, VirtualObj *pInteractionObject = nullptr) = 0;
	//virtual RESULT SetAllActiveObjectStates(ActiveObject::state newState, VirtualObj *pInteractionObject = nullptr) = 0;

	virtual RESULT Notify(SenseKeyboardEvent *pEvent) = 0;

	virtual RESULT CaptureObject(VirtualObj *pObject, VirtualObj *pInteractionObject, point ptContact, vector vDirection, vector vSurface, float threshold) = 0;
	virtual RESULT ResetObject(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) = 0;
	virtual RESULT ResetObjects(VirtualObj *pInteractionObject) = 0;
	virtual RESULT ReleaseObject(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) = 0;
	virtual RESULT ReleaseObjects(VirtualObj *pInteractionObject) = 0;
	virtual bool HasCapturedObjects(VirtualObj *pInteractionObject) = 0;
	virtual bool IsObjectCaptured(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) = 0;
	virtual std::vector<CapturedObj*> GetCapturedObjects(VirtualObj *pInteractionObject) = 0;

	virtual RESULT SetInteractionDiffThreshold(double thresh) = 0;
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
	RESULT InitializeActiveObjectQueues();

	//RESULT UpdateObjectStoreRay(ObjectStore *pObjectStore, VirtualObj *pInteractionObject);
	//RESULT UpdateObjectStore(ObjectStore *pObjectStore, VirtualObj *pInteractionObject);
	RESULT UpdateObjectStore(ActiveObject::type activeObjectType, ObjectStore *pObjectStore, VirtualObj *pInteractionObject);
	//InteractionEventType UpdateActiveObject(ActiveObject::type activeObjectType, VirtualObj *pInteractionObject, CollisionManifold manifold);
	InteractionEventType UpdateActiveObject(ActiveObject::type activeObjectType, VirtualObj *pInteractionObject, CollisionManifold manifold, VirtualObj *pEventObject);

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

	virtual RESULT CaptureObject(VirtualObj *pObject, VirtualObj *pInteractionObject, point ptContact, vector vDirection, vector vSurface, float threshold) override;
	virtual RESULT ResetObject(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) override;
	virtual RESULT ResetObjects(VirtualObj *pInteractionObject) override;
	virtual RESULT ReleaseObject(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) override;
	virtual RESULT ReleaseObjects(VirtualObj *pInteractionObject) override;
	RESULT RemoveObject(VirtualObj *pEventObject, ObjectStore *pObjectStore);
	RESULT RemoveActiveObjects(std::map<VirtualObj*, std::vector<std::shared_ptr<ActiveObject>>> activeObjectsToRemove, std::pair<ActiveObject::type, ActiveObjectQueue*> activeObjectQueuePair, VirtualObj *pInteractionObject);
	virtual bool HasCapturedObjects(VirtualObj *pInteractionObject) override;
	virtual bool IsObjectCaptured(VirtualObj *pInteractionObject, VirtualObj *pCapturedObj) override;
	virtual std::vector<CapturedObj*> GetCapturedObjects(VirtualObj *pInteractionObject) override;

	RESULT UpdateCapturedObjectStore();
	RESULT UpdateCapturedObjects(VirtualObj *pInteractionObject);

	RESULT AddInteractionObject(VirtualObj *pInteractionObject);
	RESULT RemoveInteractionObject(VirtualObj *pInteractionObject);
	RESULT ClearInteractionObjects();
	VirtualObj *FindInteractionObject(VirtualObj *pInteractionObject);

	virtual RESULT SetInteractionDiffThreshold(double thresh) override;

	//RESULT RegisterSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	// TODO: Register element events etc

	// Active Objects
public:

	virtual RESULT PushAnimationItem(DimObj *pObj,
		point ptPosition,
		quaternion qRotation,
		vector vScale,
		color cColor,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) override;

	virtual RESULT PushAnimationItem(DimObj *pObj,
		point ptPosition,
		quaternion qRotation,
		vector vScale,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) override;

	virtual RESULT PushAnimationItem(DimObj *pObj,
		color cColor,
		double duration,
		AnimationCurveType curve,
		AnimationFlags flags,
		std::function<RESULT(void*)> startCallback = nullptr,
		std::function<RESULT(void*)> endCallback = nullptr,
		void* callbackContext = nullptr) override;

	virtual RESULT CancelAnimation(DimObj *pObj) override;
	virtual bool IsAnimating(DimObj *pobj) override;
	virtual RESULT RemoveAnimationObject(DimObj *pObj) override;
	virtual RESULT RemoveAllObjects() override;

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

	std::map<VirtualObj*, std::vector<CapturedObj*>> m_capturedObjects;

	typedef std::pair<VirtualObj*, CapturedObj*> T_PendingCapturedObj;
	std::vector<T_PendingCapturedObj> m_capturedObjectsToRelease;
	std::vector<T_PendingCapturedObj> m_objectsToCapture;

	std::map<ActiveObject::type, ActiveObjectQueue> m_activeObjectQueues;
	
	AnimationQueue* m_pObjectQueue = nullptr;

private:
	double m_diffThreshold = DEFAULT_INTERACTION_DIFF_THRESHOLD;

	SandboxApp *m_pSandbox = nullptr;

	double m_interactionPadAccumulator = 0.0f;
	bool m_fActiveScroll = false;
	std::chrono::high_resolution_clock::time_point m_tLastUpdate;
	double m_msLastUpdate;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;
	double m_sTimeStep = 0.015f;		// time step in seconds 
	double m_elapsedTime;

private:
	UID m_uid;
};

#endif	// ! PHYSICS_ENGINE_H_
