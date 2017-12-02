#include "DreamUserApp.h"
#include "DreamOS.h"
#include "UI/UIKeyboard.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamControlView.h"

#include "UI/UIMallet.h"

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

RESULT DreamUserHandle::SendPopFocusStack() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(PopFocusStack());
Error:
	return r;
}


RESULT DreamUserHandle::SendPushFocusStack(DreamUserObserver* pObserver) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(PushFocusStack(pObserver));
Error:
	return r;
}

RESULT DreamUserHandle::SendClearFocusStack() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ClearFocusStack());
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
		}
		CN(m_pTextureDefaultGazeLeft);
		CN(m_pTextureDefaultGazeRight);

	}

	{
		auto keyUIDs = pDreamOS->GetAppUID("UIKeyboard");
		CBR(keyUIDs.size() == 1, R_SKIPPED);
		m_pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(pDreamOS->CaptureApp(keyUIDs[0], this));

		CN(m_pKeyboardHandle);
	}


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

	CR(r);

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

	RotationMatrix qOffset; // mallet positioning

	// update user interaction ray
	auto pCameraNode = GetDOS()->GetCameraNode();
	CN(pCameraNode);

	GetComposite()->SetPosition(pCameraNode->GetPosition());

	quaternion qOrientation = (pCameraNode->GetOrientation());
	qOrientation.Reverse();
	GetComposite()->SetOrientation(qOrientation);

	/*
	if (m_pVolume == nullptr) {
		m_pVolume = GetComposite()->AddVolume(1.0f);
		CN(m_pVolume);
		m_pVolume->SetVisible(false);
	}
	//*/
	
	if (m_pOrientationRay == nullptr) {
		m_pOrientationRay = GetComposite()->AddRay(point(0.0f, 0.0f, 0.0f), vector::kVector(-1.0f), 1.0f);
		//m_pOrientationRay = GetComposite()->AddRay(point(0.0f, 0.0f, -0.75f), vector::kVector(-1.0f), 1.0f);
		CN(m_pOrientationRay);
		m_pOrientationRay->SetVisible(false);
		CR(GetDOS()->AddInteractionObject(m_pOrientationRay.get()));
	}

	CBR(m_pLeftHand, R_SKIPPED);

	// Update Mallet Positions
	CNR(m_pLeftHand, R_SKIPPED);
	CNR(m_pRightHand, R_SKIPPED);

	qOffset.SetQuaternionRotationMatrix(m_pLeftHand->GetOrientation());

	if (m_pLeftMallet)
		m_pLeftMallet->GetMalletHead()->MoveTo(m_pLeftHand->GetPosition() + point(qOffset * m_pLeftMallet->GetHeadOffset()));

	CBR(m_pRightHand, R_SKIPPED);
	qOffset = RotationMatrix();
	qOffset.SetQuaternionRotationMatrix(m_pRightHand->GetOrientation());

	if (m_pRightMallet)
		m_pRightMallet->GetMalletHead()->MoveTo(m_pRightHand->GetPosition() + point(qOffset * m_pRightMallet->GetHeadOffset()));

	if (m_fCollisionLeft || m_fCollisionRight) {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

		if (msNow - m_msGazeStart > m_msGazeOverlayDelay) {

			m_pLeftHand->SetOverlayVisible(true);
			m_pRightHand->SetOverlayVisible(true);


			if (m_appStack.empty()) {
				m_pLeftHand->SetOverlayTexture(m_pTextureDefaultGazeLeft);
				m_pRightHand->SetOverlayTexture(m_pTextureDefaultGazeRight);
				m_pLeftHand->SetModelState(hand::ModelState::CONTROLLER);
				m_pRightHand->SetModelState(hand::ModelState::CONTROLLER);
			}
			else {
				//*
				auto pTexture = m_appStack.top()->GetOverlayTexture(HAND_TYPE::HAND_RIGHT);
				if (pTexture == nullptr) {
					m_pRightHand->SetOverlayTexture(m_pTextureDefaultGazeRight);
				}
				else {
					m_pRightHand->SetOverlayTexture(pTexture);
				}

				pTexture = m_appStack.top()->GetOverlayTexture(HAND_TYPE::HAND_LEFT);
				if (pTexture == nullptr) {
					m_pLeftHand->SetOverlayTexture(m_pTextureDefaultGazeLeft);
				}
				else {
					m_pLeftHand->SetOverlayTexture(pTexture);
				}
				//*/
			}
		}
	}

	CR(UpdateHands());

Error:
	return r;
}

RESULT DreamUserApp::UpdateHands() {
	RESULT r = R_PASS;

	CR(m_pLeftHand->Update());
	if (!m_pLeftHand->IsTracked() && m_pLeftMallet->GetMalletHead()->IsVisible()) {
		m_pLeftMallet->Hide();
	} 
	else if (m_pLeftHand->IsTracked() && !m_appStack.empty() && !m_pLeftMallet->GetMalletHead()->IsVisible()) {
		m_pLeftMallet->Show();
	}

	CR(m_pRightHand->Update());
	if (!m_pRightHand->IsTracked() && m_pRightMallet->GetMalletHead()->IsVisible()) {
		m_pRightMallet->Hide();
	} 
	else if (m_pRightHand->IsTracked() && !m_appStack.empty() && !m_pRightMallet->GetMalletHead()->IsVisible()) {
		m_pRightMallet->Show();
	}

Error:
	return r;
}

RESULT DreamUserApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	switch (mEvent->m_eventType) {

	case INTERACTION_EVENT_MENU: {

		// get app ids
		auto pDreamOS = GetDOS();

		auto menuUIDs = pDreamOS->GetAppUID("DreamUIBar");
		auto keyUIDs = pDreamOS->GetAppUID("UIKeyboard");
		//auto browserUIDs = pDreamOS->GetAppUID("DreamBrowser");
		auto controlUIDs = pDreamOS->GetAppUID("DreamControlView");

		//TODO: requesting the handles may need to be moved into the switch statements,
		//		depending on how other applications handle capturing each other


		if (m_appStack.empty()) {
			CB(menuUIDs.size() == 1);
			auto pMenuHandle = dynamic_cast<DreamUIBarHandle*>(pDreamOS->CaptureApp(menuUIDs[0], this));

			CB(controlUIDs.size() == 1);
			auto pControlHandle = dynamic_cast<DreamControlViewHandle*>(pDreamOS->CaptureApp(controlUIDs[0], this));

			CN(pMenuHandle);
			CN(pControlHandle);

			if (pMenuHandle != nullptr) {
				ResetAppComposite();
				pMenuHandle->SendShowRootMenu();
			}

			m_pLeftMallet->Show();
			m_pRightMallet->Show();

			m_pLeftHand->SetModelState(hand::ModelState::CONTROLLER);
			m_pRightHand->SetModelState(hand::ModelState::CONTROLLER);

			m_appStack.push(pMenuHandle);
			GetDOS()->ReleaseApp(pControlHandle, controlUIDs[0], this);
			GetDOS()->ReleaseApp(pMenuHandle, menuUIDs[0], this);
		}
		else {
			m_appStack.top()->HandleEvent(UserObserverEventType::BACK);
		}

	} break;

	case (ELEMENT_INTERSECT_BEGAN): {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto pEventObj = mEvent->m_pEventObject;

		auto pLeftVolume = m_pLeftHand->GetPhantomVolume().get();
		auto pRightVolume = m_pRightHand->GetPhantomVolume().get();

		if (pEventObj == pLeftVolume || pEventObj == pRightVolume) {
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
		auto pLeftVolume = m_pLeftHand->GetPhantomVolume().get();
		auto pRightVolume = m_pRightHand->GetPhantomVolume().get();
		if (pEventObj == pLeftVolume || pEventObj == pRightVolume) {
			if (pEventObj == pLeftVolume) {
				m_fCollisionLeft = false;
			}
			else if (pEventObj == pRightVolume) {
				m_fCollisionRight = false;
			}
		}
		if (!m_fCollisionLeft && !m_fCollisionRight) {
			m_pLeftHand->SetOverlayVisible(false);
			m_pRightHand->SetOverlayVisible(false);
			if (m_appStack.empty() && !m_fStreaming) {
				m_pLeftHand->SetModelState(hand::ModelState::HAND);
				m_pRightHand->SetModelState(hand::ModelState::HAND);
			}
		}


	} break;
	}
Error:
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

RESULT DreamUserApp::PopFocusStack() {
	RESULT r = R_PASS;

	m_appStack.pop();
	if (m_appStack.empty()) {
		CR(OnFocusStackEmpty());
	}

Error:
	return r;
}

RESULT DreamUserApp::PushFocusStack(DreamUserObserver *pObserver) {
	RESULT r = R_PASS;

	m_appStack.push(pObserver);

//Error:
	return r;
}

RESULT DreamUserApp::ClearFocusStack() {
	RESULT r = R_PASS;

	CBR(!m_appStack.empty(), R_SKIPPED);
	m_appStack = std::stack<DreamUserObserver*>();

	CR(OnFocusStackEmpty());

Error:
	return r;
}

RESULT DreamUserApp::OnFocusStackEmpty() {
	RESULT r = R_PASS;

	if (!m_fStreaming) {
		CR(m_pLeftMallet->Hide());
		CR(m_pRightMallet->Hide());

		if (!(m_fCollisionLeft || m_fCollisionRight)) {
			CR(m_pLeftHand->SetModelState(hand::ModelState::HAND));
			CR(m_pRightHand->SetModelState(hand::ModelState::HAND));
		}
	}
	else {
		// show app that wasn't dismissed...
	}

Error:
	return r;
}

RESULT DreamUserApp::SetHand(hand *pHand) {
	RESULT r = R_PASS;
	auto pDreamOS = GetDOS();
	HAND_TYPE type;

	CNR(pHand, R_OBJECT_NOT_FOUND);

	type = pHand->GetHandState().handType;
	CBR(type == HAND_TYPE::HAND_LEFT || type == HAND_TYPE::HAND_RIGHT, R_SKIPPED);

	if (type == HAND_TYPE::HAND_LEFT) {
		m_pLeftHand = pHand;
		GetDOS()->AddObject(m_pLeftHand);
		CR(m_pLeftHand->InitializeWithContext(pDreamOS));
		CR(m_pLeftHand->SetModelState(hand::ModelState::HAND));
		m_pLeftHand->SetOverlayTexture(m_pTextureDefaultGazeLeft);	
	}
	else {
		m_pRightHand = pHand;
		GetDOS()->AddObject(m_pRightHand);
		CR(m_pRightHand->InitializeWithContext(pDreamOS));
		CR(m_pRightHand->SetModelState(hand::ModelState::HAND));
		m_pRightHand->SetOverlayTexture(m_pTextureDefaultGazeRight);
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
	CN(m_pLeftHand);
	CN(m_pRightHand);

	{
		point ptCameraOrigin = pCamera->GetOrigin(true);
		point ptBrowserOrigin = point(0.0f, 2.0f, -2.0f);

		ptMid = (m_pLeftHand->GetPosition(true) + m_pRightHand->GetPosition(true)) / 2;
		vCameraToMenu = ptMid - ptCameraOrigin;	

		vCameraToBrowser = ptBrowserOrigin - ptCameraOrigin;

		float depth = vCameraToMenu.magnitude();
		
		if (depth < 0.3f) {
			depth = 0.3f;
		}
		
		// Reposition Menu to be on the vector between Camera and Browser
		point newpoint = depth * vCameraToBrowser.Normal();
		vCameraToMenu = (ptCameraOrigin + newpoint) - ptCameraOrigin;	

		point ptCamera = pCamera->GetPosition();

		vector vPos;
		for (auto& hand : { m_pLeftHand, m_pRightHand }) {	// which hand is closer
			point ptHand = hand->GetPosition(true);
			vector vHand = ptHand - pCamera->GetOrigin(true);
			vector vTempPos = vCameraToMenu * (vHand.dot(vCameraToMenu));
			if (vTempPos.magnitudeSquared() > vPos.magnitudeSquared())
				vPos = vTempPos;
		}

		point lookOffset = vPos + point(0.0f, yPos, 0.0f);

		//m_pAppBasis->SetPosition(pCamera->GetPosition() + lookOffset);
		//sphere* abposition = GetDOS()->AddSphere(.05f);
		//abposition->SetPosition(ptCameraOrigin + newpoint + point(0.0f, yPos, 0.0f));
		m_pAppBasis->SetPosition(ptCameraOrigin + newpoint + point(0.0f, yPos, 0.0f));
		m_pAppBasis->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vCameraToBrowser));
	}

Error:
	return r;

}

RESULT DreamUserApp::HandleKBEnterEvent() {
	RESULT r = R_PASS;
	CBR(!m_appStack.empty(), R_SKIPPED);
	CR(m_appStack.top()->HandleEvent(UserObserverEventType::KB_ENTER));
Error:
	return r;
}

RESULT DreamUserApp::HandleUserObserverEvent(UserObserverEventType type) {
	RESULT r = R_PASS;
	CBR(!m_appStack.empty(), R_SKIPPED);
	CR(m_appStack.top()->HandleEvent(type));
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

RESULT DreamUserApp::ResetAppComposite() {
	RESULT r = R_PASS;

	CR(UpdateCompositeWithHands(m_menuHeight));
	CNR(m_pKeyboardHandle, R_SKIPPED);
	CR(m_pKeyboardHandle->SendUpdateComposite(m_menuDepth, m_pAppBasis->GetPosition(), m_pAppBasis->GetOrientation()));

Error:
	return r;
}