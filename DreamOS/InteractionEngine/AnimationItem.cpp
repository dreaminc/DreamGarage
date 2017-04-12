#include "AnimationItem.h"
#include "Primitives/VirtualObj.h"

AnimationItem::AnimationItem(AnimationState startState, AnimationState endState, double startTime, double duration) {
	RESULT r = R_PASS;

	CR(Initialize());

	m_startState = startState;
	m_endState = endState;
	m_duration = duration;
	m_startTime = startTime;
	fnOnAnimationEnded = nullptr;
	fnOnAnimationEndedContext = nullptr;

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

	m_flags = AnimationFlags();

	CR(SetDirty());

Error:
	return r;
}

std::shared_ptr<AnimationItem> AnimationItem::CreateCancelAnimation(VirtualObj *pObj, double msNow) {
	AnimationState startState;
	startState.vScale = vector(1.0f, 1.0f, 1.0f);
	Update(pObj, startState, msNow);
	AnimationState endState = m_startState;
	double duration = msNow - m_startTime;

	return std::make_shared<AnimationItem>(startState, endState, msNow, duration);
}

RESULT AnimationItem::Update(VirtualObj *pObj, AnimationState& state, double msNow) {
	RESULT r = R_PASS;

	if (CheckAndCleanDirty()) {
		if (pObj != nullptr) {
			m_startState.ptPosition = pObj->GetPosition();
			m_startState.qRotation = pObj->GetOrientation();
			m_startState.vScale = pObj->GetScale();
		}
		m_startTime = msNow;
	}

	double diff = msNow - m_startTime;
	double prog = diff / m_duration;
	prog = std::min(1.0, prog);
	prog = m_curveType.GetAnimationProgress(prog);

	AnimationState updateState;
		
	updateState.ptPosition = ((float)(1.0 - prog) * m_startState.ptPosition + (float)(prog)* m_endState.ptPosition);
	updateState.qRotation = m_startState.qRotation.RotateToQuaternionLerp(m_endState.qRotation, prog);
	updateState.vScale = ((float)(1.0 - prog) * m_startState.vScale + (float)(prog)* m_endState.vScale);
	CR(state.Compose(updateState));
Error:
	return r;
}

bool AnimationItem::IsComplete(double msNow) {

	double diff = msNow - m_startTime;
	double prog = diff / m_duration;

	return prog >= 1.0;
}

AnimationFlags AnimationItem::GetFlags() {
	return m_flags;
}

RESULT AnimationItem::SetFlags(AnimationFlags flags) {
	m_flags = flags;
	return R_PASS;
}

RESULT AnimationItem::SetCurveType(AnimationCurveType type) {
	m_curveType = AnimationCurve(type);
	return R_PASS;
}

std::function<RESULT(void*)> AnimationItem::GetAnimationEndedCallback() {
	return fnOnAnimationEnded;
}

RESULT AnimationItem::SetAnimationEndedCallback(std::function<RESULT(void*)> callback) {
	fnOnAnimationEnded = callback;
	return R_PASS;
}

void* AnimationItem::GetCallbackContext() {
	return fnOnAnimationEndedContext;
}

RESULT AnimationItem::SetCallbackContext(void* context) {
	fnOnAnimationEndedContext = context;
	return R_PASS;
}