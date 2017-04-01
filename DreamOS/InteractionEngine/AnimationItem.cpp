#include "AnimationItem.h"

#include "Primitives/VirtualObj.h"

AnimationItem::AnimationItem(AnimationState startState, AnimationState endState, double duration) {
	RESULT r = R_PASS;

	CR(Initialize());
	m_startState = startState;
	m_endState = endState;
	m_duration = duration;

	Validate();
	return;
Error:
	Invalidate();
	return;
}

AnimationItem::~AnimationItem() {
	// empty
}

RESULT AnimationItem::Initialize() {

	RESULT r = R_PASS;

	m_startTime = std::chrono::high_resolution_clock::now();

//Error:
	return r;
}

RESULT AnimationItem::Update(VirtualObj* pObj, double msTimeStep) {
	RESULT r = R_PASS;

	auto diff = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();

	double prog = diff / m_duration;
	prog = std::min(1.0, prog);

	// Linear animation
	pObj->SetPosition((float)(1.0 - prog) * m_startState.ptPosition + (float)(prog)* m_endState.ptPosition);
	pObj->SetScale((float)(1.0 - prog) * m_startState.vScale + (float)(prog)* m_endState.vScale);
//Error:
	return r;
}
