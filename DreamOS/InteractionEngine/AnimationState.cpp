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
	//TODO: currently setting diffuse color through the material
	// will need an overhaul with the shaders
	//pObj->GetMaterial()->SetDiffuseColor(cColor);
	//pObj->GetMaterial()->SetColors(cColor, cColor, cColor);
	pObj->SetMaterialColors(cColor, true);
	pObj->MoveTo(ptPosition)->SetScale(vScale)->SetOrientation(qRotation);
	return pObj;
}
