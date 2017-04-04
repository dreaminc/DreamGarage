#include "AnimationItem.h"

#include "Primitives/VirtualObj.h"

RESULT AnimationState::Compose(AnimationState state) {
	ptPosition += state.ptPosition;
	vScale = vector(vScale.x() * state.vScale.x(),
					vScale.y() * state.vScale.y(),
					vScale.z() * state.vScale.z());
	return R_PASS;
}

VirtualObj* AnimationState::Apply(VirtualObj* pObj) {
	return pObj->MoveTo(ptPosition)->SetScale(vScale);
}

AnimationItem::AnimationItem(AnimationState startState, AnimationState endState, double duration) {
	RESULT r = R_PASS;

	CR(Initialize());
	m_startState = startState;
	m_endState = endState;
	m_duration = duration;
	m_flags = 0;

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

RESULT AnimationItem::StartAnimation(VirtualObj *pObj) {
	RESULT r = R_PASS;

	if (CheckAndCleanDirty()) {
		if (pObj != nullptr) {
			m_startState.ptPosition = pObj->GetPosition();
			m_startState.qRotation = pObj->GetOrientation();
			m_startState.vScale = pObj->GetScale();
		}
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	return r;
}

std::shared_ptr<AnimationItem> AnimationItem::CreateCancelAnimation() {
	AnimationState startState = Update();
	AnimationState endState = m_startState;
	auto duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();

	return std::make_shared<AnimationItem>(startState, endState, duration);
}

AnimationState AnimationItem::Update() {
	RESULT r = R_PASS;

	auto diff = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();

	double prog = diff / m_duration;
	prog = std::min(1.0, prog);

	// Linear animation
	//pObj->SetPosition((float)(1.0 - prog) * m_startState.ptPosition + (float)(prog)* m_endState.ptPosition);
	//pObj->SetScale((float)(1.0 - prog) * m_startState.vScale + (float)(prog)* m_endState.vScale);

	AnimationState state;
		
	state.ptPosition = ((float)(1.0 - prog) * m_startState.ptPosition + (float)(prog)* m_endState.ptPosition);
	state.vScale = ((float)(1.0 - prog) * m_startState.vScale + (float)(prog)* m_endState.vScale);
//Error:
	return state;
}

RESULT AnimationItem::EndAnimation() {
	
}

RESULT AnimationItem::UpdateStartTime() {
	m_startTime = std::chrono::high_resolution_clock::now();
	return R_PASS;
}

bool AnimationItem::IsComplete() {

	auto diff = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();
	double prog = diff / m_duration;

	return prog >= 1.0;
}

RESULT AnimationItem::SetFlags(int flags) {
	m_flags = flags;
	return R_PASS;
}

int AnimationItem::GetFlags() {
	return m_flags;
}