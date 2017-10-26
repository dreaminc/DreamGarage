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

RESULT DreamUserHandle::SendHapticImpulse(VirtualObj *pEventObj) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(CreateHapticImpulse(pEventObj));
Error:
	return r;
}

RESULT DreamUserHandle::SendPresentApp(ActiveAppType type) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(PresentApp(type));
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

	CR(pDreamOS->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_BEGAN, this));
	CR(pDreamOS->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_MOVED, this));
	CR(pDreamOS->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_ENDED, this));

	CR(pDreamOS->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_MENU, this));

	// Initialize mallets
	m_pLeftMallet = new UIMallet(pDreamOS);
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(pDreamOS);
	CN(m_pRightMallet);

	pDreamOS->AddInteractionObject(m_pLeftMallet->GetMalletHead());
	pDreamOS->AddInteractionObject(m_pRightMallet->GetMalletHead());

	m_pAppBasis = pDreamOS->MakeComposite();

Error:
	return r;
}

RESULT DreamUserApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
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

	if (m_pVolume == nullptr) {
		m_pVolume = GetComposite()->AddVolume(1.0f);
		CN(m_pVolume);
		m_pVolume->SetVisible(false);
	}
	
	if (m_pOrientationRay == nullptr) {
		m_pOrientationRay = GetComposite()->AddRay(point(0.0f, 0.0f, -0.75f), vector::kVector(-1.0f), 1.0f);
		CN(m_pOrientationRay);
		m_pOrientationRay->SetVisible(false);
		CR(GetDOS()->AddInteractionObject(m_pOrientationRay.get()));
	}

	// Update Mallet Positions
	qOffset.SetQuaternionRotationMatrix(m_pLeftHand->GetOrientation());

	if (m_pLeftMallet)
		m_pLeftMallet->GetMalletHead()->MoveTo(m_pLeftHand->GetPosition() + point(qOffset * m_pLeftMallet->GetHeadOffset()));

	qOffset = RotationMatrix();
	qOffset.SetQuaternionRotationMatrix(m_pRightHand->GetOrientation());

	if (m_pRightMallet)
		m_pRightMallet->GetMalletHead()->MoveTo(m_pRightHand->GetPosition() + point(qOffset * m_pRightMallet->GetHeadOffset()));

Error:
	return r;
}

RESULT DreamUserApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	if (mEvent->m_eventType == INTERACTION_EVENT_MENU) {

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

		CB(keyUIDs.size() == 1);
		auto pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(pDreamOS->CaptureApp(keyUIDs[0], this));

		CB(controlUIDs.size() == 1);
		auto pControlHandle = dynamic_cast<DreamControlViewHandle*>(pDreamOS->CaptureApp(controlUIDs[0], this));

		switch (m_activeState) {

			case ActiveAppType::NONE: {

				//TODO: set app basis

				if (pMenuHandle != nullptr) {
					m_activeState = ActiveAppType::MENU;
					UpdateCompositeWithHands(m_menuHeight);
					pMenuHandle->SendShowRootMenu();
				}
				m_pLeftMallet->Show();
				m_pRightMallet->Show();

			} break;

			case ActiveAppType::MENU: {
				if (pMenuHandle != nullptr) {
					bool fHide = false;
					bool fPathEmpty;
					CR(pMenuHandle->RequestPathEmpty(fPathEmpty));

					fHide = fPathEmpty;
					if (!fPathEmpty) {
						pMenuHandle->SendPopPath();
					}
					
					CR(pMenuHandle->RequestPathEmpty(fPathEmpty));
					fHide = fHide || fPathEmpty;

					if (fHide) {
						m_activeState = ActiveAppType::NONE;
						pMenuHandle->SendHideApp();
						m_pLeftMallet->Hide();
						m_pRightMallet->Hide();
					}
				}
			} break;

			case ActiveAppType::KB_MENU: {
				m_activeState = ActiveAppType::MENU;

				if (pKeyboardHandle != nullptr && pKeyboardHandle->IsVisible()) {
					pKeyboardHandle->Hide();
				}

				if (pMenuHandle != nullptr) {
					pMenuHandle->SendRequestMenu();
				}

			} break;

			case ActiveAppType::CONTROL: {
				m_activeState = ActiveAppType::NONE;
				m_pLeftMallet->Hide();
				m_pRightMallet->Hide();

				if (pMenuHandle != nullptr) {
					pMenuHandle->SendRequestMenu();
				}

				if (pControlHandle != nullptr && pControlHandle->IsAppVisible()) {
					pControlHandle->HideApp();
				}

			} break;

			case ActiveAppType::KB_CONTROL: {
				m_activeState = ActiveAppType::CONTROL;
				//TODO: transition between text focus keyboard and control view
			} break;

			default: break;
		}

		GetDOS()->ReleaseApp(pControlHandle, controlUIDs[0], this);
		GetDOS()->ReleaseApp(pKeyboardHandle, keyUIDs[0], this);
		GetDOS()->ReleaseApp(pMenuHandle, menuUIDs[0], this);
	}


Error:
	return r;
}

RESULT DreamUserApp::PresentApp(ActiveAppType type) {
	RESULT r = R_PASS;
	//TODO: potentially move more logic from the apps into here

	auto pDreamOS = GetDOS();

	//*
	switch (type) {

		case (ActiveAppType::KB_CONTROL):
		case (ActiveAppType::KB_MENU): {
			point ptOrigin;
			quaternion qOrigin;

			auto keyUIDs = pDreamOS->GetAppUID("UIKeyboard");
			CB(keyUIDs.size() == 1);
			auto pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(pDreamOS->CaptureApp(keyUIDs[0], this));

			CN(pKeyboardHandle);

			GetAppBasisPosition(ptOrigin);
			GetAppBasisOrientation(qOrigin);
			pKeyboardHandle->SendUpdateComposite(m_menuDepth, ptOrigin, qOrigin);
			pKeyboardHandle->Show();
			GetDOS()->ReleaseApp(pKeyboardHandle, keyUIDs[0], this);
		} break;
	
		case (ActiveAppType::CONTROL): {
			auto viewUIDs = pDreamOS->GetAppUID("DreamControlView");
			CB(viewUIDs.size() == 1);
			auto pControlViewHandle = dynamic_cast<DreamControlViewHandle*>(GetDOS()->CaptureApp(viewUIDs[0], this));
			CN(pControlViewHandle);

			CR(pControlViewHandle->ShowApp());
			CR(GetDOS()->ReleaseApp(pControlViewHandle, viewUIDs[0], this));
		} break;
	}

	
	//*/
	m_activeState = type;

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

RESULT DreamUserApp::SetHand(hand *pHand) {
	RESULT r = R_PASS;
	if (pHand->GetHandState().handType == HAND_TYPE::HAND_LEFT) {
		m_pLeftHand = pHand;
	}
	else if (pHand->GetHandState().handType == HAND_TYPE::HAND_RIGHT) {
		m_pRightHand = pHand;
	}
	return R_PASS;
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