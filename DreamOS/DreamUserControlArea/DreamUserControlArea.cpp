#include "DreamUserControlArea.h"
#include "DreamContentSource.h"

#include "DreamOS.h"
#include "DreamUserApp.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/UITabView.h"
#include "DreamControlView/UIControlView.h"
#include "DreamGarage/DreamDesktopDupplicationApp/DreamDesktopApp.h"
#include "DreamVCam.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	
#include "WebBrowser/DOMNode.h"
#include "Cloud/Environment/EnvironmentAsset.h"	
#include "Cloud/Environment/EnvironmentShare.h"	

#include "InteractionEngine/InteractionObjectEvent.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UIButton.h"
#include "UI/UIFlatScrollView.h"
#include "UI/UISurface.h"

#include "Sound/AudioPacket.h"

DreamUserControlArea::DreamUserControlArea(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUserControlArea>(pDreamOS, pContext)
{
	// empty
}

DreamUserControlArea::~DreamUserControlArea()  {
	// empty
}

RESULT DreamUserControlArea::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	m_fCanPressButton[0] = false;
	m_fCanPressButton[1] = false;

	SetAppName("hello");

Error:
	return r;
}

RESULT DreamUserControlArea::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamUserControlArea::Update(void *pContext) {
	RESULT r = R_PASS;

	point ptOrigin;
	quaternion qOrigin;

	if (m_pDreamUserApp != nullptr && m_pDreamUIBar == nullptr) {

		m_pDreamUIBar = GetDOS()->LaunchDreamApp<DreamUIBar>(this, false);
		CN(m_pDreamUIBar);

		m_pDreamVCam = GetDOS()->LaunchDreamModule<DreamVCam>(this);
		CN(m_pDreamVCam);

		if (GetDOS()->GetSandboxConfiguration().fInitNamedPipe) {
			m_pDreamVCam->InitializePipeline();
		}

		m_pView = GetComposite()->AddUIView(GetDOS());
		CN(m_pView);

		m_pUserControls = m_pView->AddUIContentControlBar();
		CN(m_pUserControls);
		m_pUserControls->Initialize(this);
		//GetDOS()->AddObjectToUIGraph(m_pUserControls.get());

		m_pDreamTabView = m_pView->AddUITabView();
		CN(m_pDreamTabView);
		m_pDreamTabView->Initialize(this);
		//GetDOS()->AddObjectToUIGraph(m_pDreamTabView.get());

		m_pControlView = m_pView->AddUIControlView();
		CN(m_pControlView);
		m_pControlView->Initialize();
		//GetDOS()->AddObjectToUIGraph(m_pControlView.get());

		m_pControlView->RegisterSubscriber(UI_SELECT_BEGIN, this);
		m_pControlView->RegisterSubscriber(UI_SELECT_MOVED, this);
		m_pControlView->RegisterSubscriber(UI_SELECT_ENDED, this);
		m_pControlView->RegisterSubscriber(UI_SCROLL, this);

		m_pDreamUIBar->SetUIStageProgram(m_pUIStageProgram);
		m_pDreamUIBar->InitializeWithParent(this);

		// DreamUserApp can call Update Composite in certain situations and automatically update the other apps
		m_pDreamUserApp->GetComposite()->AddObject(std::shared_ptr<composite>(GetComposite()));

		float viewAngleRad = m_pDreamUserApp->GetViewAngle() * (float)(M_PI) / 180.0f;
		quaternion qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(viewAngleRad, 0.0f, 0.0f);
		//point ptOrigin = point(0.0f, VIEW_POS_HEIGHT, VIEW_POS_DEPTH);
		point ptOrigin = point(0.0f, VIEW_POS_HEIGHT, -VIEW_POS_DEPTH);
		
		GetComposite()->SetOrientation(qViewQuadOrientation);
		GetComposite()->SetPosition(ptOrigin);

		//DreamUserControlArea is a friend of these classes to add the composite
		//GetComposite()->AddObject(m_pUserControls);
		//GetComposite()->AddObject(m_pDreamTabView);
		//GetComposite()->AddObject(m_pControlView);

		m_pUserControls->Hide();
		m_pDreamTabView->SetVisible(false);
		m_pControlView->SetVisible(false, false);

		CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_MENU, this));
		CR(GetDOS()->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this));
		CR(GetDOS()->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_UP, this));

		for (int i = 0; i < HMDEventType::HMD_EVENT_INVALID; i++) {
			CR(GetDOS()->RegisterSubscriber((HMDEventType)(i), this));
		}

		float currentCenter = m_pControlView->GetBackgroundWidth() / 2.0f;
		float totalCenter = (m_pControlView->GetBackgroundWidth() + m_pDreamUserApp->GetSpacingSize() + m_pDreamTabView->GetBorderWidth()) / 2.0f;
		//m_centerOffset = currentCenter - totalCenter;
		
		GetDOS()->GetKeyboardApp()->InitializeWithParent(this);
		// TODO: bad
		GetComposite()->AddObject(std::shared_ptr<composite>(GetDOS()->GetKeyboardApp()->GetComposite()));

		GetDOS()->AddObjectToUIGraph(GetComposite());
	}

	if (m_fPendDreamFormSuccess) {
		m_fPendDreamFormSuccess = false;
		CR(m_pDreamUIBar->HandleEvent(UserObserverEventType::DISMISS));
	}

	CR(m_pUserControls->Update());
	CR(m_pDreamTabView->Update());
	CR(m_pControlView->Update());

	if (m_pDreamUIBar != nullptr && m_fUpdateDreamUIBar) {
		CR(m_pDreamUIBar->ResetAppComposite());

		m_fUpdateDreamUIBar = false;
	}

	CNR(m_pDreamUserApp, R_SKIPPED);
	
	hand* pLHand;
	pLHand = m_pDreamUserApp->GetHand(HAND_TYPE::HAND_LEFT);
	CNR(pLHand, R_SKIPPED);

	hand* pRHand;
	pRHand = m_pDreamUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
	CNR(pRHand, R_SKIPPED);	

	//m_pDreamUserApp->UpdateCompositeWithHands(-0.16f);
	//m_pDreamUserApp->GetAppBasisPosition(ptOrigin);
	//m_pDreamUserApp->GetAppBasisOrientation(qOrigin);

	//GetComposite()->SetOrientation(qOrigin);

	//TODO: change this to a UISurface
	for (int i = 0; i < 2; i++) {
		hand *pHand;
		HAND_TYPE type;
		if (i == 0) {
			pHand = pLHand;
			type = HAND_TYPE::HAND_LEFT;
		}
		else {
			pHand = pRHand;
			type = HAND_TYPE::HAND_RIGHT;
		}
		// Update using mallets, send relevant information to child apps
		auto pComposite = GetComposite();
		point ptBoxOrigin = pComposite->GetOrigin(true);
		point ptSphereOrigin = pHand->GetMalletHead()->GetOrigin(true);
		ptSphereOrigin = (point)(inverse(RotationMatrix(pComposite->GetOrientation(true))) * (ptSphereOrigin - pComposite->GetOrigin(true)));

		// clear flags
		if (ptSphereOrigin.y() >= pHand->GetMalletRadius()) {
			m_fCanPressButton[i] = true;
		}

		// Update DreamTabView
		float tabViewWidth = m_pDreamTabView->GetBorderWidth();
		float tabViewHeight = m_pDreamTabView->GetBorderHeight();

		point ptDreamTabView = m_pDreamTabView->GetPosition();

		bool fWidth = ( ptSphereOrigin.x() > ptDreamTabView.x() - tabViewWidth / 2.0f  &&
						ptSphereOrigin.x() < ptDreamTabView.x() + tabViewWidth / 2.0f);

		bool fHeight = (ptSphereOrigin.z() > ptDreamTabView.z() - tabViewHeight / 2.0f &&
						ptSphereOrigin.z() < ptDreamTabView.z() + tabViewHeight / 2.0f);

		m_fMalletInTabView[i] = fWidth && fHeight;

		m_pDreamTabView->SetScrollFlag(m_fMalletInTabView[i], i);
		
	}

	if (m_pPendingEnvironmentAsset != nullptr) {
		AddEnvironmentAsset(m_pPendingEnvironmentAsset);
		m_pPendingEnvironmentAsset = nullptr;
	}

Error:
	return r;
}

RESULT DreamUserControlArea::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

DreamUserControlArea* DreamUserControlArea::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUserControlArea *pDreamApp = new DreamUserControlArea(pDreamOS, pContext);
	return pDreamApp;
}

float DreamUserControlArea::GetBaseWidth() {
	return m_pDreamUserApp->GetBaseWidth();
}

RESULT DreamUserControlArea::SetViewHeight(float height) {

	point ptOrigin = m_pDreamUserApp->m_pAppBasis->GetPosition();
	//point ptOrigin = m_pDreamUserApp->GetComposite()->GetPosition();//
	m_pDreamUserApp->m_pAppBasis->SetPosition(point(ptOrigin.x(), height, ptOrigin.z()));
	m_pDreamUserApp->GetComposite()->SetPosition(m_pDreamUserApp->m_pAppBasis->GetPosition());

	return R_PASS;
}

float DreamUserControlArea::GetBaseHeight() {
	return m_pDreamUserApp->GetBaseHeight();
}

float DreamUserControlArea::GetSpacingSize() {
	return m_pDreamUserApp->GetSpacingSize();
}

float DreamUserControlArea::GetViewAngle() {
	return m_pDreamUserApp->GetViewAngle();
}

point DreamUserControlArea::GetCenter() {
	return point(0.0f, 0.0f, m_pDreamTabView->GetPosition().z());
}

float DreamUserControlArea::GetCenterOffset() {
	return m_centerOffset;
}

float DreamUserControlArea::GetTotalWidth() {
	return m_pControlView->GetViewQuad()->GetWidth() + m_pDreamTabView->GetBorderWidth() + m_pDreamUserApp->GetSpacingSize()/2.0f;
}

float DreamUserControlArea::GetTotalHeight() {
	return m_pDreamTabView->GetBorderHeight();
}

RESULT DreamUserControlArea::Show() {
	RESULT r = R_PASS;

	m_pDreamUIBar->ClearMenuWaitingFlag();
	//TODO: animations
	//m_pControlView->GetComposite()->SetVisible(true);
	m_pControlView->Show();
	//m_pDreamTabView->GetComposite()->SetVisible(true);
	m_pDreamTabView->Show();
	//m_pControlBar->GetComposite()->SetVisible(true);
	m_pUserControls->Show();

	return r;
}

RESULT DreamUserControlArea::Hide() {
	RESULT r = R_PASS;

	//m_pControlView->GetComposite()->SetVisible(false);
	m_pControlView->Hide();
	//m_pDreamTabView->GetComposite()->SetVisible(false);
	m_pDreamTabView->Hide();
	//m_pControlBar->GetComposite()->SetVisible(false);
	m_pUserControls->Hide();

	return r;
}

RESULT DreamUserControlArea::ShowDesktopKeyboard() {
	RESULT r = R_PASS;

	CN(m_pControlView);
	CR(GetDOS()->GetUserApp()->SetEventApp(m_pControlView.get()));
	CR(m_pControlView->HandleKeyboardUp());

Error:
	return r;
}

RESULT DreamUserControlArea::Open() {
	RESULT r = R_PASS;

	CR(Hide());
	CR(m_pDreamUIBar->ShowMenuLevel(MenuLevel::OPEN));
	CR(m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get()));

Error:
	return r;
}

RESULT DreamUserControlArea::Minimize() {
	RESULT r = R_PASS;

	CR(m_pControlView->Hide());
	CR(m_pDreamTabView->Hide());

Error:
	return r;
}

RESULT DreamUserControlArea::Maximize() {
	RESULT r = R_PASS;

	CR(m_pControlView->Show());
	CR(m_pDreamTabView->Show());

Error:
	return r;
}

bool DreamUserControlArea::CanPressButton(UIButton *pButtonContext) {
	RESULT r = R_PASS;

	auto pDreamOS = GetDOS();

	auto pInteractionObj = pButtonContext->GetInteractionObject();
	int dirtyIndex = -1;
	if (pInteractionObj == m_pDreamUserApp->GetHand(HAND_TYPE::HAND_LEFT)->GetMalletHead()) {
		dirtyIndex = 0;
	}
	else if (pInteractionObj == m_pDreamUserApp->GetHand(HAND_TYPE::HAND_RIGHT)->GetMalletHead()) {
		dirtyIndex = 1;
	}

	//CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pView.get()), R_SKIPPED);
	//CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pViewQuad.get()), R_SKIPPED);
	
	CBR(GetComposite()->IsVisible(), R_SKIPPED);

	//only allow button presses while keyboard isn't active
	CBR(!IsAnimating(), R_SKIPPED);
	CBR(dirtyIndex != -1, R_SKIPPED);

	CBR(m_fCanPressButton[dirtyIndex], R_SKIPPED);

	// avoids pressing two control bar buttons at once
	m_fCanPressButton[0] = false;
	m_fCanPressButton[1] = false;

//	CBR(m_pControlBar->IsVisible(), R_SKIPPED);

	CR(m_pDreamUserApp->CreateHapticImpulse(pButtonContext->GetInteractionObject()));

	return true;
Error:
	return false;
}

std::shared_ptr<DreamContentSource> DreamUserControlArea::GetActiveSource() {
	return m_pActiveSource;
}

std::shared_ptr<DreamContentSource> DreamUserControlArea::GetActiveCameraSource() {
	return m_pActiveCameraSource;
}

RESULT DreamUserControlArea::ShowControlView() {
	RESULT r = R_PASS;

	auto pView = m_pControlView->GetViewQuad();

	auto fnEndCallback = [&](void *pContext) {
		SetIsAnimating(false);
		return R_PASS;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pView.get(),
		pView->GetPosition(),
		pView->GetOrientation(),
		1.0f,
		m_pDreamUserApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamUserControlArea::SetActiveSource(std::shared_ptr<DreamContentSource> pNewContent) {
	RESULT r = R_PASS;

	m_pActiveSource = pNewContent;
	if (m_pUserControls != nullptr) {
		m_pUserControls->SetTitleText(m_pActiveSource->GetTitle());
		m_pUserControls->UpdateControlBarButtonsWithType(m_pActiveSource->GetContentType());
	}
	if (m_pControlView != nullptr) {
		m_pControlView->SetURLText(pNewContent->GetURL());
	}

	//m_pControlView->SetViewQuadTexture(m_pActiveSource->GetSourceTexture());


	auto pView = m_pControlView->GetViewQuad();
	SetIsAnimating(true);

	// close active browser
	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pView = m_pControlView->GetViewQuad();

		// replace with top of tab bar
		m_pControlView->SetViewQuadTexture(m_pActiveSource->GetSourceTexture());
		
		if (m_fFromMenu) {	// if we opened desktop through menu for not the first time
			m_fFromMenu = false;
			SetIsAnimating(false);
			Show();
		}
		else {
			CR(ShowControlView());
		}

	Error:
		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pView.get(),
		pView->GetPosition(),
		pView->GetOrientation(),
		//vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
		m_animationScale,
		m_pDreamUserApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamUserControlArea::UpdateContentSourceTexture(texture* pTexture, std::shared_ptr<DreamContentSource> pContext) {
	if (pContext == m_pActiveSource) {
		m_pControlView->SetViewQuadTexture(pTexture);
	}
	else {
		m_pDreamTabView->UpdateContentTexture(pContext);
	}
	return R_PASS;
}

RESULT DreamUserControlArea::UpdateControlBarText(std::string& strTitle) {
	RESULT r = R_PASS;

	CR(m_pUserControls->SetTitleText(strTitle));

Error:
	return r;
}

RESULT DreamUserControlArea::UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	CR(m_pUserControls->UpdateNavigationButtons(fCanGoBack, fCanGoForward));
	
Error:
	return r;
}

RESULT DreamUserControlArea::UpdateAddressBarSecurity(bool fSecure) {
	RESULT r = R_PASS;

	CR(m_pControlView->SetURLSecurity(fSecure));

Error:
	return r;
}

RESULT DreamUserControlArea::UpdateAddressBarText(std::string& strURL) {
	RESULT r = R_PASS;

	CR(m_pControlView->SetURLText(strURL));

Error:
	return r;
}

RESULT DreamUserControlArea::HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	// if the content source that received an audio packet is the active piece of content, 
	// send the audio packet to the shared view
	//if (pContext == m_pActiveSource.get()) {
	auto pDreamOS = GetDOS();
	CNR(pContext, R_SKIPPED);
	CNR(pContext->GetSourceTexture(), R_SKIPPED);
	CNR(pDreamOS, R_SKIPPED);
	//CNR(pDreamOS->GetSharedContentTexture(), R_SKIPPED);

	if (pDreamOS->GetSharedContentTexture() != nullptr && pContext->GetSourceTexture() == pDreamOS->GetSharedContentTexture()) {
		auto pCloudController = GetDOS()->GetCloudController();
		if (pCloudController != nullptr) {
			CR(GetDOS()->BroadcastSharedAudioPacket(pendingAudioPacket));
			
			// if it's not the vcam shared, then send the audio to mixdown
			if (pContext->GetSourceTexture() != GetDOS()->GetSharedCameraTexture()) {
				
				CR(GetDOS()->PushAudioPacketToMixdown(DreamSoundSystem::MIXDOWN_TARGET::LOCAL_BROWSER_0, pendingAudioPacket.GetNumFrames(), pendingAudioPacket));
			}
		}
	}
	else if (pDreamOS->GetSharedCameraTexture() != nullptr && pContext->GetSourceTexture() == pDreamOS->GetSharedCameraTexture()) {
		CR(GetDOS()->GetCloudController()->BroadcastAudioPacket(kVCamAudiolabel, pendingAudioPacket));
	}

Error:
	return r;
}

RESULT DreamUserControlArea::HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	bool fMaskPasswordEnabled = false;

	std::shared_ptr<UIKeyboard> pKeyboard = GetDOS()->GetKeyboardApp();
	CN(pKeyboard);

	CBR(pContext == m_pActiveSource.get(), R_SKIPPED);
	CN(pDOMNode);

	if (pDOMNode->GetType() == DOMNode::type::ELEMENT && pDOMNode->IsEditable() && !m_fKeyboardUp) {
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		fMaskPasswordEnabled = pDOMNode->IsPassword();

		CR(pKeyboard->ShowBrowserButtons());
		CR(m_pControlView->HandleKeyboardUp());
		CR(m_pUserControls->Hide());
		CR(m_pDreamTabView->Hide());

		std::string strTextField = pDOMNode->GetValue();
		pKeyboard->PopulateKeyboardTextBox(strTextField);

		m_fKeyboardUp = true;
	}

	pKeyboard->SetPasswordFlag(fMaskPasswordEnabled);

Error:
	return r;
}

RESULT DreamUserControlArea::HandleIsInputFocused(bool fIsFocused, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	CBR(pContext == m_pActiveSource.get(), R_SKIPPED);

	if (fIsFocused) {
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		auto pKeyboard = GetDOS()->GetKeyboardApp();
		CN(pKeyboard);
		CR(pKeyboard->ShowBrowserButtons());
		CR(m_pControlView->HandleKeyboardUp());
		CR(m_pUserControls->Hide());
		CR(m_pDreamTabView->Hide());
		m_fKeyboardUp = true;
	}
	else {
		CR(HideWebsiteTyping());
	}

Error:
	return r;
}

RESULT DreamUserControlArea::HandleLoadEnd() {
	return R_NOT_IMPLEMENTED;
}

bool DreamUserControlArea::IsContentVisible() {
	return m_pControlView != nullptr && m_pControlView->IsVisible();
}

RESULT DreamUserControlArea::HandleDreamFormSuccess() {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamUserControlArea::HandleDreamFormCancel() {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamUserControlArea::HandleCanTabNext(bool fCanNext) {
	RESULT r = R_PASS;
	auto pKeyboard = GetDOS()->GetKeyboardApp();
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabNextTexture(fCanNext));
Error:
	return r;
}

RESULT DreamUserControlArea::HandleCanTabPrevious(bool fCanPrevious) {
	RESULT r = R_PASS;
	auto pKeyboard = GetDOS()->GetKeyboardApp();
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabPreviousTexture(fCanPrevious));
Error:
	return r;
}

std::string DreamUserControlArea::GetCertificateErrorURL() {
	return m_strCertificateErrorURL;
}

std::string DreamUserControlArea::GetLoadErrorURL() {
	return m_strLoadErrorURL;
}

RESULT DreamUserControlArea::OnVirtualCameraCaptured() {
	RESULT r = R_PASS;

	m_pActiveCameraSource = m_pActiveSource;
	DOSLOG(INFO, "Grabbed browser that's using camera");

Error:
	return r;
}

RESULT DreamUserControlArea::OnVirtualCameraReleased() {
	RESULT r = R_PASS;

	m_pActiveCameraSource = nullptr;

	CR(m_pDreamVCam->HideCameraSource());
	CR(m_pDreamVCam->StopSharing());

Error:
	return r;
}

RESULT DreamUserControlArea::OnVirtualCameraSettings(point ptPosition, quaternion qOrientation) {
	RESULT r = R_PASS;

	CNM(m_pDreamVCam, "Received vcam settings but vcam was null?!");
	CR(m_pDreamVCam->HandleSettings(ptPosition, qOrientation));

Error:
	return r;
}

RESULT DreamUserControlArea::SetVirtualCameraSource(DreamVCam::SourceType sourceType) {
	RESULT r = R_PASS;

	CR(m_pDreamVCam->SetSourceType(sourceType));

Error:
	return r;
}

RESULT DreamUserControlArea::MuteVirtualCamera(bool fMute) {
	RESULT r = R_PASS;

	CR(m_pDreamVCam->Mute(fMute));

Error:
	return r;
}

RESULT DreamUserControlArea::ResetVirtualCamera() {
	RESULT r = R_PASS;

	point ptPosition;
	quaternion qOrientation;

	CR(GetDOS()->GetDefaultVCamPlacement(ptPosition, qOrientation));

	CN(m_pDreamVCam);
	CR(m_pDreamVCam->HandleSettings(ptPosition, qOrientation));

	CR(GetDOS()->SaveCameraSettings(ptPosition, qOrientation));

Error:
	return r;
}

int DreamUserControlArea::GetWidth() {
	return m_pActiveSource->GetWidth();
}

int DreamUserControlArea::GetHeight() {
	return m_pActiveSource->GetHeight();
}

RESULT DreamUserControlArea::OnClick(point ptContact, bool fMouseDown) {
	RESULT r = R_PASS;

	CNR(m_pActiveSource, R_SKIPPED);
	CR(m_pActiveSource->OnClick(ptContact, fMouseDown));

Error:
	return r;
}

RESULT DreamUserControlArea::OnKeyPress(char chkey, bool fKeyDown) {
	RESULT r = R_PASS;

	CNR(m_pActiveSource, R_SKIPPED);
	CR(m_pActiveSource->OnKeyPress(chkey, fKeyDown));

Error:
	return r;
}

RESULT DreamUserControlArea::OnMouseMove(point mousePoint) {
	RESULT r = R_PASS;

	CNR(m_pActiveSource, R_SKIPPED);
	CR(m_pActiveSource->OnMouseMove(mousePoint));

Error:
	return r;
}

RESULT DreamUserControlArea::OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) {
	RESULT r = R_PASS;

	CNR(m_pActiveSource, R_OBJECT_NOT_FOUND);
	CR(m_pActiveSource->OnScroll(pxXDiff, pxYDiff, scrollPoint));

Error:
	return r;
}

RESULT DreamUserControlArea::SetScope(std::string strScope) {
	RESULT r = R_PASS;
	
	CNR(m_pActiveSource, R_SKIPPED);
	CR(m_pActiveSource->SetScope(strScope));
	
Error:
	return r;
}

RESULT DreamUserControlArea::SetPath(std::string strPath) {
	RESULT r = R_PASS;
	
	CNR(m_pActiveSource, R_SKIPPED);
	CR(m_pActiveSource->SetPath(strPath));
	
Error:
	return r;
}

RESULT DreamUserControlArea::RequestOpenAsset(std::string strScope, std::string strPath, std::string strTitle) {
	RESULT r = R_PASS;

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(pEnvironmentControllerProxy, "Failed to get environment controller proxy");

	if (strScope == m_strDesktopScope) {
		if (m_pDreamDesktop == nullptr) {
			CRM(pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");
		}
	}

	else if (strScope == m_strCameraScope) {
		// TODO: temp
		if (m_pDreamVCam != nullptr) {
			
			CRM(pEnvironmentControllerProxy->RequestOpenCamera(), "Failed to share environment asset");
		}
	}

	else {
		CRM(pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");
	}

Error:
	return r;
}

RESULT DreamUserControlArea::CreateBrowserSource(std::string strScope) {
	RESULT r = R_PASS;

	std::string strTitle = "Website";
	std::string strURL = GetDOS()->GetKeyboardApp()->GetText();

	if (strURL == "") {
		CR(m_pDreamUIBar->HandleEvent(UserObserverEventType::BACK));
		m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get());
	}
	else {
		CR(RequestOpenAsset(strScope, strURL, strTitle));
	}
Error:
	return r;
}

RESULT DreamUserControlArea::HideWebsiteTyping() {
	RESULT r = R_PASS;

	if (m_fKeyboardUp) {
	//	CR(m_pDreamUserApp->GetKeyboard()->Hide());
		m_fKeyboardUp = false;
		CR(m_pControlView->HandleKeyboardDown());
		m_pUserControls->Show();
		m_pDreamTabView->Show();
		auto pBrowser = dynamic_cast<DreamBrowser*>(m_pActiveSource.get());
		CNR(pBrowser, R_SKIPPED);
		CR(pBrowser->HandleUnfocusEvent());
	}

Error:
	return r;
}

RESULT DreamUserControlArea::OnCameraInMotion() {
	RESULT r = R_PASS;

	/*
	m_pControlView->FlipViewUp();
	m_pDreamTabView->Hide();
	m_pUserControls->Hide();
	//*/

	return r;
}

RESULT DreamUserControlArea::OnCameraAtRest() {
	RESULT r = R_PASS;

	/*
	m_pControlView->FlipViewDown();
	m_pDreamTabView->Show();
	m_pUserControls->Show();
	//*/

	return r;
}

RESULT DreamUserControlArea::ResetAppComposite() {
	RESULT r = R_PASS;

	CN(m_pDreamUserApp);

	/*
	auto pRenderContext = m_pDreamTabView->m_pScrollView->GetRenderContext();
	point ptOrigin = m_pDreamTabView->GetComposite()->GetPosition(true);
	quaternion qOrigin = m_pDreamTabView->GetComposite()->GetOrientation(true);
	pRenderContext->SetPosition(ptOrigin);
	pRenderContext->SetOrientation(qOrigin);
	//*/

//	m_fUpdateDreamUIBar = true;
	
	m_pDreamUserApp->ResetAppComposite();
	
	if (m_pDreamUIBar != nullptr) {
		m_pDreamUIBar->ResetAppComposite();
	}

Error:
	return r;
}

RESULT DreamUserControlArea::PendEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	m_pPendingEnvironmentAsset = pEnvironmentAsset;
	return R_PASS;
}

RESULT DreamUserControlArea::PendCameraEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;
	CN(m_pDreamVCam);

	m_pPendingEnvironmentCameraAsset = pEnvironmentAsset;
	m_pDreamVCam->SetEnvironmentAsset(pEnvironmentAsset);

Error:
	return r;
}

RESULT DreamUserControlArea::AddEnvironmentCameraAsset() {
	DOSLOG(INFO, "adding environment camera asset");
	m_pPendingEnvironmentAsset = m_pPendingEnvironmentCameraAsset;
	return R_PASS;
}

RESULT DreamUserControlArea::AddEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;
	
	if (m_pActiveSource != nullptr) {													// If content is already open
		if (pEnvironmentAsset->GetScope() == m_strDesktopScope && m_pDreamDesktop != nullptr) {				// and we're trying to share the desktop for not the first time
			if (m_pDreamDesktop != m_pActiveSource) {									// and desktop is in the tabview	
				SetIsAnimating(false);
				m_fFromMenu = true;
				m_pDreamTabView->SelectByContent(m_pDreamDesktop);						// pull desktop out of tabview
			}
			else {
				Show();
			}
		}	
		else {
			m_pDreamTabView->AddContent(m_pActiveSource);
		}
	}

	
	//it is not safe to set the environment asset until after the browser is finished initializing
	// this is because LoadRequest requires a URL to have been set (about:blank in InitializeWithBrowserManager)
	m_fHasOpenApp = true;
	m_pDreamUserApp->SetHasOpenApp(true);
	if(pEnvironmentAsset->GetContentType() == CAMERA_CONTENT_CONTROL_TYPE && m_pDreamVCam != nullptr) {
		m_pDreamVCam->InitializeWithParent(this);
		m_pActiveSource = m_pDreamVCam;
		m_pUserControls->SetSharingFlag(false);
		m_pDreamVCam->SetEnvironmentAsset(pEnvironmentAsset);
		m_pUserControls->SetTitleText(m_pDreamVCam->GetTitle());
	}
	else if(pEnvironmentAsset->GetScope() == m_strDesktopScope && m_pDreamDesktop == nullptr) {
		// TODO: desktop setup
		m_pDreamDesktop = GetDOS()->LaunchDreamApp<DreamDesktopApp>(this);
		m_pActiveSource = m_pDreamDesktop;
		m_pDreamDesktop->InitializeWithParent(this);
		m_pUserControls->SetTitleText(m_pDreamDesktop->GetTitle());
		// new desktop can't be the current content
		m_pUserControls->SetSharingFlag(false);
		m_pDreamDesktop->SetEnvironmentAsset(pEnvironmentAsset);
	}
	else {
		std::shared_ptr<DreamBrowser> pBrowser = nullptr;
		pBrowser = GetDOS()->LaunchDreamApp<DreamBrowser>(this);

		m_pActiveSource = pBrowser;

		pBrowser->SetScope(pEnvironmentAsset->GetScope());
		pBrowser->SetPath(pEnvironmentAsset->GetPath());
		pBrowser->SetEnvironmentAsset(pEnvironmentAsset);

		pBrowser->InitializeWithBrowserManager(m_pDreamUserApp->GetBrowserManager(), pEnvironmentAsset->GetURL());
		pBrowser->RegisterObserver(this);
		m_pUserControls->SetTitleText(pBrowser->GetTitle());

		if (pEnvironmentAsset->GetTitle() != "website") {
			std::string strTitle = pEnvironmentAsset->GetTitle();
			UpdateControlBarText(strTitle);
		}
		//pBrowser->SetEnvironmentAsset(pEnvironmentAsset);
	}

	//m_pActiveBrowser->SetEnvironmentAsset(pEnvironmentAsset);
	//m_pActiveBrowser->SetURI(pEnvironmentAsset->GetURL());
	//m_pControlView->SetControlViewTexture(m_pActiveBrowser->GetScreenTexture());

	CR(Show());

	m_pUserControls->UpdateControlBarButtonsWithType(pEnvironmentAsset->GetContentType());

Error:
	return r;
}

RESULT DreamUserControlArea::OnReceiveAsset() {
	RESULT r = R_PASS;
	
	// new browser can't be the current content
	m_pUserControls->SetSharingFlag(false);

	return r;
}

RESULT DreamUserControlArea::StartSharing(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	CN(pEnvironmentShare);
	if (pEnvironmentShare->GetShareType() == SHARE_TYPE_SCREEN) {
		CR(m_pActiveSource->SendFirstFrame());

		m_pCurrentScreenShare = pEnvironmentShare;
	}
	else if (pEnvironmentShare->GetShareType() == SHARE_TYPE_CAMERA) {
		CR(m_pDreamVCam->StartSharing(pEnvironmentShare));
	}

Error:
	return r;
}

RESULT DreamUserControlArea::SendFirstFrame() {
	RESULT r = R_PASS;

	// It's impossible to be sharing AND have a nullptr active source
	CNR(m_pActiveSource, R_SKIPPED);

	if (m_pActiveSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture()) {
		CR(m_pActiveSource->SendFirstFrame());
	}
	else {
		for (auto pSource : m_pDreamTabView->GetAllSources()) {
			if (pSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture()) {
				CR(pSource->SendFirstFrame());
			}
		}
	}

Error:
	return r;
}

RESULT DreamUserControlArea::ForceStopSharing() {
	RESULT r = R_PASS;

	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	//*
	CNR(m_pActiveSource, R_SKIPPED);
	if (m_pActiveSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture()) {
	//	CRM(m_pEnvironmentControllerProxy->RequestStopSharing(m_pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
		//TODO: maintain list of shares
		GetDOS()->OnStopSending(m_pCurrentScreenShare);
	}
	else {
		for (auto pSource : m_pDreamTabView->GetAllSources()) {
			if (pSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture()) {
			//	CRM(m_pEnvironmentControllerProxy->RequestStopSharing(pSource->GetCurrentAssetID()), "Failed to share environment asset");
				GetDOS()->OnStopSending(m_pCurrentScreenShare);
			}
		}
	}

Error:
	return r;
}

RESULT DreamUserControlArea::HandleStopSending() {
	RESULT r = R_PASS;

	m_pCurrentScreenShare = nullptr;

Error:
	return r;
}

std::shared_ptr<EnvironmentShare> DreamUserControlArea::GetCurrentScreenShare() {
	return m_pCurrentScreenShare;
}

bool DreamUserControlArea::IsSharingScreen() {
	return (m_pCurrentScreenShare != nullptr);
}

bool DreamUserControlArea::IsScrollingTabs(HAND_TYPE handType) {

	if (handType == HAND_TYPE::HAND_LEFT) {
		return m_fMalletInTabView[0];
	}
	else if (handType == HAND_TYPE::HAND_RIGHT) {
		return m_fMalletInTabView[1];
	}

	return false;
	//return (m_fMalletInTabView[0] || m_fMalletInTabView[1]);
}

RESULT DreamUserControlArea::UpdateIsActive(bool fIsActive) {
	RESULT r = R_PASS;

	CR(m_pUserControls->UpdateIsActive(fIsActive));

Error:
	return r;
}

RESULT DreamUserControlArea::HandleCameraClosed() {
	RESULT r = R_PASS;

	CR(m_pUserControls->HandleCameraClose());

Error:
	return r;
}

RESULT DreamUserControlArea::SetCertificateErrorURL(std::string strURL) {
	m_strCertificateErrorURL = strURL;
	return R_PASS;
}

RESULT DreamUserControlArea::SetLoadErrorURL(std::string strURL) {
	m_strLoadErrorURL = strURL;
	return R_PASS;
}

RESULT DreamUserControlArea::ShutdownSource() {
	RESULT r = R_PASS;

	CNR(m_pActiveSource, R_SKIPPED);

	if (m_pActiveSource == m_pActiveCameraSource) {
		m_pDreamVCam->HideCameraSource();
	}

	m_pActiveSource->CloseSource();
	
	if (m_pDreamDesktop == m_pActiveSource) {
		GetDOS()->ShutdownDreamApp<DreamDesktopApp>(m_pDreamDesktop);
		m_pDreamDesktop = nullptr;
	}
	else if (m_pDreamVCam == m_pActiveSource) {
		// empty (avoid calling ShutdownDreamApp<DreamBrowser>)
	}
	else {	
		auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
		GetDOS()->ShutdownDreamApp<DreamBrowser>(pBrowser);
		//TODO: should set pBrowser to nullptr?
	}

Error:
	return r;
}

RESULT DreamUserControlArea::ShutdownAllSources() {
	RESULT r = R_PASS;

	GetDOS()->OnStopReceivingCameraPlacement();

	m_pDreamUIBar->HandleEvent(UserObserverEventType::DISMISS);

	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	//*
	CNR(m_pActiveSource, R_SKIPPED);
	if (m_pActiveSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture()) {
	//	CRM(m_pEnvironmentControllerProxy->RequestStopSharing(m_pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
		GetDOS()->OnStopSending(m_pCurrentScreenShare);
	}
	else {
		for (auto pSource : m_pDreamTabView->GetAllSources()) {
			if (pSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture()) {
			//	CRM(m_pEnvironmentControllerProxy->RequestStopSharing(pSource->GetCurrentAssetID()), "Failed to share environment asset");
				GetDOS()->OnStopSending(m_pCurrentScreenShare);
			}
		}
	}
	//*/

	// related to crash on exit?
	//GetDOS()->OnStopSending();
	//GetDOS()->OnStopReceiving();

	m_pDreamTabView->FlagShutdownAllSources();
	CloseActiveAsset();

Error:
	return r;
}

RESULT DreamUserControlArea::CloseActiveAsset() {
	RESULT r = R_PASS;

	// close active browser
	SetIsAnimating(true);

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pView = m_pControlView->GetViewQuad();

		// Update VCam
		/*
		if (m_pActiveCameraSource == m_pActiveSource) {
			m_pDreamVCam->HideCameraSource();
			m_pActiveCameraSource = nullptr;
		}
		//*/

		CR(ShutdownSource());
		m_pActiveSource = m_pDreamTabView->RemoveContent();
		//m_pControlView->GetViewQuad()->SetVisible(false);
		// replace with top of tab bar
		if (m_pActiveSource != nullptr) {
			m_pUserControls->SetTitleText(m_pActiveSource->GetTitle());
			m_pUserControls->UpdateControlBarButtonsWithType(m_pActiveSource->GetContentType());

			m_pControlView->SetViewQuadTexture(m_pActiveSource->GetSourceTexture());
			CR(ShowControlView());
		}
		else {
			m_pControlView->GetViewQuad()->SetVisible(false);
			Hide();

			m_fHasOpenApp = false;
			if (m_pDreamUserApp->m_pEventApp == m_pControlView.get()) {
				m_pDreamUserApp->SetHasOpenApp(m_fHasOpenApp);
				m_pDreamUserApp->SetEventApp(nullptr);
			}

			SetIsAnimating(false);
		}

	Error:
		return r;
	};

	auto pView = m_pControlView->GetViewQuad();

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pView.get(),
		pView->GetPosition(),
		pView->GetOrientation(),
		//vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
		m_animationScale,
		m_pDreamUserApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));
	//m_pControlView->HideView();


Error:
	return r;
}

RESULT DreamUserControlArea::CloseCameraTab() {
	RESULT r = R_PASS;

	CR(m_pDreamTabView->RemoveTab(m_pDreamVCam));
	CR(m_pDreamVCam->CloseSource());

Error:
	return r;
}

RESULT DreamUserControlArea::SetUIProgramNode(UIStageProgram *pUIProgramNode) {
	m_pUIStageProgram = pUIProgramNode;
	return R_PASS;
}

float DreamUserControlArea::GetAnimationDuration() {
	return m_pDreamUserApp->GetAnimationDuration();
}

float DreamUserControlArea::GetAnimationScale() {
	return m_animationScale;
}

bool DreamUserControlArea::IsAnimating() {
	return m_fIsAnimating;
}

RESULT DreamUserControlArea::SetIsAnimating(bool fIsAnimating) {
	m_fIsAnimating = fIsAnimating;
	return R_PASS;
}

RESULT DreamUserControlArea::OnDreamFormSuccess() {
	RESULT r = R_PASS;
	
	if (!m_pDreamUIBar->IsEmpty()) {
		m_fPendDreamFormSuccess = true;
	}
	if (m_fHasOpenApp) {
		CR(Show());
		m_pDreamUserApp->SetHasOpenApp(true);
	}
	else {
		m_pDreamUserApp->SetHasOpenApp(false);
	}
	
Error:
	return r;
}

RESULT DreamUserControlArea::Notify(InteractionObjectEvent *pSubscriberEvent) {
	RESULT r = R_PASS;

	DreamUserObserver *pEventApp = m_pDreamUserApp->m_pEventApp;
	CBR(pEventApp == m_pControlView.get() || 
		pEventApp == m_pDreamUIBar.get() ||
		pEventApp == nullptr, R_SKIPPED);

	switch (pSubscriberEvent->m_eventType) {
	case INTERACTION_EVENT_MENU: {
		CNR(m_pDreamUIBar, R_SKIPPED);

		if (m_fKeyboardUp) {
			HideWebsiteTyping();
		}

		else if ((!m_fHasOpenApp && m_pDreamUIBar->IsEmpty()) || (!m_pDreamUserApp->m_fHasOpenApp && !m_fHasOpenApp)) {	// Pulling up Menu from nothing
			ResetAppComposite();
			CR(m_pDreamUIBar->ShowMenuLevel(MenuLevel::ROOT));
			m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get());
			m_pDreamUserApp->SetHasOpenApp(true);

		}
		
		else if (m_fHasOpenApp && (m_pControlView->IsVisible() || m_pUserControls->IsVisible())) {	// Pressing Menu while we have content open or minimized content
			ResetAppComposite();
			Hide();
			m_pDreamUIBar->ShowMenuLevel(MenuLevel::ROOT);
			m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get());
		}

		else {	// Pressing back when Menu has a level saved
			if (m_pDreamUserApp->m_pPreviousApp != m_pDreamUIBar.get()) {
				m_pDreamUIBar->HandleEvent(UserObserverEventType::BACK);
				if (m_pDreamUIBar->IsEmpty()) {	// Case where pressing back on Top level menu
					if (m_fHasOpenApp) {
						Show();
					}
					else {
						m_pDreamUserApp->SetEventApp(nullptr);
						m_pDreamUserApp->SetHasOpenApp(false);
					}
				}
			}
		}
	} break;

	case INTERACTION_EVENT_KEY_UP:
	case INTERACTION_EVENT_KEY_DOWN: {
		char chkey = (char)(pSubscriberEvent->m_value);
		bool fKeyDown = (pSubscriberEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN);
		CBR(chkey != 0x00, R_SKIPPED);	// To catch empty chars used to refresh textbox	

		//TODO: re-enable typing in the browser
		//*
		if (m_fKeyboardUp) {
			// CBR(chkey != SVK_RETURN, R_SKIPPED);		// might be necessary to prevent dupe returns being sent to browser.
			
			bool fIsSpecialKey = (chkey == SVK_TAB || chkey == SVK_SHIFTTAB || chkey == SVK_CLOSE);	// These are keys not actually on our keyboard that we send manually
			auto pBrowser = dynamic_cast<DreamBrowser*>(m_pActiveSource.get());
			if (!fIsSpecialKey) {
				CR(m_pActiveSource->OnKeyPress(chkey, fKeyDown));
			}
			else if (fKeyDown) {
				if (chkey == SVK_TAB) {
					CR(pBrowser->HandleTabEvent());
				}
				else if (chkey == SVK_SHIFTTAB) {
					CR(pBrowser->HandleBackTabEvent());
				}
				else if (chkey == SVK_CLOSE) {
					//CR(pBrowser->HandleUnfocusEvent());
					//CR(m_pControlView->HandleKeyboardDown());
					CR(HideWebsiteTyping());
				}
			}
		}

	}
	}

Error:
	return r;
}

RESULT DreamUserControlArea::Notify(HMDEvent *pEvent) {
	RESULT r = R_PASS;

	DreamUserObserver *pEventApp = m_pDreamUserApp->m_pEventApp;
	CBR(pEventApp == m_pControlView.get() ||
		pEventApp == m_pDreamUIBar.get() /*||
		pEventApp == nullptr*/, R_SKIPPED);

	switch (pEvent->m_eventType) {

	// Restore from previous state
	case HMDEventType::HMD_EVENT_FOCUS: {
		CNR(m_pDreamUIBar, R_SKIPPED);

		if (m_fPendHMDRecenter) {
			CR(ResetAppComposite());
			m_fPendHMDRecenter = false;
		}

		if (m_fKeyboardUp) {
			GetDOS()->GetKeyboardApp()->SetVisible(true);
			GetDOS()->GetKeyboardApp()->ShowBrowserButtons();
		}

		if (pEventApp == m_pControlView.get()) {
			GetComposite()->SetVisible(true, false);
			m_pControlView->GetViewQuad()->SetVisible(true);	// because the quad is in UIScenegraph
			if (m_fWasTabViewOpen) {
				m_pDreamTabView->Show();
				m_fWasTabViewOpen = false;
			}
		}

		else if (pEventApp == m_pDreamUIBar.get()) {
			CR(m_pDreamUIBar->ShowApp());
		}
	} break;

	// Hide UI without changing state so we can restore from it
	case HMDEventType::HMD_EVENT_UNFOCUS: {
		CNR(m_pDreamUIBar, R_SKIPPED);

		if (m_fKeyboardUp) {
			GetDOS()->GetKeyboardApp()->SetVisible(false);
			GetDOS()->GetKeyboardApp()->HideBrowserButtons();
		}

		if (pEventApp == m_pControlView.get()) {
			if (m_pDreamTabView->IsVisible()) {
				m_pDreamTabView->Hide();
				m_fWasTabViewOpen = true;
			}
			GetComposite()->SetVisible(false, false);
			m_pControlView->GetViewQuad()->SetVisible(false);	
		}

		else if (pEventApp == m_pDreamUIBar.get()) {
			CR(m_pDreamUIBar->HideApp());
		}
	} break;
	
	// Reset UI with seat position
	case HMDEventType::HMD_EVENT_RESET_VIEW: {
		m_fPendHMDRecenter = true;
	} break;
	}

Error:
	return r;
}

RESULT DreamUserControlArea::Notify(UIEvent *pUIEvent) {
	RESULT r = R_PASS;
	
	WebBrowserPoint wptContact;
	point ptContact;

	CNR(m_pActiveSource, R_SKIPPED);
	CBR(GetComposite()->IsVisible(), R_SKIPPED);

	wptContact = GetRelativePointofContact(pUIEvent->m_ptEvent);
	ptContact = point(wptContact.x, wptContact.y, 0.0f);

	//TODO: temporary, there could be future UISurfaces associated with the DreamTabView and DreamControlBar
	CBR(pUIEvent->m_pObj == m_pControlView->GetViewQuad().get(), R_SKIPPED);

	switch (pUIEvent->m_eventType) {
	case UI_SELECT_BEGIN: {
		CR(HideWebsiteTyping());
		CR(OnClick(ptContact, true));
	} break;

	case UI_SELECT_ENDED: {
		CR(OnClick(ptContact, false));
	} break;

	case UI_SELECT_MOVED: {
		CR(OnMouseMove(ptContact));
	} break;
	case UI_SCROLL: {

		bool fScrollingTabView = false;

		auto pHand = dynamic_cast<hand*>(pUIEvent->m_pInteractionObject);
		CNR(pHand, R_SKIPPED);

		if (m_fMalletInTabView[0] && pHand->GetHandState().handType == HAND_TYPE::HAND_LEFT) {
			fScrollingTabView = true;
		}
		if (m_fMalletInTabView[1] && pHand->GetHandState().handType == HAND_TYPE::HAND_RIGHT) {
			fScrollingTabView = true;
		}

		if (!fScrollingTabView) {
			CR(OnScroll(pUIEvent->m_vDelta.x(), pUIEvent->m_vDelta.y(), ptContact));
		}
	}
	};

Error:
	return r;
}

WebBrowserPoint DreamUserControlArea::GetRelativePointofContact(point ptContact) {
	point ptIntersectionContact = ptContact;
	ptIntersectionContact.w() = 1.0f;
	WebBrowserPoint ptRelative;

	// First apply transforms to the ptIntersectionContact 
	//point ptAdjustedContact = inverse(m_pViewQuad->GetModelMatrix()) * ptIntersectionContact;
	point ptAdjustedContact = ptIntersectionContact;
	
	float width = m_pControlView->GetViewQuad()->GetWidth();
	float height = m_pControlView->GetViewQuad()->GetHeight();

	float posX = ptAdjustedContact.x() / (width / 2.0f);	
	float posY = ptAdjustedContact.z() / (height / 2.0f);

	//float posZ = ptAdjustedContact.z();	// 3D browser when

	posX = (posX + 1.0f) / 2.0f;	// flip it
	posY = (posY + 1.0f) / 2.0f;  

	ptRelative.x = posX * m_pActiveSource->GetWidth();
	ptRelative.y = posY * m_pActiveSource->GetHeight();

	return ptRelative;
}

RESULT DreamUserControlArea::SetDreamUserApp(std::shared_ptr<DreamUserApp> pDreamUserApp) {
	m_pDreamUserApp = pDreamUserApp;
	return R_PASS;
}

RESULT DreamUserControlArea::BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	CR(DreamAppBase::BroadcastDreamAppMessage(pDreamAppMessage));

Error:
	return r;
}

RESULT DreamUserControlArea::HandleDreamAppMessage(PeerConnection *pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	// DreamModule doesn't have access to these, so UserControlArea is acting as a passthrough
	if (pDreamAppMessage->GetDreamAppName() == "hello") {
		m_pDreamVCam->HandleDreamAppMessage(pPeerConnection, pDreamAppMessage);
	}

Error:
	return r;
}

std::shared_ptr<DreamVCam> DreamUserControlArea::GetVCam() {
	return m_pDreamVCam;
}
