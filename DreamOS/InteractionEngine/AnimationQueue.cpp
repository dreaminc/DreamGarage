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

RESULT AnimationQueue::Update() {
	RESULT r = R_PASS;
	for (auto& qObj : m_objectQueue) {
		auto& pObj = qObj.first;
		auto& pQueue = qObj.second;

		if (pQueue.empty()) continue;

		auto& pItem = pQueue.begin();

		auto tNow = std::chrono::high_resolution_clock::now();

		AnimationState state;
		state.vScale = vector(1.0f, 1.0f, 1.0f);

		do {
			(*pItem)->Update(pObj, state, tNow);

			// modifying the deque likely invalidates the iterator
			if ((*pItem)->IsComplete()) {
				pQueue.pop_front();
				continue;
			}
		} while ((*pItem)->GetFlags().fNoBlock && ++pItem != pQueue.end());

		state.Apply(pObj);
	}
//Error:
	return r;
}

RESULT AnimationQueue::PushAnimationItem(VirtualObj *pObj, AnimationState endState, double duration, AnimationItem::AnimationFlags flags) {//, AnimationCurveType curve) {
	RESULT r = R_PASS;

	AnimationState startState;
	startState.ptPosition = pObj->GetOrigin();
	startState.qRotation = pObj->GetOrientation();
	startState.vScale = pObj->GetScale();

	std::shared_ptr<AnimationItem> pItem = std::make_shared<AnimationItem>(startState, endState, duration);
	pItem->SetFlags(flags);

	m_objectQueue[pObj].push_back(pItem);

//Error:
	return r;
}

RESULT AnimationQueue::CancelAnimation(VirtualObj *pObj) {
	RESULT r = R_PASS;
	
	auto& qObj = m_objectQueue[pObj];

	auto tNow = std::chrono::high_resolution_clock::now();

	auto pNewItem = qObj.front()->CreateCancelAnimation(pObj, tNow);

	qObj.pop_front();
	qObj.push_front(pNewItem);

	return r;
}