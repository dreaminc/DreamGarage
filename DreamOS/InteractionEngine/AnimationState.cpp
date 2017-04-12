#include "AnimationState.h"
#include "Primitives/VirtualObj.h"

RESULT AnimationState::Compose(AnimationState state) {
	ptPosition += state.ptPosition;
	qRotation = state.qRotation;
	vScale = vector(vScale.x() * state.vScale.x(),
					vScale.y() * state.vScale.y(),
					vScale.z() * state.vScale.z());
	return R_PASS;
}

VirtualObj* AnimationState::Apply(VirtualObj* pObj) {
	return pObj->MoveTo(ptPosition)->SetScale(vScale)->SetOrientation(qRotation);
}
