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

UIMallet *DreamUserHandle::RequestMallet(HAND_TYPE type) {
	RESULT r = R_PASS;

	CB(GetAppState());

	return GetMallet(type);
Error:
	return nullptr;
}

RESULT DreamUserHandle::RequestHapticImpulse(VirtualObj *pEventObj) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(CreateHapticImpulse(pEventObj));
Error:
	return r;
}

RESULT DreamUserHandle::RequestAppBasisPosition(point& ptOrigin) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(GetAppBasisPosition(ptOrigin));
Error:
	return r;
}

RESULT DreamUserHandle::RequestAppBasisOrientation(quaternion& qOrigin) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(GetAppBasisOrientation(qOrigin));
Error:
	return r;
}

RESULT DreamUserHandle::SendSetPreviousApp(DreamUserObserver* pObserver) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetPreviousApp(pObserver));
Error:
	return r;
}

RESULT DreamUserHandle::SendStopSharing() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(StopSharing());
Error:
	return r;
}

RESULT DreamUserHandle::SendKBEnterEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(HandleKBEnterEvent());
Error:
	return r;
}

RESULT DreamUserHandle::SendUserObserverEvent(UserObserverEventType type) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(HandleUserObserverEvent(type));
Error:
	return r;
}

UIKeyboardHandle* DreamUserHandle::RequestKeyboard() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return GetKeyboard();
Error:
	return nullptr;
}

RESULT DreamUserHandle::SendReleaseKeyboard() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ReleaseKeyboard());
Error:
	return r;
}

RESULT DreamUserHandle::RequestStreamingState(bool& fStreaming) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(GetStreamingState(fStreaming));
Error:
	return r;
}

RESULT DreamUserHandle::SendStreamingState(bool fStreaming) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetStreamingState(fStreaming));
Error:
	return r;
}

RESULT DreamUserHandle::SendPreserveSharingState(bool fIsSharing) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(PreserveSharingState(fIsSharing));
Error:
	return r;
}

RESULT DreamUserHandle::RequestResetAppComposite() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ResetAppComposite());
Error:
	return r;
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

	//CR(pDreamOS->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_BEGAN, this));
	//CR(pDreamOS->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_MOVED, this));
	//CR(pDreamOS->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_ENDED, this));

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
			//m_pTextureDefaultGaze = GetDOS()->MakeTexture(L"right-controller-overlay-test.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			//m_pTextureDefaultGaze = GetDOS()->MakeTexture(L"Controller-Overlay.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(L"left-controller-overlay-inactive.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(L"right-controller-overlay-inactive.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		case HMDDeviceType::VIVE: {
			//m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(L"vive-controller-overlay-left-test.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			//m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(L"vive-controller-overlay-right-test.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(L"vive-controller-overlay-left-inactive.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(L"vive-controller-overlay-right-inactive.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		case HMDDeviceType::META: {
			m_pTextureDefaultGazeLeft = GetDOS()->MakeTexture(L"vive-controller-overlay-left-inactive.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pTextureDefaultGazeRight = GetDOS()->MakeTexture(L"vive-controller-overlay-right-inactive.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		}
		CN(m_pTextureDefaultGazeLeft);
		CN(m_pTextureDefaultGazeRight);

	}

	// user settings
	m_userSettings = new UserSettings();

	m_userSettings->m_aspectRatio = ((float)m_userSettings->m_pxWidth / (float)m_userSettings->m_pxHeight);
	m_userSettings->m_baseWidth = std::sqrt(((m_userSettings->m_aspectRatio * m_userSettings->m_aspectRatio) * (m_userSettings->m_diagonalSize * m_userSettings->m_diagonalSize)) / (1.0f + (m_userSettings->m_aspectRatio * m_userSettings->m_aspectRatio)));
	m_userSettings->m_baseHeight = std::sqrt((m_userSettings->m_diagonalSize * m_userSettings->m_diagonalSize) / (1.0f + (m_userSettings->m_aspectRatio * m_userSettings->m_aspectRatio)));

	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

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

	CR(m_pWebBrowserManager->Shutdown());
	m_pWebBrowserManager = nullptr;

Error:
	return r;
}

DreamAppHandle* DreamUserApp::GetAppHandle() {
	return (DreamUserHandle*)(this);
}

unsigned int DreamUserApp::GetHandleLimit() {
	return -1;
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

		//CN(m_pKeyboardHandle);
	}
	
	/*
	GetComposite()->SetPosition(pCameraNode->GetPosition());

	qOrientation = (pCameraNode->GetOrientation());
	qOrientation.Reverse();
	GetComposite()->SetOrientation(qOrientation);
	//*/

	if (m_pOrientationRay == nullptr) {
		m_pOrientationRay = std::shared_ptr<DimRay>(GetDOS()->AddRay(point(0.0f, 0.0f, 0.0f), vector::kVector(-1.0f), 1.0f));
		//m_pOrientationRay = GetComposite()->AddRay(point(0.0f, 0.0f, -0.75f), vector::kVector(-1.0f), 1.0f);
		CN(m_pOrientationRay);
		m_pOrientationRay->SetVisible(false);
		CR(GetDOS()->AddInteractionObject(m_pOrientationRay.get()));
	}
	m_pOrientationRay->SetPosition(pCameraNode->GetPosition());

	qOrientation = (pCameraNode->GetOrientation());
	qOrientation.Reverse();
	m_pOrientationRay->SetOrientation(qOrientation);

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

	// Update Overlay visibility / texture
	if (fRayHandCollision) {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

		if (msNow - m_msGazeStart > m_msGazeOverlayDelay) {
			bool fMeta = false;
			auto pHMD = GetDOS()->GetHMD();
			if (pHMD != nullptr) {
				fMeta = pHMD->GetDeviceType() == HMDDeviceType::META;
			}
			//pHand->SetOverlayVisible(true);
			if (m_pLeftHand != nullptr && !fMeta) {
				m_pLeftHand->SetOverlayVisible(true);
			}
			if (m_pRightHand != nullptr && !fMeta) {
				m_pRightHand->SetOverlayVisible(true);
			}

			//UpdateOverlayTexture(type);
			UpdateOverlayTextures();
			if (m_pEventApp == nullptr) {
				//pHand->SetModelState(hand::ModelState::CONTROLLER);
				if (m_pLeftHand != nullptr) {
					m_pLeftHand->SetModelState(hand::ModelState::CONTROLLER);
				}
				if (m_pRightHand != nullptr) {
					m_pRightHand->SetModelState(hand::ModelState::CONTROLLER);
				}
			}
		}
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
		//HandleUserObserverEvent(UserObserverEventType::BACK);
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
			if (m_pEventApp == nullptr && !m_fStreaming) {
				if (m_pLeftHand != nullptr) {
					m_pLeftHand->SetModelState(hand::ModelState::HAND);
				}
				if (m_pRightHand != nullptr) {
					m_pRightHand->SetModelState(hand::ModelState::HAND);
				}
			}
		}


	} break;
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

RESULT DreamUserApp::StopSharing() {
	RESULT r = R_PASS;

	return r;
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
		pHand->SetOverlayVisible(pOtherHand->IsOverlayVisible());
		pHand->SetModelState(pOtherHand->GetModelState());
	}
	else {
		pHand->SetOverlayVisible(false);
		pHand->SetModelState(hand::ModelState::HAND);
	}

	auto pVolume = pHand->GetPhantomVolume().get();
	CNR(pVolume, R_SKIPPED);
	pDreamOS->AddObjectToInteractionGraph(pVolume);

	CR(pDreamOS->RegisterEventSubscriber(pVolume, ELEMENT_INTERSECT_BEGAN, this));
	CR(pDreamOS->RegisterEventSubscriber(pVolume, ELEMENT_INTERSECT_MOVED, this));
	CR(pDreamOS->RegisterEventSubscriber(pVolume, ELEMENT_INTERSECT_ENDED, this));

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

RESULT DreamUserApp::UpdateCompositeWithCameraLook(float depth, float yPos) {

	composite *pComposite = GetComposite();
	auto pCamera = pComposite->GetCamera();
	vector vLookXZ = GetCameraLookXZ();
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
		point ptBrowserOrigin = point(0.0f, 2.0f, -2.0f);

		//ptMid = (m_pLeftHand->GetPosition(true) + m_pRightHand->GetPosition(true)) / 2;
		ptMid = point::midpoint(m_pLeftMallet->GetMalletHead()->GetPosition(true), m_pRightMallet->GetMalletHead()->GetPosition(true));
		vCameraToMenu = ptMid - ptCameraOrigin;	

		vCameraToBrowser = ptBrowserOrigin - ptCameraOrigin;
		vCameraToBrowser.y() = 0;

		float menuDepth = vCameraToMenu.magnitude();
		
		// min and max menu depths
		util::Clamp(menuDepth, MENU_DEPTH_MIN, MENU_DEPTH_MAX);
		
		// Reposition Menu to be on the vector between Camera and Browser
		point ptMenuPosition = menuDepth * vCameraToBrowser.Normal();
		vCameraToMenu = (ptCameraOrigin + ptMenuPosition) - ptCameraOrigin;	
		vCameraToMenu.y() = 0.0f;
		point ptCamera = pCamera->GetPosition();

		/*
		vector vPos;
		for (auto& hand : { m_pLeftHand, m_pRightHand }) {	// which hand is closer
			point ptHand = hand->GetPosition(true);
			vector vHand = ptHand - pCamera->GetOrigin(true);
			vector vTempPos = vCameraToMenu * (vHand.dot(vCameraToMenu));
			if (vTempPos.magnitudeSquared() > vPos.magnitudeSquared())
				vPos = vTempPos;
		} 
		
		point lookOffset = vPos + point(0.0f, yPos, 0.0f);
		m_pAppBasis->SetPosition(pCamera->GetPosition() + lookOffset);
		*/
		
		m_pAppBasis->SetPosition(ptCameraOrigin + ptMenuPosition + point(0.0f, yPos, 0.0f));
		m_pAppBasis->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vCameraToMenu));
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

float DreamUserApp::GetHeight() {
	return m_userSettings->m_height;
}

float DreamUserApp::GetDepth() {
	return m_userSettings->m_depth;
}

float DreamUserApp::GetScale() {
	return m_userSettings->m_scale;
}

float DreamUserApp::GetSpacingSize() {
	return m_userSettings->m_spacingSize;
}

RESULT DreamUserApp::SetScale(float widthScale) {
	m_userSettings->m_scale = widthScale;
	return R_PASS;
}

std::shared_ptr<CEFBrowserManager> DreamUserApp::GetBrowserManager() {
	RESULT r = R_PASS;

	if (m_pWebBrowserManager == nullptr) {
		m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
		CN(m_pWebBrowserManager);
		CR(m_pWebBrowserManager->Initialize());
	}

Error:
	return m_pWebBrowserManager;
}

RESULT DreamUserApp::UpdateHeight(float heightDiff) {

	point ptComposite = GetComposite()->GetPosition();
	ptComposite += point(0.0f, heightDiff, 0.0f);

	m_userSettings->m_height += heightDiff;

	GetComposite()->SetPosition(ptComposite);

	return R_PASS;
}

RESULT DreamUserApp::UpdateDepth(float depthDiff) {

	stereocamera *pCamera = GetDOS()->GetCamera();

	RotationMatrix matLook = RotationMatrix(m_pAppBasis->GetOrientation());
	vector vAppLook = matLook * vector(0.0f, 0.0f, -1.0f);
	vAppLook.Normalize();
	vector vAppLookXZ = vector(vAppLook.x(), 0.0f, vAppLook.z()).Normal();
	vector vDiff = depthDiff * vAppLookXZ;

	m_userSettings->m_depth += depthDiff;

	point ptCamera = pCamera->GetEyePosition(EYE_MONO);

	pCamera->SetHMDAdjustedPosition(ptCamera + vDiff);

	return R_PASS;
}
RESULT DreamUserApp::UpdateScale(float scale) {

	m_userSettings->m_scale = scale;
	GetComposite()->SetScale(scale);

	return R_PASS;
}
