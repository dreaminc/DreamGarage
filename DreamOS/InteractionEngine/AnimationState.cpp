#include "AnimationState.h"

#include "Primitives/DimObj.h"

RESULT AnimationState::Compose(AnimationState state) {
	ptPosition += state.ptPosition;
	qRotation = state.qRotation;
	vScale = vector(vScale.x() * state.vScale.x(),
					vScale.y() * state.vScale.y(),
					vScale.z() * state.vScale.z());
	cColor = state.cColor;
	return R_PASS;
}

DimObj* AnimationState::Apply(DimObj* pObj) {

//	pObj->SetColor(cColor);
	//currently setting diffuse color through the material
	pObj->GetMaterial()->SetDiffuseColor(cColor);
	pObj->MoveTo(ptPosition)->SetScale(vScale)->SetOrientation(qRotation);
	return pObj;
}
