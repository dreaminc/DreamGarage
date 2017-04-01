#include "AnimationQueue.h"
#include "AnimationItem.h"
#include "Primitives/VirtualObj.h"

AnimationQueue::AnimationQueue() {
	m_objectQueue = {};
}

AnimationQueue::~AnimationQueue() {
	// empty
}

RESULT AnimationQueue::Update(double msTimeStep) {
	RESULT r = R_PASS;

	for (auto p : m_objectQueue) {
		auto& pObj = p.first;
		auto& pQueue = p.second;

		pQueue.front()->Update(pObj, msTimeStep);
		//remove if done
	}
//Error:
	return r;
}

RESULT AnimationQueue::PushAnimationItem(VirtualObj *pObj, AnimationState endState, double duration) {//, AnimationCurveType curve) {
	RESULT r = R_PASS;

	AnimationState startState;
	startState.ptPosition = pObj->GetOrigin();
	startState.qRotation = pObj->GetOrientation();
	startState.vScale = pObj->GetScale();

	std::shared_ptr<AnimationItem> pItem = std::make_shared<AnimationItem>(startState, endState, duration);

	m_objectQueue[pObj].push(pItem);

//Error:
	return r;
}