#include "hand.h"
#include "Sense/SenseLeapMotionHand.h"
#include "Primitives/sphere.h"
#include "Primitives/model/model.h"
#include "Primitives/DimObj.h"
#include "Primitives/model/mesh.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationItem.h"
#include "Core/Utilities.h"

#include "DreamGarage/UICommon.h"

hand::hand(HALImp* pHALImp, HAND_TYPE type) :
	composite(pHALImp)
{
	RESULT r = R_PASS;
	CR(Initialize(type));

	Validate();
	return;
Error:
	Invalidate();
	return;
}

hand::hand(HALImp* pHALImp, HAND_TYPE type, long avatarModelID) :
	composite(pHALImp)
{
	RESULT r = R_PASS;
	CR(Initialize(type, avatarModelID));

	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT hand::SetFrameOfReferenceObject(std::shared_ptr<DimObj> pParent, const hand::HandState& pHandState) {

	if (!CompareParent(pParent.get()) && pHandState.fOriented)
		pParent->AddChild(std::shared_ptr<DimObj>(this));

	return R_PASS;
}

std::shared_ptr<composite> hand::GetModel(HAND_TYPE handType) {
	return m_pModel;
}

RESULT hand::Initialize(HAND_TYPE type, long avatarModelID) {
	RESULT r = R_PASS;


	// why
	SetPosition(point(0.0f, 0.0f, -1.0f));

	m_handType = type;
	
	m_fOriented = false;

	m_qRotation = GetOrientation();

	m_fTracked = false;
	//Start all visibility at false
	CR(OnLostTrack());	//CR here because the only other C is inside of the #ifndef

	m_avatarModelId = avatarModelID;

	if (m_avatarModelId >= 1 && m_avatarModelId <= NUM_AVATARS) {
		LoadHandModel();
	}

Error:
	return r;
}

RESULT hand::PendCreateHandModel(long avatarModelID) {
	RESULT r = R_PASS;

	CBM(avatarModelID >= 1 && avatarModelID <= NUM_AVATARS, "invalid avatar model id %d", avatarModelID);
	
	m_fLoadHandModel = true;
	m_avatarModelId = avatarModelID;

Error:
	return r;
}

RESULT hand::LoadHandModel() {
	RESULT r = R_PASS;

	// why
	float palmRadius = 0.01f;
	point ptModel = point(0.0f, 0.0f, 0.08f);
	float scaleModel = 0.01f;
	color modelColor;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;

	// don't create hands twice
	CBR(m_pModel, R_SKIPPED);

#ifndef _DEBUG

	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	if (m_handType == HAND_TYPE::HAND_LEFT) {

		std::wstring wstrModel = wstrAssetPath + k_wstrFolder + std::to_wstring(m_avatarModelId) + L"/" + k_wstrLeft + k_wstrFileType;
		m_pModel = AddModel(wstrModel);

		vector vLeftHandOffset = vector(0.0f, (float)(M_PI), (float)(M_PI_2));
		m_pModel->SetOrientationOffset(vLeftHandOffset);
	}
	
	if (m_handType == HAND_TYPE::HAND_RIGHT) {

		std::wstring wstrModel = wstrAssetPath + k_wstrFolder + std::to_wstring(m_avatarModelId) + L"/" + k_wstrRight + k_wstrFileType;
		m_pModel = AddModel(wstrModel, ModelFactory::flags::FLIP_WINDING);

		vector vRightHandOffset = vector(0.0f, (float)(M_PI), (float)(-M_PI_2));
		m_pModel->SetOrientationOffset(vRightHandOffset);
	}

	CN(m_pModel);

	m_pModel->SetPosition(ptModel);
	m_pModel->SetScale(scaleModel);

	m_pModel->SetVisible(m_fTracked && m_modelState == ModelState::HAND);

	// TODO: this is bad
	modelColor = ((model*)(m_pModel.get()))->GetChildMesh(0)->GetDiffuseColor();
	((model*)(m_pModel.get()))->SetMaterialSpecularColor(modelColor, true);
	((model*)(m_pModel.get()))->SetMaterialShininess(4.0f, true);

#else

	//m_pModel = AddComposite();
	//m_pModel->AddVolume(0.02f);
	m_pModel = AddModel(L"cube.obj");
	m_pModel->SetScale(0.02f);
#endif

	m_fLoadHandModel = false;

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

	//TODO: several unique positioning variables per device here that aren't used anywhere else
	switch (pHMD->GetDeviceType()) {
	case (HMDDeviceType::OCULUS): {

		float scale = OVR_OVERLAY_SCALE;
		float overlayAspect = OVR_OVERLAY_ASPECT_RATIO;
		float t = m_handType == HAND_TYPE::HAND_RIGHT ? 1.0f : -1.0f;
		m_pDreamOS->AddObjectToUIGraph(m_pController);
		// re-enable if overlays are used again
		/*
		m_pOverlayQuad = m_pController->AddQuad(scale / overlayAspect, scale);
		m_pOverlayQuad->SetPosition(point(scale * t * OVR_OVERLAY_POSITION_X, 
										scale * OVR_OVERLAY_POSITION_Y, 
										scale * OVR_OVERLAY_POSITION_Z));
		m_pOverlayQuad->SetVisible(false);
		//*/

	} break;
	case (HMDDeviceType::VIVE): {

		float scale = VIVE_OVERLAY_SCALE;
		float overlayAspect = VIVE_ASPECT_RATIO;
		float t = m_handType == HAND_TYPE::HAND_RIGHT ? 1.0f : -1.0f;

		m_pDreamOS->AddObjectToUIGraph(m_pController);
		// re-enable if overlays are used again
		/*
		m_pOverlayQuad = m_pController->AddQuad(scale / overlayAspect, scale);
		m_pOverlayQuad->SetPosition(point(scale * t * VIVE_OVERLAY_POSITION_X, 
										scale * VIVE_OVERLAY_POSITION_Y, 
										scale * VIVE_OVERLAY_POSITION_Z));
		m_pOverlayQuad->SetVisible(false);
		//*/

	} break;
	case (HMDDeviceType::META): {
		float scale = VIVE_OVERLAY_SCALE;
		float overlayAspect = VIVE_ASPECT_RATIO;
		float t = m_handType == HAND_TYPE::HAND_RIGHT ? 1.0f : -1.0f;
		/*
		m_pOverlayQuad = m_pController->AddQuad(scale / overlayAspect, scale);
		m_pDreamOS->AddObjectToUIGraph(m_pController);
		m_pOverlayQuad->SetPosition(point(scale * t * VIVE_OVERLAY_POSITION_X,
			scale * VIVE_OVERLAY_POSITION_Y,
			scale * VIVE_OVERLAY_POSITION_Z));
		m_pOverlayQuad->SetVisible(false);
		//*/
	} break;
	}


Error:
	return r;
}

hand::ModelState hand::GetModelState() {
	return m_modelState;
}

RESULT hand::SetModelState(ModelState modelState) {
	RESULT r = R_PASS;

	CBR(m_modelState != modelState, R_SKIPPED);
	//CNR(m_modelState)
	/*
	m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(m_pModel.get());
	auto pMesh = m_pController->GetFirstChild<mesh>().get();
	m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(pMesh);
	m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(m_pOverlayQuad.get());
	//*/
	
	if (m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pModel.get())) {
		m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(m_pModel.get());
	}

	switch (m_modelState) {
	case ModelState::HAND: {
		if (m_pModel != nullptr) {
			HideModel();
		}
	} break;
	case ModelState::CONTROLLER: {
		HideController();
	} break;
	}

	switch (modelState) {
	case ModelState::HAND: {
		if (m_pModel != nullptr) {
			ShowModel();
		}
	} break;
	case ModelState::CONTROLLER: {
		ShowController();
	//	ShowObject(m_pController, HAND_ANIMATION_DURATION);
	} break;
	}

	m_modelState = modelState;

Error:
	return r;
}

RESULT hand::Update() {
	RESULT r = R_PASS;

	if (m_fLoadHandModel) {
		CR(LoadHandModel());
	}

	switch (m_modelState) {
	case ModelState::HAND: {
		if (m_pModel != nullptr) {
			m_pModel->SetVisible(m_fTracked);
		}
	} break;
	case ModelState::CONTROLLER: {
		m_pController->SetVisible(m_fTracked);
	} break;
	}

Error:
	return r;
}

RESULT hand::SetVisible(bool fVisible, bool fSetChildren /* = true */) {
	RESULT r = R_PASS;

	//Ensure hand is not set to visible while not tracked
	CR(DimObj::SetVisible(fVisible && m_fTracked, fSetChildren));
	//Ensure phantom volume is not set to visible
	if (m_pPhantomVolume != nullptr) {
		m_pPhantomVolume->SetVisible(false);
	}

Error:
	return r;
}

RESULT hand::SetOverlayVisible(bool fVisible) {
	RESULT r = R_PASS;

	if (m_fOverlayVisible != fVisible && m_pOverlayQuad != nullptr) {
		if (fVisible) {
			CR(ShowOverlay());
		}
		else {
			CR(HideOverlay());
		}
	}
	m_fOverlayVisible = fVisible;

Error:
	return r;
}

bool hand::IsOverlayVisible() {
	return m_fOverlayVisible;
}

RESULT hand::SetOverlayTexture(texture *pOverlayTexture) {
	RESULT r = R_PASS;

	CN(pOverlayTexture);

	// TODO: change to CN if overlays are re-enabled
	CNR(m_pOverlayQuad, R_SKIPPED);

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
	
	if (m_pModel != nullptr) {
		m_pModel->SetVisible(m_fTracked);
	}

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

	if (m_pModel != nullptr) {
		m_pModel->SetOrientation(pHandState.qOrientation);
	}

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

	m_pModel->SetVisible(false);

	/*
	color matColor = m_pModel->GetDiffuseColor();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pModel.get(), 
		color(matColor.r(), matColor.g(), matColor.b(), 0.0f),
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		nullptr,
		fnVisibleCallback,
		this));
	//*/

Error:
	return r;
}

RESULT hand::ShowModel() {
	RESULT r = R_PASS;

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		m_pModel->SetVisible(m_fTracked);
		m_pOverlayQuad->SetVisible(false);

		return r;
	};
	m_pModel->SetVisible(m_fTracked);

	if (m_pOverlayQuad != nullptr) {
		m_pOverlayQuad->SetVisible(false);
	}

	/*
	color matColor = m_pModel->GetDiffuseColor();

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pModel.get(),
		color(matColor.r(), matColor.g(), matColor.b(), 0.0f),
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags()));

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pModel.get(),
		color(matColor.r(), matColor.g(), matColor.b(), 1.0f),
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags(),
		fnVisibleCallback,
		nullptr,
		this));
		//*/

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

	m_pController->SetVisible(false);

	/*
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
		//*/

Error:
	return r;
}

RESULT hand::ShowController() {
	RESULT r = R_PASS;
	//CNR(m_pDreamOS, R_SKIPPED);

	auto fnVisibleCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pController->SetVisible(m_fTracked);
		m_pOverlayQuad->SetVisible(false);
		return r;
	};

	m_pController->SetVisible(m_fTracked);

	auto pMesh = m_pController->GetFirstChild<mesh>().get();
	CNR(pMesh, R_SKIPPED);


	// TODO: re-enable if overlays come back
	if (m_pOverlayQuad != nullptr) {
		m_pOverlayQuad->SetVisible(false);
	}

	/*
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		pMesh,
		color(1.0f, 1.0f, 1.0f, 0.0f), 
		HAND_ANIMATION_DURATION, 
		AnimationCurveType::SIGMOID, 
		AnimationFlags()));

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
		//*/

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
		m_pOverlayQuad->SetVisible(m_fTracked && m_fOverlayVisible);
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
