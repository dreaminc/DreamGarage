#include "DreamUserApp.h"
#include "DreamOS.h"
#include "UI/UIKeyboard.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamControlView/DreamControlView.h"

#include "UI/UIMallet.h"

#include "Core/Utilities.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	

#include "Primitives/camera.h"	

texture *DreamUserObserver::GetOverlayTexture(HAND_TYPE type) {
	return nullptr;
}

DreamUserApp::DreamUserApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUserApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamUserApp::~DreamUserApp() {
	Shutdown();
}

DreamUserApp* DreamUserApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUserApp *pDreamApp = new DreamUserApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamUserApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	SetAppName("DreamUserApp");
	SetAppDescription("A Dream User App");

	GetComposite()->InitializeOBB();

	auto pDreamOS = GetDOS();

	pDreamOS->AddObjectToInteractionGraph(GetComposite());

	CR(pDreamOS->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_MENU, this));

	// Initialize mallets
	m_pLeftMallet = new UIMallet(pDreamOS);
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(pDreamOS);
	CN(m_pRightMallet);

	pDreamOS->AddInteractionObject(m_pLeftMallet->GetMalletHead());
	pDreamOS->AddInteractionObject(m_pRightMallet->GetMalletHead());

	m_pAppBasis = pDreamOS->MakeComposite();

	auto pHMD = GetDOS()->GetHMD();
	if (pHMD != nullptr) {
		auto deviceType = pHMD->GetDeviceType();
		switch (deviceType) {
		case HMDDeviceType::OCULUS: {
			//m_pTextureDefaultGaze = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"right-controller-overlay-test.png");
			//m_pTextureDefaultGaze = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"Controller-Overlay.png");
			m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"left-controller-overlay-inactive.png");
			m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"right-controller-overlay-inactive.png");
		} break;
		case HMDDeviceType::VIVE: {
			//m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-left-test.png");
			//m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-right-test.png");
			m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-left-inactive.png");
			m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-right-inactive.png");
		} break;
		case HMDDeviceType::META: {
			m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-left-inactive.png");
			m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-right-inactive.png");
		} break;
		}
		CN(m_pTextureDefaultGazeLeft);
		CN(m_pTextureDefaultGazeRight);

	}


	m_pMessageComposite = GetComposite()->MakeComposite();
	m_pMessageComposite->SetVisible(false, false);
	m_pMessageComposite->SetPosition(0.0f, 0.0f, -2.0f);
	pDreamOS->AddObjectToUIGraph(m_pMessageComposite.get());

	m_pMessageQuad = m_pMessageComposite->AddQuad(m_messageQuadWidth, m_messageQuadHeight, 1, 1, nullptr);
	m_pMessageQuad->SetVisible(true);
	m_pMessageQuad->RotateXByDeg(90);

	m_pMessageQuadBackground = m_pMessageComposite->AddQuad(m_messageQuadWidth * m_messageBackgroundWidth, m_messageQuadWidth * m_messageBackgroundHeight, 1, 1, nullptr);
	m_pMessageQuadBackground->SetPosition(point(0.0f, 0.0f, -0.001f));
	m_pMessageQuadBackground->SetDiffuseTexture(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"control-view-main-background.png"));
	m_pMessageQuadBackground->SetVisible(true);
	m_pMessageQuadBackground->RotateXByDeg(90);


	// user settings
	m_userSettings = new UserSettings();

	m_userSettings->m_aspectRatio = ((float)m_userSettings->m_pxWidth / (float)m_userSettings->m_pxHeight);
	m_userSettings->m_baseWidth = std::sqrt(((m_userSettings->m_aspectRatio * m_userSettings->m_aspectRatio) * (m_userSettings->m_diagonalSize * m_userSettings->m_diagonalSize)) / (1.0f + (m_userSettings->m_aspectRatio * m_userSettings->m_aspectRatio)));
	m_userSettings->m_baseHeight = std::sqrt((m_userSettings->m_diagonalSize * m_userSettings->m_diagonalSize) / (1.0f + (m_userSettings->m_aspectRatio * m_userSettings->m_aspectRatio)));

#ifndef _DEBUG
	// TODO: This should move into DreamBrowser as a singleton
	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());
#endif

	// this is the guess at where the UI could be, only call this here
	CR(ResetAppComposite());

Error:
	return r;
}

RESULT DreamUserApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;


//Error:
	return r;
}

RESULT DreamUserApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	if (m_pWebBrowserManager != nullptr) {
		//CR(m_pWebBrowserManager->Shutdown());
		m_pWebBrowserManager = nullptr;
	}

	if (m_pMessageComposite != nullptr) {
		m_pMessageComposite = nullptr;
	}
	if (m_pMessageQuad != nullptr) {
		m_pMessageQuad = nullptr;
	}
	if (m_pMessageQuadBackground != nullptr) {
		m_pMessageQuadBackground = nullptr;
	}
	if (m_pOrientationRay != nullptr) {
		m_pOrientationRay = nullptr;
	}

Error:
	return r;
}

RESULT DreamUserApp::ToggleUserModel() {
	RESULT r = R_PASS;

	CB(GetDOS()->GetSandboxConfiguration().f3rdPersonCamera);

	if (m_pUserModel != nullptr) {
		bool fUserModelVisible = m_pUserModel->IsVisible();

		m_pUserModel->SetVisible(!fUserModelVisible);
		m_pUserModel->GetMouth()->SetVisible(!fUserModelVisible);

		// Don't show the hands
		if (m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT) != nullptr) {
			CR(m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT)->SetVisible(false));
		}

		if (m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT) != nullptr) {
			CR(m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT)->SetVisible(false));
		}
	}

Error:
	return r;
}

RESULT DreamUserApp::Update(void *pContext) {
	RESULT r = R_PASS;

	quaternion qOrientation;

	// update user interaction ray
	auto pCameraNode = GetDOS()->GetCameraNode();
	CN(pCameraNode);

	if (m_pKeyboardHandle == nullptr) {
		auto keyUIDs = GetDOS()->GetAppUID("UIKeyboard");
		if (keyUIDs.size() == 1) {
			m_pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(GetDOS()->CaptureApp(keyUIDs[0], this));
		}
	}
	
	if (m_pOrientationRay == nullptr) {
		m_pOrientationRay = std::shared_ptr<DimRay>(GetDOS()->AddRay(point(0.0f, 0.0f, 0.0f), vector::kVector(-1.0f), 1.0f));
		CN(m_pOrientationRay);
		m_pOrientationRay->SetVisible(false);
	}
	else {
		m_pOrientationRay->SetPosition(pCameraNode->GetPosition());
		qOrientation = (pCameraNode->GetOrientation());
		qOrientation.Reverse();
		m_pOrientationRay->SetOrientation(qOrientation);
	}

	auto pHMD = GetDOS()->GetHMD();
	if (m_pLeftHand == nullptr && pHMD != nullptr) {
		if (GetDOS()->GetHMD()->GetSenseControllerObject(CONTROLLER_LEFT) != nullptr) {
			SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_LEFT));
		}
	}
	if (m_pRightHand == nullptr && pHMD != nullptr) {
		if (GetDOS()->GetHMD()->GetSenseControllerObject(CONTROLLER_RIGHT) != nullptr) {
			SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_RIGHT));
		}
	}

	CR(UpdateHand(HAND_TYPE::HAND_LEFT));
	CR(UpdateHand(HAND_TYPE::HAND_RIGHT));

	if (m_pUserModel != nullptr) {
		m_pUserModel->SetPosition(pCameraNode->GetPosition());
		m_pUserModel->SetOrientation(qOrientation);

		// This is for debug to work
		if (m_pUserModel->GetMouth() != nullptr) {
			m_pUserModel->SetMouthOrientation(qOrientation);
			m_pUserModel->SetMouthPosition(pCameraNode->GetPosition());

			// Local mouth scale
			float mouthScale = GetDOS()->GetDreamSoundSystem()->GetRunTimeCaptureAverage();
			mouthScale *= 10.0f;
			util::Clamp<float>(mouthScale, 0.0f, 1.0f);

			m_pUserModel->UpdateMouth(mouthScale);
			m_pUserModel->UpdateMouthPose();
		}
	}

	// checks for the first time the headset and hands are tracked together
	// this allows for settings to accurately be used to position the message quad
	// this isn't as necessary for the menu because you at least have to press the menu button to get there

	if (m_fShowLaunchQuad && m_fHeadsetAndHandsTracked) {

		m_pMessageQuad->SetDiffuseTexture(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, m_textureStringFromStartupMessage[m_currentLaunchMessage].c_str()));

		UpdateCompositeWithHands(0.0f);

		RotationMatrix matLook = RotationMatrix(GetComposite()->GetOrientation());
		vector vAppLook = matLook * vector(0.0f, 0.0f, -1.0f);
		vAppLook.Normalize();
		vector vAppLookXZ = vector(vAppLook.x(), 0.0f, vAppLook.z()).Normal();

		m_pMessageComposite->SetPosition(GetComposite()->GetPosition() + (vAppLookXZ*2.0f));
		m_pMessageComposite->SetOrientation(GetComposite()->GetOrientation());
		m_pMessageComposite->SetVisible(true, false);

		m_fShowLaunchQuad = false;
	}

	if (!m_fHeadsetAndHandsTracked) {
		if (m_pLeftHand != nullptr && m_pRightHand != nullptr && GetDOS()->GetHMD() != nullptr) {
			if (m_pLeftHand->IsTracked() && m_pRightHand->IsTracked() && GetDOS()->GetHMD()->IsHMDTracked()) {
				m_fHeadsetAndHandsTracked = true;
			}
		}
	}
	
	if (GetDOS()->GetSandboxConfiguration().f3rdPersonCamera && m_pUserModel == nullptr) {
		m_pUserModel = std::shared_ptr<user>(GetDOS()->AddUser());
		CN(m_pUserModel);

		CR(m_pUserModel->SetDreamOS(GetDOS()));
		CR(m_pUserModel->UpdateAvatarModelWithID(1));
		CR(m_pUserModel->SetVisible(true));
		if (m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT) != nullptr) {
			CR(m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT)->SetVisible(false));
		}

		if (m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT) != nullptr) {
			CR(m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT)->SetVisible(false));
		}
	}

Error:
	return r;
}

RESULT DreamUserApp::UpdateHand(HAND_TYPE type) {
	RESULT r = R_PASS;

	hand *pHand = nullptr;
	UIMallet *pMallet = nullptr;
	bool fRayHandCollision = false;
	RotationMatrix qOffset; 

	// define variables based on hand type
	if (type == HAND_TYPE::HAND_LEFT) {
		pHand = m_pLeftHand;
		pMallet = m_pLeftMallet;
		fRayHandCollision = m_fCollisionLeft;
	}
	else if (type == HAND_TYPE::HAND_RIGHT) {
		pHand = m_pRightHand;
		pMallet = m_pRightMallet;
		fRayHandCollision = m_fCollisionRight;
	}

	CNR(pHand, R_SKIPPED);

	CR(pHand->Update());

	// Update Mallet Position
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (pMallet != nullptr) {
		pMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * pMallet->GetHeadOffset()));
	}

	// Update Mallet Visibility
	if (!pHand->IsTracked() && pMallet->GetMalletHead()->IsVisible()) {
		pMallet->Hide();
	} 
	else if (pHand->IsTracked() && m_pEventApp != nullptr && !pMallet->GetMalletHead()->IsVisible()) {
		pMallet->Show();
	}

Error:
	return r;
}

RESULT DreamUserApp::SetHasOpenApp(bool fHasOpenApp) {
	RESULT r = R_PASS;

	//CBR(fHasOpenApp != m_fHasOpenApp, R_SKIPPED);
	if (!fHasOpenApp && m_pPreviousApp != nullptr) {
		m_fHasOpenApp = true;
		m_pEventApp = m_pPreviousApp;
		HandleUserObserverEvent(UserObserverEventType::BACK);
		m_pPreviousApp = nullptr;
	}
	else {
		m_fHasOpenApp = fHasOpenApp;
	}

	if (m_fHasOpenApp) {

		if (m_pLeftHand != nullptr) {
			m_pLeftMallet->Show();
			m_pLeftHand->SetModelState(hand::ModelState::CONTROLLER);
		}
		if (m_pRightHand != nullptr) {
			m_pRightMallet->Show();
			m_pRightHand->SetModelState(hand::ModelState::CONTROLLER);
		}
	}
	else {

		if (m_pLeftHand != nullptr) {
			m_pLeftHand->SetModelState(hand::ModelState::HAND);
			m_pLeftMallet->Hide();
		}
		if (m_pRightHand != nullptr) {
			m_pRightMallet->Hide();
			m_pRightHand->SetModelState(hand::ModelState::HAND);
		}
	}

	UpdateOverlayTextures();

//Error:
	return r;
}

RESULT DreamUserApp::SetEventApp(DreamUserObserver *pEventApp) {
	m_pEventApp = pEventApp;
	return R_PASS;
}

RESULT DreamUserApp::UpdateOverlayTexture(HAND_TYPE type) {
	RESULT r = R_PASS;

	hand *pHand = nullptr;
	texture *pDefaultTexture = nullptr;
	texture *pOverlayTexture = nullptr;

	if (type == HAND_TYPE::HAND_LEFT) {
		pHand = m_pLeftHand;
		pDefaultTexture = m_pTextureDefaultGazeLeft;
	}
	else if (type == HAND_TYPE::HAND_RIGHT) {
		pHand = m_pRightHand;
		pDefaultTexture = m_pTextureDefaultGazeRight;
	}

	CNR(pHand, R_SKIPPED);
	CNR(pDefaultTexture, R_SKIPPED);

	if (m_pEventApp != nullptr) {
		pOverlayTexture = m_pEventApp->GetOverlayTexture(type);
	}

	if (pOverlayTexture != nullptr) {
		CR(pHand->SetOverlayTexture(pOverlayTexture));
	}
	else {
		CR(pHand->SetOverlayTexture(pDefaultTexture));
	}

Error:
	return r;
}

RESULT DreamUserApp::UpdateOverlayTextures() {
	RESULT r = R_PASS;

	CR(UpdateOverlayTexture(HAND_TYPE::HAND_LEFT));
	CR(UpdateOverlayTexture(HAND_TYPE::HAND_RIGHT));

Error:
	return r;
}

RESULT DreamUserApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	switch (mEvent->m_eventType) {

	case (ELEMENT_INTERSECT_BEGAN): {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto pEventObj = mEvent->m_pEventObject;
		auto pInteractionObj = mEvent->m_pInteractionObject;

		volume* pLeftVolume = nullptr;
		volume* pRightVolume = nullptr;
		if (m_pLeftHand != nullptr) {
			pLeftVolume = m_pLeftHand->GetPhantomVolume().get();
		}
		if (m_pRightHand != nullptr) {
			pRightVolume = m_pRightHand->GetPhantomVolume().get();
		}

		if (pInteractionObj == m_pOrientationRay.get() && (pEventObj == pLeftVolume || pEventObj == pRightVolume)) {
			m_msGazeStart = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

			if (pEventObj == pLeftVolume) {
				m_fCollisionLeft = true;
			}
			else if (pEventObj == pRightVolume) {
				m_fCollisionRight = true;
			}
		}

	} break;
	case (ELEMENT_INTERSECT_ENDED): {

		auto pEventObj = mEvent->m_pEventObject;
		volume* pLeftVolume = nullptr;
		volume* pRightVolume = nullptr;
		if (m_pLeftHand != nullptr) {
			pLeftVolume = m_pLeftHand->GetPhantomVolume().get();
		}
		if (m_pRightHand != nullptr) {
			pRightVolume = m_pRightHand->GetPhantomVolume().get();
		}

		if (pEventObj == pLeftVolume || pEventObj == pRightVolume) {
			if (pEventObj == pLeftVolume) {
				m_fCollisionLeft = false;
			}
			else if (pEventObj == pRightVolume) {
				m_fCollisionRight = false;
			}
		}
		if (!m_fCollisionLeft && !m_fCollisionRight) {
			if (m_pLeftHand != nullptr) {
				m_pLeftHand->SetOverlayVisible(false);
			}
			if (m_pRightHand != nullptr) {
				m_pRightHand->SetOverlayVisible(false);
			}
			if (m_pEventApp == nullptr) {
				if (m_pLeftHand != nullptr) {
					m_pLeftHand->SetModelState(hand::ModelState::HAND);
				}
				if (m_pRightHand != nullptr) {
					m_pRightHand->SetModelState(hand::ModelState::HAND);
				}
			}
		}


	} break;
	case INTERACTION_EVENT_MENU: {
		HideMessageQuad();
	}
	}
//Error:
	return r;
}

RESULT DreamUserApp::GetAppBasisPosition(point& ptOrigin) {
	ptOrigin = m_pAppBasis->GetPosition();
	return R_PASS;
}

RESULT DreamUserApp::GetAppBasisOrientation(quaternion& qOrigin) {
	qOrigin = m_pAppBasis->GetOrientation();
	return R_PASS;
}

RESULT DreamUserApp::SetPreviousApp(DreamUserObserver* pObserver) {
	RESULT r = R_PASS;

	m_pPreviousApp = pObserver;

	return r;
}

RESULT DreamUserApp::SetHand(hand *pHand) {
	RESULT r = R_PASS;
	auto pDreamOS = GetDOS();
	HAND_TYPE type;
	hand *pOtherHand = nullptr;

	CNR(pHand, R_OBJECT_NOT_FOUND);

	type = pHand->GetHandState().handType;
	CBR(type == HAND_TYPE::HAND_LEFT || type == HAND_TYPE::HAND_RIGHT, R_SKIPPED);

	pDreamOS->AddObject(pHand);
	CR(pHand->InitializeWithContext(pDreamOS));

	if (type == HAND_TYPE::HAND_LEFT) {
		m_pLeftHand = pHand;
		m_pLeftHand->SetOverlayTexture(m_pTextureDefaultGazeLeft);	
	}
	else {
		m_pRightHand = pHand;
		m_pRightHand->SetOverlayTexture(m_pTextureDefaultGazeRight);
	}

	// if the second hand is created later on, make sure that the states match
	if (pHand == m_pLeftHand) {
		pOtherHand = m_pRightHand;
	}
	else {
		pOtherHand = m_pLeftHand;
	}
	if (pOtherHand != nullptr) {
		pHand->SetModelState(pOtherHand->GetModelState());
	}
	else {
		if (m_fHasOpenApp) {
			pHand->SetModelState(hand::ModelState::CONTROLLER);
		}
		else {
			pHand->SetModelState(hand::ModelState::HAND);
		}
	}

Error:
	return r;
}

RESULT DreamUserApp::ClearHands() {
	RESULT r = R_PASS;

	CNR(m_pLeftHand, R_SKIPPED);
	CNR(m_pRightHand, R_SKIPPED);

	GetDOS()->RemoveObject(m_pLeftHand);
	m_pLeftHand = nullptr;

	GetDOS()->RemoveObject(m_pRightHand);
	m_pRightHand = nullptr;

Error:
	return r;
}

UIMallet *DreamUserApp::GetMallet(HAND_TYPE type) {

	if (type == HAND_TYPE::HAND_LEFT) {
		return m_pLeftMallet;
	}
	else if (type == HAND_TYPE::HAND_RIGHT) {
		return m_pRightMallet;
	}

	return nullptr;
}

hand *DreamUserApp::GetHand(HAND_TYPE type) {

	if (type == HAND_TYPE::HAND_LEFT) {
		return m_pLeftHand;
	}
	else if (type == HAND_TYPE::HAND_RIGHT) {
		return m_pRightHand;
	}

	return nullptr;
}

RESULT DreamUserApp::CreateHapticImpulse(VirtualObj *pEventObj) {
	RESULT r = R_PASS;

	CNR(GetDOS()->GetHMD(), R_OBJECT_NOT_FOUND);

	if (pEventObj == m_pLeftMallet->GetMalletHead()) {
		CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
	}
	else if (pEventObj == m_pRightMallet->GetMalletHead()) {
		CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(1), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
	}

Error:
	return r;
}

RESULT DreamUserApp::SetAppCompositeOrientation(quaternion qOrientation) {
	RESULT r = R_PASS;

	m_pAppBasis->SetOrientation(qOrientation);
	GetComposite()->SetOrientation(m_pAppBasis->GetOrientation());

	return R_PASS;
}

RESULT DreamUserApp::SetAppCompositePosition(point ptPosition) {
	RESULT r = R_PASS;

	stereocamera *pCamera = GetDOS()->GetCamera();

	m_pAppBasis->SetPosition(ptPosition);
	GetComposite()->SetPosition(ptPosition);

	return R_PASS;
}

RESULT DreamUserApp::UpdateCompositeWithCameraLook(float depth, float yPos) {

	composite *pComposite = GetComposite();
	auto pCamera = pComposite->GetCamera();
	vector vLookXZ = pCamera->GetLookVectorXZ();
	point lookOffset = depth * vLookXZ + point(0.0f, yPos, 0.0f);

	m_pAppBasis->SetPosition(pCamera->GetPosition() + lookOffset);
	m_pAppBasis->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vLookXZ));

	return R_PASS;
}

RESULT DreamUserApp::UpdateCompositeWithHands(float yPos) {
	RESULT r = R_PASS;

	composite *pComposite = GetComposite();
	auto pCamera = pComposite->GetCamera();
	vector vCameraToMenu;
	vector vCameraToBrowser;
	vector vMenuToBrowser;
	point ptMid;
	vector vUp = vector(0.0f, 1.0f, 0.0f);

	CN(pCamera);
	{
		point ptCameraOrigin = pCamera->GetOrigin(true);

		//RotationMatrix matLook = RotationMatrix(m_pAppBasis->GetOrientation());
		RotationMatrix matLook = RotationMatrix(pCamera->GetWorldOrientation());
		vector vAppLook = matLook * vector(0.0f, 0.0f, -1.0f);
		vAppLook.Normalize();
		vector vAppLookXZ = vector(vAppLook.x(), 0.0f, vAppLook.z()).Normal();

		float menuDepth = vCameraToMenu.magnitude();

		m_pAppBasis->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vAppLookXZ));

		// min and max menu depths
		util::Clamp(menuDepth, MENU_DEPTH_MIN, MENU_DEPTH_MAX);

		point ptCamera = pCamera->GetPosition();

		//*
		if (m_pLeftMallet != nullptr && m_pRightMallet != nullptr &&
			m_pLeftHand != nullptr && m_pRightHand != nullptr) {

			vector vPos;

			for (auto& mallet : { m_pLeftMallet, m_pRightMallet }) {	// which hand is closer

				RotationMatrix qOffset;
				auto pHand = mallet == m_pLeftMallet ? m_pLeftHand : m_pRightHand;

				// at least one of two hands should be tracked, since this is 
				// called as a result of menu press
				if (pHand->IsTracked()) {
					qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

					//point ptHand = pHand->GetPosition(true) + point(qOffset * mallet->GetHeadOffset());

					point ptHand = mallet->GetMalletHead()->GetPosition(true);
					vector vHand = ptHand - pCamera->GetOrigin(true);
					vHand = vector(vHand.x(), 0.0f, vHand.z());
					//vector vTempPos = vAppLookXZ * (vHand.dot(vAppLookXZ));
					vector vTempPos = vAppLookXZ * vHand.magnitude();
					if (vTempPos.magnitudeSquared() > vPos.magnitudeSquared())
						vPos = vTempPos;
				}
			}
			point lookOffset = vPos + point(0.0f, yPos, 0.0f);
			m_pAppBasis->SetPosition(pCamera->GetPosition() + lookOffset);
		}
		
		//*/
		
		//m_pAppBasis->SetPosition(ptCameraOrigin + ptMenuPosition + point(0.0f, yPos, 0.0f));
		GetComposite()->SetPosition(m_pAppBasis->GetPosition());
		GetComposite()->SetOrientation(m_pAppBasis->GetOrientation());
	}

Error:
	return r;

}

RESULT DreamUserApp::HandleKBEnterEvent() {
	RESULT r = R_PASS;
	CNR(m_pEventApp, R_SKIPPED);
	m_pEventApp->HandleEvent(UserObserverEventType::KB_ENTER);
Error:
	return r;
}

RESULT DreamUserApp::HandleUserObserverEvent(UserObserverEventType type) {
	RESULT r = R_PASS;
	CNR(m_pEventApp, R_SKIPPED);
	m_pEventApp->HandleEvent(type);
Error:
	return r;
}

UIKeyboardHandle *DreamUserApp::GetKeyboard() {
	return m_pKeyboardHandle;
}

RESULT DreamUserApp::ReleaseKeyboard() {
	return R_PASS;
}

RESULT DreamUserApp::GetStreamingState(bool& fStreaming) {
	fStreaming = m_fStreaming;
	return R_PASS;
}

RESULT DreamUserApp::SetStreamingState(bool fStreaming) {
	m_fStreaming = fStreaming;
	return R_PASS;
}

RESULT DreamUserApp::PreserveSharingState(bool fIsSharing) {
	m_fIsSharing = fIsSharing;
	return R_PASS;
}

RESULT DreamUserApp::ResetAppComposite() {
	RESULT r = R_PASS;

	CR(UpdateCompositeWithHands(m_menuHeight));

Error:
	return r;
}

float DreamUserApp::GetPXWidth() {
	return m_userSettings->m_pxWidth;
}

float DreamUserApp::GetPXHeight() {
	return m_userSettings->m_pxHeight;
}

float DreamUserApp::GetBaseWidth() {
	return m_userSettings->m_baseWidth;
}

float DreamUserApp::GetBaseHeight() {
	return m_userSettings->m_baseHeight;
}

float DreamUserApp::GetViewAngle() {
	return m_userSettings->m_viewAngle;
}

float DreamUserApp::GetAnimationDuration() {
	return m_userSettings->m_animationDuration;
}

float DreamUserApp::GetSpacingSize() {
	return m_userSettings->m_spacingSize;
}

std::shared_ptr<CEFBrowserManager> DreamUserApp::GetBrowserManager() {
	RESULT r = R_PASS;

	if (m_pWebBrowserManager == nullptr) {
#ifndef _DEBUG
		m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
		CN(m_pWebBrowserManager);
		CR(m_pWebBrowserManager->Initialize());
#endif
	}

Error:
	return m_pWebBrowserManager;
}

RESULT DreamUserApp::SetStartupMessageType(StartupMessage messageType) {
	m_currentLaunchMessage = messageType;
	return R_PASS;
}

RESULT DreamUserApp::ShowMessageQuad() {
	RESULT r = R_PASS;

	m_fShowLaunchQuad = true;

Error:
	return r;
}

RESULT DreamUserApp::HideMessageQuad() {
	RESULT r = R_PASS;

	CR(m_pMessageComposite->SetVisible(false, false));

Error:
	return r;
}
