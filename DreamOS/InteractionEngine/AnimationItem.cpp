#include "AnimationItem.h"
#include "Primitives/VirtualObj.h"

AnimationItem::AnimationItem(AnimationState startState, AnimationState endState, double duration) {
	RESULT r = R_PASS;

	CR(Initialize());
	m_startState = startState;
	m_endState = endState;
	m_duration = duration;

	m_flags = AnimationFlags();

	SetDirty();

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

std::shared_ptr<AnimationItem> AnimationItem::CreateCancelAnimation(VirtualObj *pObj, std::chrono::time_point<std::chrono::steady_clock> tNow) {
	AnimationState startState;
	startState.vScale = vector(1.0f, 1.0f, 1.0f);
	Update(pObj, startState, tNow);
	AnimationState endState = m_startState;
	auto duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();

	return std::make_shared<AnimationItem>(startState, endState, duration);
}

RESULT AnimationItem::Update(VirtualObj *pObj, AnimationState& state, std::chrono::time_point<std::chrono::steady_clock> tNow) {
	RESULT r = R_PASS;

	if (CheckAndCleanDirty()) {
		if (pObj != nullptr) {
			m_startState.ptPosition = pObj->GetPosition();
			m_startState.qRotation = pObj->GetOrientation();
			m_startState.vScale = pObj->GetScale();
		}
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	auto diff = std::chrono::duration<double>(tNow - m_startTime).count();

	double prog = diff / m_duration;
	prog = std::min(1.0, prog);

	AnimationState updateState;
		
	//TODO replace with animation curves 
	updateState.ptPosition = ((float)(1.0 - prog) * m_startState.ptPosition + (float)(prog)* m_endState.ptPosition);
	updateState.vScale = ((float)(1.0 - prog) * m_startState.vScale + (float)(prog)* m_endState.vScale);
	CR(state.Compose(updateState));
Error:
	return r;
}

bool AnimationItem::IsComplete() {

	auto diff = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();
	double prog = diff / m_duration;

	return prog >= 1.0;
}

AnimationItem::AnimationFlags AnimationItem::GetFlags() {
	return m_flags;
}

RESULT AnimationItem::SetFlags(AnimationFlags flags) {
	m_flags = flags;
	return R_PASS;
}