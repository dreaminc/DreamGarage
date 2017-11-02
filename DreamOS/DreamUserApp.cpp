#include "DreamUserApp.h"
#include "DreamOS.h"
#include "UI/UIKeyboard.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamControlView.h"

#include "UI/UIMallet.h"

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

	{
		auto keyUIDs = pDreamOS->GetAppUID("UIKeyboard");
		CB(keyUIDs.size() == 1);
		m_pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(pDreamOS->CaptureApp(keyUIDs[0], this));

		CN(m_pKeyboardHandle);
	}

	m_pTextureDefaultGaze = GetDOS()->MakeTexture(L"Controller-Overlay.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

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

	if (m_fGazeInteraction) {
		auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();

		if (msNow - m_msGazeStart > m_msGazeOverlayDelay) {
			m_pQuadOverlayRight->SetDiffuseTexture(m_pTextureDefaultGaze);
			m_pQuadOverlayRight->SetVisible(true);

			if (m_appStack.empty()) {
				m_pLeftHand->SetVisible(false);
				m_pRightHand->SetVisible(false);

				m_pLeftController->SetVisible(true);
				m_pRightController->SetVisible(true);
			}
		}
	}

	CR(UpdateHands());
/*
	if (m_pLeftHand->IsTracked() && !m_fGazeInteraction) {
		//m_pLeftController->SetVisible(true);
		m_pLeftHand->SetVisible(true);
	}
	else if (!m_pLeftHand->IsTracked()) {
		//m_pLeftController->SetVisible(false);
		m_pLeftHand->SetVisible(false);
	}

	if (m_pRightHand->IsTracked() && !m_fGazeInteraction) {
		//m_pRightController->SetVisible(true);
		m_pRightHand->SetVisible(true);
		//m_pQuadOverlayRight->SetVisible(false);
	}
	else if (!m_pRightHand->IsTracked()) {
		//m_pRightController->SetVisible(false);
		m_pRightHand->SetVisible(false);
	}
	//*/

Error:
	return r;
}

RESULT DreamUserApp::UpdateHands() {
	RESULT r = R_PASS;
/*
	for (int i = 0; i < 2; i++) {
		auto pHand = i == 0 ? m_pLeftHand : m_pRightHand;
		auto pController = i == 0 ? m_pLeftController : m_pRightController;

		auto pObject = m_appStack.empty() ? pHand : pController;

		pObject->SetVisible(pHand->IsTracked());
	}
	//*/

	for (int i = 0; i < 2; i++) {
		auto pHand = i == 0 ? m_pLeftHand : m_pRightHand;
		auto pController = i == 0 ? m_pLeftController : m_pRightController;

		if (m_appStack.empty() && !m_fGazeInteraction) {
			//if (pHand->IsTracked()) {
			//	pHand->SetVisible(true);
			//}
			pHand->SetVisible(pHand->IsTracked());
		}
		else {
			pController->GetFirstChild<mesh>()->SetVisible(pHand->IsTracked());
			//m_pQuadOverlayRight->SetVisible(m_fGazeCurrent);
		}
	}

	return R_PASS;
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

		CB(menuUIDs.size() == 1);
		auto pMenuHandle = dynamic_cast<DreamUIBarHandle*>(pDreamOS->CaptureApp(menuUIDs[0], this));

		CB(controlUIDs.size() == 1);
		auto pControlHandle = dynamic_cast<DreamControlViewHandle*>(pDreamOS->CaptureApp(controlUIDs[0], this));

		if (m_appStack.empty()) {

			if (pMenuHandle != nullptr) {
				UpdateCompositeWithHands(m_menuHeight);
				m_pKeyboardHandle->SendUpdateComposite(m_menuDepth, m_pAppBasis->GetPosition(), m_pAppBasis->GetOrientation());
				pMenuHandle->SendShowRootMenu();
			}

			m_pLeftMallet->Show();
			m_pRightMallet->Show();

			m_pLeftHand->SetVisible(false);
			m_pRightHand->SetVisible(false);

			m_pLeftController->SetVisible(true);
			m_pRightController->SetVisible(true);

			if (!m_fGazeInteraction) {
				m_pQuadOverlayRight->SetVisible(false);
			}

			m_appStack.push(pMenuHandle);
		}

		m_appStack.top()->HandleEvent(UserObserverEventType::BACK);

		GetDOS()->ReleaseApp(pControlHandle, controlUIDs[0], this);
		GetDOS()->ReleaseApp(pMenuHandle, menuUIDs[0], this);
	} break;

	case (ELEMENT_INTERSECT_BEGAN): {
		//if (m_pInteractionObj == m_pLeftController || m_pInteractionObj == m_pRightController) {
			auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
			m_msGazeStart = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
			m_fGazeInteraction = true;
			m_pInteractionObj = mEvent->m_pEventObject;
			//m_pInteractionObj = mEvent->m_pInteractionObject;
		//}
	} break;
	case (ELEMENT_INTERSECT_ENDED): {
		m_fGazeInteraction = false;
	//	auto dObj = dynamic_cast<DimObj*>(m_pInteractionObj);
	//	dObj->SetVisible(true);
	//	m_pLeftController->SetVisible(true);
		m_pQuadOverlayRight->SetVisible(false);

		if (m_appStack.empty()) {
			m_pLeftHand->SetVisible(true);
			m_pRightHand->SetVisible(true);

			m_pLeftController->SetVisible(false);
			m_pRightController->SetVisible(false);
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
		m_pLeftMallet->Hide();
		m_pRightMallet->Hide();

		m_pLeftHand->SetVisible(true);
		m_pRightHand->SetVisible(true);

		m_pLeftController->SetVisible(false);
		m_pRightController->SetVisible(false);
	}

//Error:
	return r;
}

RESULT DreamUserApp::PushFocusStack(DreamUserObserver *pObserver) {
	RESULT r = R_PASS;

	m_appStack.push(pObserver);

//Error:
	return r;
}

RESULT DreamUserApp::ClearFocusStack() {
	m_appStack = std::stack<DreamUserObserver*>();

	m_pLeftMallet->Hide();
	m_pRightMallet->Hide();

	return R_PASS;
}

RESULT DreamUserApp::SetHand(hand *pHand) {
	RESULT r = R_PASS;
	auto pDreamOS = GetDOS();
	HAND_TYPE type;

	CNR(pHand, R_OBJECT_NOT_FOUND);

	type = pHand->GetHandState().handType;
	CBR(type == HAND_TYPE::HAND_LEFT || type == HAND_TYPE::HAND_RIGHT, R_SKIPPED);

	auto pHMD = pDreamOS->GetHMD();
	CNR(pHMD, R_SKIPPED);

	auto controllerType = type == HAND_TYPE::HAND_LEFT ? CONTROLLER_TYPE::CONTROLLER_LEFT : CONTROLLER_TYPE::CONTROLLER_RIGHT;
	auto pController = pHMD->GetSenseControllerObject(controllerType);
	CNR(pController, R_SKIPPED);
	//pDreamOS->AddObjectToInteractionGraph(pController);

	if (type == HAND_TYPE::HAND_LEFT) {
		m_pLeftHand = pHand;
		GetDOS()->AddObject(m_pLeftHand);
		m_pLeftController = pController;
		//m_pLeftPhantomVolume = m_pLeftHand->AddVolume(0.25f);
		//m_pLeftPhantomVolume->SetVisible(false);
	}
	else {
		m_pRightHand = pHand;
		GetDOS()->AddObject(m_pRightHand);
		m_pRightController = pController;

		//m_pRightPhantomVolume = m_pLeftHand->AddVolume(0.25f);
		//m_pRightPhantomVolume->SetVisible(false);

		m_pQuadOverlayRight = m_pRightController->MakeQuad(0.057f, 0.057f);
		GetDOS()->AddObjectToUIGraph(m_pQuadOverlayRight.get());
		m_pQuadOverlayRight->SetPosition(point(0.0f, 0.0f, -0.0045f));
		m_pQuadOverlayRight->SetVisible(false);
		//m_pTestQuadTexture = m_pRightControllerModel->MakeTexture(L"Controller-Overlay-Fake.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	}

	//CR(pDreamOS->RegisterEventSubscriber(pController, ELEMENT_INTERSECT_BEGAN, this));
	//CR(pDreamOS->RegisterEventSubscriber(pController, ELEMENT_INTERSECT_MOVED, this));
	//CR(pDreamOS->RegisterEventSubscriber(pController, ELEMENT_INTERSECT_ENDED, this));

	auto pMesh = pController->GetFirstChild<mesh>().get();
	CNR(pMesh, R_SKIPPED);
	pDreamOS->AddObjectToInteractionGraph(pMesh);
	CR(pDreamOS->RegisterEventSubscriber(pMesh, ELEMENT_INTERSECT_BEGAN, this));
	CR(pDreamOS->RegisterEventSubscriber(pMesh, ELEMENT_INTERSECT_MOVED, this));
	CR(pDreamOS->RegisterEventSubscriber(pMesh, ELEMENT_INTERSECT_ENDED, this));

	/*
	auto pPhantomVolume = pHand->GetFirstChild<volume>().get();

	CR(pDreamOS->RegisterEventSubscriber(pPhantomVolume, ELEMENT_INTERSECT_BEGAN, this));
	CR(pDreamOS->RegisterEventSubscriber(pPhantomVolume, ELEMENT_INTERSECT_MOVED, this));
	CR(pDreamOS->RegisterEventSubscriber(pPhantomVolume, ELEMENT_INTERSECT_ENDED, this));
	//*/

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
	vector vLookXZ = GetCameraLookXZ();
	vector vUp = vector(0.0f, 1.0f, 0.0f);

	CN(pCamera);
	CN(m_pLeftHand);
	CN(m_pRightHand);
	{
		float dist = 0.0f;

		point ptCamera = pCamera->GetPosition();
		vector vPos;
		for (auto& hand : { m_pLeftHand, m_pRightHand }) {
			float handDist = 0.0f;
			point ptHand = hand->GetPosition(true);
			vector vHand = ptHand - pCamera->GetOrigin(true);
			vector vTempPos = vLookXZ * (vHand.dot(vLookXZ));
			if (vTempPos.magnitudeSquared() > vPos.magnitudeSquared())
				vPos = vTempPos;
		}

		point lookOffset = vPos + point(0.0f, yPos, 0.0f);

		m_pAppBasis->SetPosition(pCamera->GetPosition() + lookOffset);
		m_pAppBasis->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vLookXZ));
	}

Error:
	return r;

}

RESULT DreamUserApp::HandleKBEnterEvent() {
	RESULT r = R_PASS;
	CB(!m_appStack.empty());
	m_appStack.top()->HandleEvent(UserObserverEventType::KB_ENTER);
Error:
	return r;
}

UIKeyboardHandle *DreamUserApp::GetKeyboard() {
	return m_pKeyboardHandle;
}

RESULT DreamUserApp::ReleaseKeyboard() {
	return R_PASS;
}