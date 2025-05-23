#include "hand.h"

#include "os/DreamOS.h"

#include "core/Utilities.h"

#include "core/model/model.h"
#include "core/model/mesh.h"

#include "core/primitives/sphere.h"

#include "core/dimension/DimObj.h"

// TODO: Why is this here
#include "ui/UICommon.h"

#include "modules/AnimationEngine/AnimationItem.h"

#include "sense/SenseLeapMotionHand.h"

#include "sandbox/PathManager.h"

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

RESULT hand::Initialize(HAND_TYPE type, long avatarModelID) {
	RESULT r = R_PASS;

	m_handType = type;
	
	m_fTracked = false;

	//m_pHMDComposite = AddComposite();

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

	// replace hands
	if (m_pModel != nullptr) {
		RemoveChild(m_pModel);
		m_pModel = nullptr;

		RemoveChild(m_pPhantomModel);
		m_pPhantomModel = nullptr;

		if (m_pDreamOS != nullptr) {
			m_pDreamOS->RemoveObject(m_pPhantomModel.get());
		}
	}

#ifndef _DEBUG

	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	if (m_handType == HAND_TYPE::HAND_LEFT) {

		std::wstring wstrModel = wstrAssetPath + k_wstrFolder + std::to_wstring(m_avatarModelId) + L"/" + k_wstrLeft + k_wstrFileType;
		//m_pModel = m_pHMDComposite->AddModel(wstrModel);
		m_pModel = AddModel(wstrModel);
		m_pPhantomModel = MakeModel(wstrModel);

		if (m_pDreamOS != nullptr) {
			m_pDreamOS->AddObject(m_pPhantomModel.get(), Sandbox::PipelineType::AUX);
		}

		vector vLeftHandOffset = vector(0.0f, (float)(M_PI), (float)(M_PI_2));
		m_pModel->SetOrientationOffset(vLeftHandOffset);
		m_pPhantomModel->SetOrientationOffset(vLeftHandOffset);
	}
	
	if (m_handType == HAND_TYPE::HAND_RIGHT) {

		std::wstring wstrModel = wstrAssetPath + k_wstrFolder + std::to_wstring(m_avatarModelId) + L"/" + k_wstrRight + k_wstrFileType;
		//m_pModel = m_pHMDComposite->AddModel(wstrModel, ModelFactory::flags::FLIP_WINDING);
		m_pModel = AddModel(wstrModel, ModelFactory::flags::FLIP_WINDING);
		m_pPhantomModel = MakeModel(wstrModel, ModelFactory::flags::FLIP_WINDING);
		
		if (m_pDreamOS != nullptr) {
			m_pDreamOS->AddObject(m_pPhantomModel.get(), Sandbox::PipelineType::AUX);
		}

		vector vRightHandOffset = vector(0.0f, (float)(M_PI), (float)(-M_PI_2));
		m_pModel->SetOrientationOffset(vRightHandOffset);
		m_pPhantomModel->SetOrientationOffset(vRightHandOffset);
	}

	CN(m_pModel);
	CN(m_pPhantomModel);

	m_pModel->SetPosition(ptModel);
	m_pModel->SetScale(scaleModel);

	m_pModel->SetVisible(m_fTracked && m_modelState == ModelState::HAND);

	m_pPhantomModel->SetPosition(ptModel);
	m_pPhantomModel->SetScale(scaleModel);
	m_pPhantomModel->SetVisible(true);

	// TODO: this is bad
	modelColor = ((model*)(m_pModel.get()))->GetChildMesh(0)->GetDiffuseColor();
	((model*)(m_pModel.get()))->SetMaterialSpecularColor(modelColor, true);
	((model*)(m_pModel.get()))->SetMaterialShininess(4.0f, true);

	((model*)(m_pPhantomModel.get()))->SetMaterialSpecularColor(modelColor, true);
	((model*)(m_pPhantomModel.get()))->SetMaterialShininess(4.0f, true);

#else

	//m_pModel = AddComposite();
	//m_pModel->AddVolume(0.02f);
	m_pModel = AddModel(L"cube.obj");
	m_pModel->SetScale(0.02f);

	if (m_handType == HAND_TYPE::HAND_LEFT) {
		vector vLeftHandOffset = vector(0.0f, (float)(M_PI), (float)(M_PI_2));
		m_pModel->SetOrientationOffset(vLeftHandOffset);
	}
	
	if (m_handType == HAND_TYPE::HAND_RIGHT) {
		vector vRightHandOffset = vector(0.0f, (float)(M_PI), (float)(-M_PI_2));
		m_pModel->SetOrientationOffset(vRightHandOffset);
	}

#endif

	m_fLoadHandModel = false;

Error:
	return r;
}

std::shared_ptr<composite> hand::GetModel() {
	return m_pModel;
}

std::shared_ptr<composite> hand::GetPhantomModel() {
	return m_pPhantomModel;
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

	m_radius = 0.015f;

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

		m_distance = 0.2f;
		m_angle = -23.0f * (float)(M_PI) / 180.0f;

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

		m_distance = 0.2f;
		m_angle = -60.0f * (float)(M_PI) / 180.0f;

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

		m_distance = 0.0f;
		m_angle = 0.0f;

	} break;
	}

	m_headOffset = point(0.0f, m_distance * sin(m_angle), -m_distance * cos(m_angle));

	//m_pHead = m_pHMDComposite->AddSphere(m_radius, 20.0f, 20.0f);
	m_pHead = AddSphere(m_radius, 20.0f, 20.0f);
	m_pHead->SetVisible(false);
	m_pHead->SetPosition(m_headOffset);
	m_pHead->RotateXBy(m_angle);

	// the mallet head is used for the collision interactions with buttons
	pDreamOS->AddInteractionObject(m_pHead.get());

	// the hand is used for the intersection interactions like pointing
	pDreamOS->AddInteractionObject(this);


Error:
	return r;
}

RESULT hand::ShowMallet() {
	RESULT r = R_PASS;
	//TODO: Mallet animation
	m_pHead->SetVisible(true);
	
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pHead.get(), 
		color(1.0f, 1.0f, 1.0f, 1.0f), 
		0.1, 
		AnimationCurveType::LINEAR, 
		AnimationFlags()));

Error:
	return r;
}

RESULT hand::HideMallet() {
	RESULT r = R_PASS;
	m_pHead->SetVisible(false);

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pHead.get(), 
		color(1.0f, 1.0f, 1.0f, 0.0f), 
		0.1, 
		AnimationCurveType::LINEAR, 
		AnimationFlags()));

Error:
	return r;
}

float hand::GetMalletRadius() {
	return m_radius;
}

sphere* hand::GetMalletHead() {
	return m_pHead.get();
}

point hand::GetMalletOffset() {
	return m_headOffset;
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

	if (m_pPhantomModel != nullptr) {
		m_pPhantomModel->SetVisible(m_fTracked);
	}

	switch (m_modelState) {
	case ModelState::HAND: {
		if (m_pModel != nullptr) {
			m_pModel->SetVisible(m_fTracked);
		}
	} break;
	case ModelState::CONTROLLER: {
		m_pController->SetVisible(m_fTracked);
		m_pHead->SetVisible(m_fTracked);
		//m_pHMDComposite->SetVisible(m_fTracked, false);
	} break;
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

RESULT hand::SetTracked(bool fTracked) {
	m_fTracked = fTracked;

	if (!m_fTracked) {
		OnLostTrack();
	}

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
	if (m_pHead != nullptr) {
		m_pHead->SetVisible(m_fTracked);
	}

	//m_pPalm->SetVisible(m_fTracked);

	return R_PASS;
}

RESULT hand::SetHandModel(HAND_TYPE type) {
	RESULT r = R_PASS;

	CBR(type == HAND_TYPE::HAND_SKELETON, R_SKIPPED) 

	SetVisible(true, false);
	m_pModel->SetVisible(true);

Error:
	return r;
}

RESULT hand::SetHandState(const HandState& pHandState) {
	RESULT r = R_PASS;

	SetPosition(pHandState.ptPalm);

	m_handType = pHandState.handType;
	SetHandModel(pHandState.handType);

	m_fTracked = pHandState.fTracked;
	if (!m_fTracked) {
		OnLostTrack();
	}

	if (m_pModel != nullptr) {
		m_pModel->SetOrientation(pHandState.qOrientation);
	}

//Error:
	return r;
}

HandState hand::GetHandState() {
	HandState handState = {
		GetPosition(true),
		GetOrientation(true),
		m_handType,
		m_fTracked
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

	CR(HideMallet());
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



	// TODO: re-enable if overlays come back
	if (m_pOverlayQuad != nullptr) {
		m_pOverlayQuad->SetVisible(false);
	}

	if (m_fTracked) {
		CR(ShowMallet());
	}

	/*
	auto pMesh = m_pController->GetFirstChild<mesh>().get();
	CNR(pMesh, R_SKIPPED);
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
