#include "AnimationQueue.h"
#include "AnimationItem.h"
#include "Primitives/VirtualObj.h"
#include "DreamConsole/DreamConsole.h"

AnimationQueue::AnimationQueue() {
	m_objectQueue = {};
}

AnimationQueue::~AnimationQueue() {
	// empty
}

RESULT AnimationQueue::Update(double sNow) {
	RESULT r = R_PASS;
	for (auto& qObj : m_objectQueue) {
		auto& pObj = qObj.first;
		auto& pQueue = qObj.second;

		if (pQueue.empty()) continue;

		auto& pItem = pQueue.begin();

		AnimationState state;
		state.vScale = vector(1.0f, 1.0f, 1.0f);

		do {
			(*pItem)->Update(pObj, state, sNow);

			// modifying the deque likely invalidates the iterator
			if ((*pItem)->IsComplete(sNow)) {
				if ((*pItem)->GetAnimationEndedCallback() != nullptr) {
					(*pItem)->GetAnimationEndedCallback()((*pItem)->GetCallbackContext());
				}
				pQueue.pop_front();
				continue;
			}
		} while ((*pItem)->GetFlags().fNoBlock && ++pItem != pQueue.end());

		state.Apply(pObj);
	}
//Error:
	return r;
}

RESULT AnimationQueue::PushAnimationItem(
	VirtualObj *pObj, 
	AnimationState endState, 
	double startTime, 
	double duration, 
	AnimationCurveType curve, 
	AnimationFlags flags, 
	std::function<RESULT(void*)> startCallback, 
	std::function<RESULT(void*)> endCallback, 
	void* callbackContext) {

	RESULT r = R_PASS;

	AnimationState startState;
	startState.ptPosition = pObj->GetOrigin();
	startState.qRotation = pObj->GetOrientation();
	startState.vScale = pObj->GetScale();

	std::shared_ptr<AnimationItem> pItem = std::make_shared<AnimationItem>(startState, endState, startTime, duration);

	pItem->SetFlags(flags);
	pItem->SetCurveType(curve);
	pItem->SetAnimationStartCallback(startCallback);
	pItem->SetAnimationEndedCallback(endCallback);
	pItem->SetCallbackContext(callbackContext);

	m_objectQueue[pObj].push_back(pItem);

	// TODO is this the right place to call this?
	CNR(startCallback, R_PASS);
	CR(startCallback(callbackContext));

Error:
	return r;
}

RESULT AnimationQueue::CancelAnimation(VirtualObj *pObj, double startTime) {
	RESULT r = R_PASS;
	
	auto& qObj = m_objectQueue[pObj];

	auto pNewItem = qObj.front()->CreateCancelAnimation(pObj, startTime);

	qObj.pop_front();
	qObj.push_front(pNewItem);

	return r;
}