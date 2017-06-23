#include "AnimationItem.h"
#include "Primitives/DimObj.h"

AnimationItem::AnimationItem(AnimationState startState, AnimationState endState, double startTime, double duration) {
	RESULT r = R_PASS;

	CR(Initialize());

	m_startState = startState;
	m_endState = endState;
	m_duration = duration;
	m_startTime = startTime;
	m_fnOnAnimationStart = nullptr;
	m_fnOnAnimationEnded = nullptr;
	m_fnOnAnimationContext = nullptr;

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

std::shared_ptr<AnimationItem> AnimationItem::CreateCancelAnimation(DimObj *pObj, double msNow) {
	AnimationState startState;
	startState.vScale = vector(1.0f, 1.0f, 1.0f);
	Update(pObj, startState, msNow);
	AnimationState endState = m_startState;
	double duration = msNow - m_startTime;

	return std::make_shared<AnimationItem>(startState, endState, msNow, duration);
}

RESULT AnimationItem::Update(DimObj *pObj, AnimationState& state, double msNow) {
	RESULT r = R_PASS;

	if (CheckAndCleanDirty()) {
		if (pObj != nullptr) {
			m_startState.ptPosition = pObj->GetPosition();
			m_startState.qRotation = pObj->GetOrientation();
			m_startState.vScale = pObj->GetScale();
			m_startState.cColor = pObj->GetColor();
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

	auto vColor = ((float)(1.0 - prog) * m_startState.cColor + (float)(prog)* m_endState.cColor);
	updateState.cColor.SetColor(vColor.element(0, 0), vColor.element(1, 0), vColor.element(2, 0), vColor.element(3, 0));

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
	return m_fnOnAnimationEnded;
}

RESULT AnimationItem::SetAnimationEndedCallback(std::function<RESULT(void*)> callback) {
	m_fnOnAnimationEnded = callback;
	return R_PASS;
}

std::function<RESULT(void*)> AnimationItem::GetAnimationStartCallback() {
	return m_fnOnAnimationStart;
}

RESULT AnimationItem::SetAnimationStartCallback(std::function<RESULT(void*)> callback) {
	m_fnOnAnimationStart = callback;
	return R_PASS;
}

void* AnimationItem::GetCallbackContext() {
	return m_fnOnAnimationContext;
}

RESULT AnimationItem::SetCallbackContext(void* context) {
	m_fnOnAnimationContext = context;
	return R_PASS;
}