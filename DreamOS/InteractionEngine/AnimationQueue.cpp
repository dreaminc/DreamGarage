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

RESULT AnimationQueue::Update(double msTimeStep) {
	RESULT r = R_PASS;
	for (auto& qObj : m_objectQueue) {
		auto& pObj = qObj.first;
		auto& pQueue = qObj.second;
		OVERLAY_DEBUG_SET("size", (int)pQueue.size());
		if (pQueue.empty()) continue;

		auto& pItem = pQueue.begin();

		(*pItem)->StartAnimation(pObj);
		AnimationState state = (*pItem)->Update();

		if ((*pItem)->IsComplete()) {
			pQueue.pop_front();
			continue;
		}
		while ((*pItem)->GetFlags() == (int)AnimationItem::AnimationFlags::NO_BLOCK && ++pItem != pQueue.end()) {
			(*pItem)->StartAnimation(pObj);
			state.Compose((*pItem)->Update());
		}

		// while pItem.flags & NO_BLOCK && !pQueue.end()
			// pItem = next()
			// state.compose(pItem->Update())
		//  

		state.Apply(pObj);
	}
//Error:
	return r;
}

RESULT AnimationQueue::PushAnimationItem(VirtualObj *pObj, AnimationState endState, double duration, int flags) {//, AnimationCurveType curve) {
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

	auto pNewItem = qObj.front()->CreateCancelAnimation();

	qObj.pop_front();
	qObj.push_front(pNewItem);

	return r;
}