#include "hand.h"
#include "Sense/SenseLeapMotionHand.h"
#include "DreamConsole/DreamConsole.h"
#include "Primitives/sphere.h"
#include "Primitives/model/model.h"
#include "Primitives/DimObj.h"
#include "Primitives/model/mesh.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationItem.h"

hand::hand(HALImp* pHALImp, HAND_TYPE type) :
	composite(pHALImp)
{
	Initialize(type);
}

RESULT hand::SetFrameOfReferenceObject(std::shared_ptr<DimObj> pParent, const hand::HandState& pHandState) {

	if (!CompareParent(pParent.get()) && pHandState.fOriented)
		pParent->AddChild(std::shared_ptr<DimObj>(this));

	return R_PASS;
}

std::shared_ptr<composite> hand::GetModel(HAND_TYPE handType) {
	return m_pModel;
}

RESULT hand::Initialize(HAND_TYPE type) {
	RESULT r = R_PASS;

	float palmRadius = 0.01f;
	point ptModel = point(0.0f, 0.0f, 0.08f);
	float scaleModel = 0.015f;

	//m_pPalm = AddSphere(palmRadius, 10, 10);

	SetPosition(point(0.0f, 0.0f, -1.0f));

#ifndef _DEBUG
	if (type == HAND_TYPE::HAND_LEFT) {
		m_pModel = AddModel(L"\\face4\\LeftHand.obj");
		m_pModel->SetOrientationOffset((float)(-M_PI_2), (float)(M_PI_2), 0.0f);
	}
	
	if (type == HAND_TYPE::HAND_RIGHT) {
		m_pModel = AddModel(L"\\face4\\RightHand.obj");
		m_pModel->SetOrientationOffset((float)(-M_PI_2), (float)(-M_PI_2), 0.0f);
	}

	CN(m_pModel);

	m_pModel->SetPosition(ptModel);
	m_pModel->SetScale(scaleModel);
						
	m_pPhantomVolume = MakeVolume(0.25);
	CN(m_pPhantomVolume);
	m_pPhantomVolume->SetVisible(false);
	AddObject(m_pPhantomVolume);

#else
	//m_pModel = AddComposite();
	//m_pModel->AddVolume(0.02f);
	m_pModel = AddModel(L"cube.obj");
	m_pModel->SetScale(0.02f);
#endif
	
	m_fOriented = false;

	m_qRotation = GetOrientation();

	m_fTracked = false;
	//Start all visibility at false
	CR(OnLostTrack());	//CR here because the only other C is inside of the #ifndef

Error:
	return r;
}

RESULT hand::InitializeWithContext(DreamOS *pDreamOS) {
	RESULT r = R_PASS;

	CN(pDreamOS);
	m_pDreamOS = pDreamOS;

	auto pHMD = m_pDreamOS->GetHMD();
	CNR(pHMD, R_SKIPPED);

	m_pController = pHMD->GetSenseControllerObject((ControllerType)(m_handType));
	CN(m_pController);
	m_pController->SetVisible(false);

	float scale = 0.035f;
	float overlayAspect = (332.0f / 671.0f);
	float t = m_handType == HAND_TYPE::HAND_RIGHT ? 1.0f : -1.0f;
	m_pOverlayQuad = m_pController->MakeQuad(scale / overlayAspect, scale);
	m_pDreamOS->AddObjectToUIGraph(m_pOverlayQuad.get());
	m_pOverlayQuad->SetPosition(point(-scale * 0.575f * t, 0.0f, -scale * 0.575f));
	m_pOverlayQuad->SetVisible(false);

Error:
	return r;
}

hand::ModelState hand::GetModelState() {
	return m_modelState;
}

RESULT hand::SetModelState(ModelState modelState) {
	RESULT r = R_PASS;

	CBR(m_modelState != modelState, R_SKIPPED);

	switch (m_modelState) {
	case ModelState::HAND: {
		HideModel();
	} break;
	case ModelState::CONTROLLER: {
		HideController();
	} break;
	}

	switch (modelState) {
	case ModelState::HAND: {
		ShowModel();
	} break;
	case ModelState::CONTROLLER: {
		ShowController();
		m_pOverlayQuad->SetVisible(m_fTracked && m_fOverlayVisible);
	//	ShowObject(m_pController, HAND_ANIMATION_DURATION);
	} break;
	}

	m_modelState = modelState;

Error:
	return r;
}

RESULT hand::Update() {
	RESULT r = R_PASS;

	switch (m_modelState) {
	case ModelState::HAND: {
		m_pModel->SetVisible(m_fTracked);
	} break;
	case ModelState::CONTROLLER: {
		m_pController->SetVisible(m_fTracked);
		if (!m_pOverlayQuad->IsVisible())
			m_pOverlayQuad->SetVisible(m_fOverlayVisible && m_fTracked);
	} break;
	}

	return r;
}

RESULT hand::SetOverlayVisible(bool fVisible) {
	RESULT r = R_PASS;

	if (m_fOverlayVisible != fVisible && m_pOverlayQuad != nullptr) {
		if (fVisible) {
			ShowOverlay();
		}
		else {
			HideOverlay();
		}
	}
	m_fOverlayVisible = fVisible;
//	CNR(m_pOverlayQuad, R_SKIPPED);
//	m_pOverlayQuad->SetVisible(fVisible);

//Error:
	return r;
}

RESULT hand::SetOverlayTexture(texture *pOverlayTexture) {
	RESULT r = R_PASS;

	CN(pOverlayTexture);
	m_pOverlayQuad->SetDiffuseTexture(pOverlayTexture);

Error:
	return r;
}

std::shared_ptr<volume> hand::GetPhantomVolume() {
	return m_pPhantomVolume;
}

RESULT hand::SetOriented(bool fOriented) {
	m_fOriented = fOriented;
	return R_PASS;
}

bool hand::IsOriented() {
	return m_fOriented;
}

RESULT hand::SetTracked(bool fTracked) {
	m_fTracked = fTracked;
	return R_PASS;
}

bool hand::IsTracked() {
	return m_fTracked;
}

RESULT hand::OnLostTrack() {
	m_fTracked = false;
	
	m_pModel->SetVisible(m_fTracked);
	
	//m_pPalm->SetVisible(m_fTracked);

	return R_PASS;
}

RESULT hand::SetLocalOrientation(quaternion qRotation) {
	m_qRotation = qRotation;
	return R_PASS;
}


RESULT hand::SetHandModel(HAND_TYPE type) {
	RESULT r = R_PASS;

	CBR(type == HAND_TYPE::HAND_SKELETON, R_SKIPPED) 

	SetVisible();
	m_pModel->SetVisible(true);

Error:
	return r;
}

RESULT hand::SetHandModelOrientation(quaternion qOrientation) {
	m_pModel->SetOrientation(qOrientation);
	return R_PASS;
}

RESULT hand::SetHandState(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	point pt = pHandState.ptPalm;
	SetPosition(pt);

	m_handType = pHandState.handType;
	SetHandModel(pHandState.handType);

	m_fTracked = pHandState.fTracked;
	if (!m_fTracked)
		OnLostTrack();

	m_pModel->SetOrientation(pHandState.qOrientation);

//Error:
	return r;
}

hand::HandState hand::GetHandState() {
	hand::HandState handState = {
		m_handType,
		GetPosition(true),
		m_qRotation,
		m_fOriented,
		m_fTracked
	};

	return handState;
}

hand::HandState hand::GetDebugHandState(HAND_TYPE handType) {
	hand::HandState handState = {
		handType,
		point(1,2,3),
		quaternion(),
		false,
		false
	};

	return handState;
}

RESULT hand::HideModel() {
	RESULT r = R_PASS;

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pModel->SetVisible(false);
		return r;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pModel.get(), 
		color(1.0f, 1.0f, 1.0f, 0.0f), 
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		nullptr,
		fnVisibleCallback,
		this));
Error:
	return r;
}

RESULT hand::ShowModel() {
	RESULT r = R_PASS;

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pModel->SetVisible(true && m_fTracked);
		return r;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pModel.get(), 
		color(1.0f, 1.0f, 1.0f, 1.0f), 
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		fnVisibleCallback,
		nullptr,
		this));

Error:
	return r;
}

RESULT hand::HideController() {
	RESULT r = R_PASS;
	//CNR(m_pDreamOS, R_SKIPPED);

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pController->SetVisible(false);
		return r;
	};

	auto pMesh = m_pController->GetFirstChild<mesh>().get();
	CNR(pMesh, R_SKIPPED);

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
//		m_pController, 
		pMesh,
		color(1.0f, 1.0f, 1.0f, 0.0f), 
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		nullptr,
		fnVisibleCallback,
		this));

Error:
	return r;
}

RESULT hand::ShowController() {
	RESULT r = R_PASS;
	//CNR(m_pDreamOS, R_SKIPPED);

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pController->SetVisible(true && m_fTracked);
		m_pOverlayQuad->SetVisible(m_fTracked && m_fOverlayVisible);
		return r;
	};

	auto pMesh = m_pController->GetFirstChild<mesh>().get();
	CNR(pMesh, R_SKIPPED);

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
//		m_pController, 
		pMesh,
		color(1.0f, 1.0f, 1.0f, 1.0f), 
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		fnVisibleCallback,
		nullptr,
		this));

Error:
	return r;
}

RESULT hand::HideOverlay() {
	RESULT r = R_PASS;
	//CNR(m_pDreamOS, R_SKIPPED);

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pOverlayQuad->SetVisible(false);
		return r;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pOverlayQuad.get(), 
		color(1.0f, 1.0f, 1.0f, 0.0f), 
		OVERLAY_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		nullptr,
		fnVisibleCallback,
		this));

Error:
	return r;
}

RESULT hand::ShowOverlay() {
	RESULT r = R_PASS;
	//CNR(m_pDreamOS, R_SKIPPED);

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pOverlayQuad->SetVisible(true && m_fTracked && m_fOverlayVisible);
		return r;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pOverlayQuad.get(), 
		color(1.0f, 1.0f, 1.0f, 1.0f), 
		OVERLAY_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		fnVisibleCallback,
		nullptr,
		this));

Error:
	return r;
}
