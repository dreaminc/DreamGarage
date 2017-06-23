#include "AnimationState.h"

#include "Primitives/DimObj.h"

RESULT AnimationState::Compose(AnimationState state) {
	ptPosition += state.ptPosition;
	qRotation = state.qRotation;
	vScale = vector(vScale.x() * state.vScale.x(),
					vScale.y() * state.vScale.y(),
					vScale.z() * state.vScale.z());
	//TODO: color
	return R_PASS;
}

DimObj* AnimationState::Apply(DimObj* pObj) {

	pObj->SetColor(cColor);
	pObj->MoveTo(ptPosition)->SetScale(vScale)->SetOrientation(qRotation);
	return pObj;
}
