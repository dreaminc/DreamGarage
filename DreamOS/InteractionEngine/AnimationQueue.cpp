#include "AnimationQueue.h"
#include "AnimationItem.h"
#include "Primitives/DimObj.h"
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
				break;
			}
		} while ((*pItem)->GetFlags().fNoBlock && ++pItem != pQueue.end());

		state.Apply(pObj);
	}
//Error:
	return r;
}

RESULT AnimationQueue::PushAnimationItem(
	DimObj *pObj, 
	AnimationState endState, 
	double startTime, 
	double duration, 
	AnimationCurveType curve, 
	AnimationFlags flags, 
	std::function<RESULT(void*)> fnStartCallback, 
	std::function<RESULT(void*)> fnEndCallback, 
	void* pCallbackContext) {

	RESULT r = R_PASS;

	AnimationState startState;
	startState.ptPosition = pObj->GetOrigin();
	startState.qRotation = pObj->GetOrientation();
	startState.vScale = pObj->GetScale();
	startState.cColor = pObj->GetMaterial()->GetDiffuseColor();

	std::shared_ptr<AnimationItem> pItem = std::make_shared<AnimationItem>(startState, endState, startTime, duration);

	pItem->SetFlags(flags);
	pItem->SetCurveType(curve);
	pItem->SetAnimationStartCallback(fnStartCallback);
	pItem->SetAnimationEndedCallback(fnEndCallback);
	pItem->SetCallbackContext(pCallbackContext);

	m_objectQueue[pObj].push_back(pItem);

	CNR(fnStartCallback, R_PASS);
	CR(fnStartCallback(pCallbackContext));

Error:
	return r;
}

RESULT AnimationQueue::CancelAnimation(DimObj *pObj, double startTime) {
	RESULT r = R_PASS;
	
	auto& qObj = m_objectQueue[pObj];

	auto pNewItem = qObj.front()->CreateCancelAnimation(pObj, startTime);

	qObj.pop_front();
	qObj.push_front(pNewItem);

	return r;
}

RESULT AnimationQueue::RemoveAnimationObject(DimObj *pObj) {
	m_objectQueue[pObj] = std::deque<std::shared_ptr<AnimationItem>>();
	return R_PASS;
}

RESULT AnimationQueue::RemoveAllObjects() {
	m_objectQueue.clear();
	return R_PASS;
}