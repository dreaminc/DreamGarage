#include "DreamUserControlArea.h"
#include "DreamContentSource.h"

#include "DreamOS.h"
#include "DreamUserApp.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/DreamTabView.h"
#include "DreamControlView/DreamControlView.h"
#include "DreamGarage/DreamDesktopDupplicationApp/DreamDesktopApp.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	
#include "WebBrowser/DOMNode.h"
#include "Cloud/Environment/EnvironmentAsset.h"	

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
		auto pKeyboard = GetDOS()->LaunchDreamApp<UIKeyboard>(this, false);
		CN(pKeyboard);

		m_pDreamUIBar = GetDOS()->LaunchDreamApp<DreamUIBar>(this, false);
		CN(m_pDreamUIBar);

		m_pControlBar = GetDOS()->LaunchDreamApp<DreamControlBar>(this, false);
		CN(m_pControlBar);
		m_pControlBar->InitializeWithParent(this);

		m_pControlView = GetDOS()->LaunchDreamApp<DreamControlView>(this, false);
		CN(m_pControlView);
		m_pControlView->InitializeWithUserApp(m_pDreamUserApp.get());
		m_pControlView->GetViewSurface()->RegisterSubscriber(UI_SELECT_BEGIN, this);
		m_pControlView->GetViewSurface()->RegisterSubscriber(UI_SELECT_MOVED, this);
		m_pControlView->GetViewSurface()->RegisterSubscriber(UI_SELECT_ENDED, this);
		m_pControlView->GetViewSurface()->RegisterSubscriber(UI_SCROLL, this);

		m_pDreamTabView = GetDOS()->LaunchDreamApp<DreamTabView>(this, false);
		CN(m_pDreamTabView);
		m_pDreamTabView->InitializeWithParent(this);

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
		GetComposite()->AddObject(std::shared_ptr<composite>(m_pControlBar->GetComposite()));
		GetComposite()->AddObject(std::shared_ptr<composite>(m_pControlView->GetComposite()));
		GetComposite()->AddObject(std::shared_ptr<composite>(m_pDreamTabView->GetComposite()));
		//GetComposite()->AddObject(std::shared_ptr<composite>(m_pDreamUIBar->GetComposite()));

		m_pControlBar->Hide();
		m_pDreamTabView->GetComposite()->SetVisible(false);
		m_pControlView->GetComposite()->SetVisible(false);

		CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_MENU, this));
		CR(GetDOS()->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this));
		CR(GetDOS()->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_UP, this));

		float currentCenter = m_pControlView->GetBackgroundWidth() / 2.0f;
		float totalCenter = (m_pControlView->GetBackgroundWidth() + m_pDreamUserApp->GetSpacingSize() + m_pDreamTabView->GetBorderWidth()) / 2.0f;
		//m_centerOffset = currentCenter - totalCenter;
		
		pKeyboard->InitializeWithParent(this);
		GetComposite()->AddObject(std::shared_ptr<composite>(pKeyboard->GetComposite()));
	}

	if (m_pDreamUIBar != nullptr && m_fUpdateDreamUIBar) {
		CR(m_pDreamUIBar->ResetAppComposite());

		m_fUpdateDreamUIBar = false;
	}

	CNR(m_pDreamUserApp, R_SKIPPED);
	
	UIMallet* pLMallet;
	pLMallet = m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);

	UIMallet* pRMallet;
	pRMallet = m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_RIGHT);
	CNR(pRMallet, R_SKIPPED);	

	//m_pDreamUserApp->UpdateCompositeWithHands(-0.16f);
	//m_pDreamUserApp->GetAppBasisPosition(ptOrigin);
	//m_pDreamUserApp->GetAppBasisOrientation(qOrigin);

	//GetComposite()->SetOrientation(qOrigin);

	//TODO: change this to a UISurface
	for (int i = 0; i < 2; i++) {
		UIMallet *pMallet;
		HAND_TYPE type;
		if (i == 0) {
			pMallet = pLMallet;
			type = HAND_TYPE::HAND_LEFT;
		}
		else {
			pMallet = pRMallet;
			type = HAND_TYPE::HAND_RIGHT;
		}
		// Update using mallets, send relevant information to child apps
		auto pComposite = GetComposite();
		point ptBoxOrigin = pComposite->GetOrigin(true);
		point ptSphereOrigin = pMallet->GetMalletHead()->GetOrigin(true);
		ptSphereOrigin = (point)(inverse(RotationMatrix(pComposite->GetOrientation(true))) * (ptSphereOrigin - pComposite->GetOrigin(true)));

		// clear flags
		if (ptSphereOrigin.y() >= pMallet->GetRadius()) {
			m_fCanPressButton[i] = true;
		}

		// Update DreamTabView
		float tabViewWidth = m_pDreamTabView->GetBorderWidth();
		float tabViewHeight = m_pDreamTabView->GetBorderHeight();

		point ptDreamTabView = m_pDreamTabView->GetComposite()->GetPosition();

		bool fWidth = ( ptSphereOrigin.x() > ptDreamTabView.x() - tabViewWidth / 2.0f  &&
						ptSphereOrigin.x() < ptDreamTabView.x() + tabViewWidth / 2.0f);

		bool fHeight = (ptSphereOrigin.z() > ptDreamTabView.z() - tabViewHeight / 2.0f &&
						ptSphereOrigin.z() < ptDreamTabView.z() + tabViewHeight / 2.0f);

		bool fMalletInTabView = fWidth && fHeight;

		m_pDreamTabView->SetScrollFlag(fMalletInTabView, i);
		
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
	return point(0.0f, 0.0f, m_pDreamTabView->GetComposite()->GetPosition().z());
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
	m_pControlBar->Show();

	return r;
}

RESULT DreamUserControlArea::Hide() {
	RESULT r = R_PASS;

	//m_pControlView->GetComposite()->SetVisible(false);
	m_pControlView->Hide();
	//m_pDreamTabView->GetComposite()->SetVisible(false);
	m_pDreamTabView->Hide();
	//m_pControlBar->GetComposite()->SetVisible(false);
	m_pControlBar->Hide();

	return r;
}

RESULT DreamUserControlArea::HandleControlBarEvent(ControlEventType type) {
	RESULT r = R_PASS;

	switch (type) {

	case ControlEventType::BACK: {
		// Send back event to active browser
		auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
		CNR(pBrowser, R_SKIPPED);
		CR(pBrowser->HandleBackEvent());
	} break;

	case ControlEventType::FORWARD: {
		auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
		CNR(pBrowser, R_SKIPPED);
		CR(pBrowser->HandleForwardEvent());
	} break;

	case ControlEventType::OPEN: {
		// pull up menu to select new piece of content
		// send hide events to control bar, control view, and tab bar
		CR(Hide());
		CR(m_pDreamUIBar->ShowMenuLevel(MenuLevel::OPEN));
		CR(m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get()));
	} break;

	case ControlEventType::CLOSE: {
		auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
		CNR(m_pActiveSource, R_SKIPPED);	// double tapping close? 
//		long assetID = m_pActiveBrowser->Get
		CRM(m_pEnvironmentControllerProxy->RequestCloseAsset(m_pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");

	} break;

	case ControlEventType::MAXIMIZE: {
		//CR(Show());
		m_pControlView->Show();
		m_pDreamTabView->Show();
	} break;

	case ControlEventType::MINIMIZE: {
		//TODO: change this with animations, control bar needs to still be visible here
		//CR(Hide());
		m_pControlView->Hide();
		m_pDreamTabView->Hide();
		//m_pControlBar->GetComposite()->SetVisible(true);
		//m_pControlBar->Hide();
	} break;

	case ControlEventType::SHARE: {
		// send share event with active browser
		GetDOS()->SetSharedContentTexture(m_pActiveSource->GetSourceTexture());
		auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
		CRM(m_pEnvironmentControllerProxy->RequestShareAsset(m_pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
	} break;

	case ControlEventType::STOP: {
		// send stop sharing event 
		auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
		CRM(m_pEnvironmentControllerProxy->RequestStopSharing(m_pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
	} break;

	case ControlEventType::URL: {
		// dismiss everything(?) and pull up the keyboard
		Hide();
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		m_pControlView->ShowKeyboard();
		m_pControlView->m_fIsShareURL = true;
		/*
		CR(pDreamOS->GetInteractionEngineProxy()->ResetObjects(pButtonContext->GetInteractionObject()));
		CR(pDreamOS->GetInteractionEngineProxy()->ReleaseObjects(pButtonContext->GetInteractionObject()));

		if (m_pKeyboardHandle == nullptr) {
			CR(ShowKeyboard());
			CR(m_pKeyboardHandle->ShowTitleView());
		}

		CR(HideView());
		m_fIsShareURL = true;
		//*/
	} break;

	case ControlEventType::KEYBOARD: {
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		//float yValue = (BROWSER_WIDTH) + (BROWSER_HEIGHT * SPACING_SIZE);
		//HandleNodeFocusChanged(true, m_pActiveSource.get());
	}
	}

Error:
	return r;
}

bool DreamUserControlArea::CanPressButton(UIButton *pButtonContext) {
	RESULT r = R_PASS;

	auto pDreamOS = GetDOS();

	auto pInteractionObj = pButtonContext->GetInteractionObject();
	int dirtyIndex = -1;
	if (pInteractionObj == m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_LEFT)->GetMalletHead()) {
		dirtyIndex = 0;
	}
	else if (pInteractionObj == m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_RIGHT)->GetMalletHead()) {
		dirtyIndex = 1;
	}

	//CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pView.get()), R_SKIPPED);
	//CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pViewQuad.get()), R_SKIPPED);

	//only allow button presses while keyboard isn't active
	//CBR(m_pKeyboardHandle == nullptr, R_SKIPPED);	
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
	if (m_pControlBar != nullptr) {
		m_pControlBar->SetTitleText(m_pActiveSource->GetTitle());
		m_pControlBar->UpdateControlBarButtonsWithType(m_pActiveSource->GetContentType());
	}

	//m_pControlView->SetViewQuadTexture(m_pActiveSource->GetSourceTexture());

	//bool fIsSharing = (m_pActiveSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture());
	//m_pControlBar->SetSharingFlag(fIsSharing);

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

RESULT DreamUserControlArea::UpdateContentSourceTexture(std::shared_ptr<texture> pTexture, DreamContentSource* pContext) {
	if (pContext == m_pActiveSource.get()) {
		m_pControlView->SetViewQuadTexture(pTexture);
	}
	else {
		m_pDreamTabView->UpdateContentTexture(std::shared_ptr<DreamContentSource>(pContext));
	}
	return R_PASS;
}

RESULT DreamUserControlArea::UpdateControlBarText(std::string& strTitle) {
	RESULT r = R_PASS;

	CR(m_pControlBar->SetTitleText(strTitle));

Error:
	return r;
}

RESULT DreamUserControlArea::UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	CR(m_pControlBar->UpdateNavigationButtons(fCanGoBack, fCanGoForward));
	
Error:
	return r;
}

RESULT DreamUserControlArea::HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	// if the content source that received an audio packet is the active piece of content, 
	// send the audio packet to the shared view
	//if (pContext == m_pActiveSource.get()) {
		auto pCloudController = GetDOS()->GetCloudController();
		if (pCloudController != nullptr) {
			CR(GetDOS()->BroadcastSharedAudioPacket(pendingAudioPacket));
		}
	//}

Error:
	return r;
}

RESULT DreamUserControlArea::HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) {
	RESULT r = R_PASS;

	bool fMaskPasswordEnabled = false;

	UIKeyboard* pKeyboard = dynamic_cast<UIKeyboard*>(m_pDreamUserApp->GetKeyboard());
	CN(pKeyboard);

	CBR(pContext == m_pActiveSource.get(), R_SKIPPED);
	CN(pDOMNode);

	if (pDOMNode->GetType() == DOMNode::type::ELEMENT && pDOMNode->IsEditable() && !m_fKeyboardUp) {
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		fMaskPasswordEnabled = pDOMNode->IsPassword();

		CR(pKeyboard->ShowBrowserButtons());
		CR(m_pControlView->HandleKeyboardUp());
		CR(m_pControlBar->Hide());
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
	CBR(!m_pControlView->m_fIsShareURL, R_SKIPPED);

	if (fIsFocused) {
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pDreamUserApp->GetKeyboard());
		CN(pKeyboard);
		CR(pKeyboard->ShowBrowserButtons());
		CR(m_pControlView->HandleKeyboardUp());
		CR(m_pControlBar->Hide());
		CR(m_pDreamTabView->Hide());
		m_fKeyboardUp = true;
	}
	else {
		CR(HideWebsiteTyping());
	}

Error:
	return r;
}

bool DreamUserControlArea::IsContentVisible() {
	return true;
}

RESULT DreamUserControlArea::HandleDreamFormSuccess() {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamUserControlArea::HandleCanTabNext(bool fCanNext) {
	RESULT r = R_PASS;
	auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pDreamUserApp->GetKeyboard());
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabNextTexture(fCanNext));
Error:
	return r;
}

RESULT DreamUserControlArea::HandleCanTabPrevious(bool fCanPrevious) {
	RESULT r = R_PASS;
	auto pKeyboard = dynamic_cast<UIKeyboard*>(m_pDreamUserApp->GetKeyboard());
	CN(pKeyboard);
	CR(pKeyboard->UpdateTabPreviousTexture(fCanPrevious));
Error:
	return r;
}

RESULT DreamUserControlArea::OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) {
	RESULT r = R_PASS;

	CNR(m_pDreamDesktop, R_SKIPPED);
	m_pDreamDesktop->OnDesktopFrame(messageSize, pMessageData, pxHeight, pxWidth);

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

	if (m_pActiveSource != nullptr) {													// If content is already open
		if (strTitle == m_strDesktopTitle && m_pDreamDesktop != nullptr) {						// and we're trying to share the desktop for not the first time
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

	if (strTitle == m_strDesktopTitle) {
		if (m_pDreamDesktop == nullptr) {
			CRM(pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");

			m_pDreamDesktop = GetDOS()->LaunchDreamApp<DreamDesktopApp>(this);
			m_pActiveSource = m_pDreamDesktop;
			m_pDreamDesktop->InitializeWithParent(this);
			m_pControlBar->SetTitleText(m_pDreamDesktop->GetTitle());
			// new desktop can't be the current content
			m_pControlBar->SetSharingFlag(false);
		}
	}

	else {
		std::shared_ptr<DreamBrowser> pBrowser = nullptr;
		pBrowser = GetDOS()->LaunchDreamApp<DreamBrowser>(this);

		m_pActiveSource = pBrowser;

		pBrowser->SetScope(strScope);
		pBrowser->SetPath(m_strURL);

		CRM(pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");

		if (strTitle != "website") {
			UpdateControlBarText(strTitle);
		}
	}

Error:
	return r;
}

RESULT DreamUserControlArea::CreateBrowserSource(std::string strScope) {
	RESULT r = R_PASS;

	std::string strTitle = m_strWebsiteTitle;
	if (m_strURL == "") {
		CR(m_pDreamUIBar->HandleEvent(UserObserverEventType::BACK));
		m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get());
	}
	else {
		CR(RequestOpenAsset(strScope, m_strURL, strTitle));
	}
Error:
	return r;
}

RESULT DreamUserControlArea::SetActiveBrowserURI() {
	RESULT r = R_PASS;

	std::string strScope = "MenuProviderScope.WebsiteMenuProvider";
	std::string strTitle = m_strWebsiteTitle;

	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
	CNR(pBrowser, R_SKIPPED);

	CR(Show());

	EnvironmentControllerProxy* m_pEnvironmentControllerProxy;
	m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	CRM(m_pEnvironmentControllerProxy->RequestOpenAsset(strScope, m_strURL, strTitle), "Failed to share environment asset");

	m_strURL = "";

Error:
	return r;
}

RESULT DreamUserControlArea::HideWebsiteTyping() {
	RESULT r = R_PASS;

	if (m_fKeyboardUp) {
	//	CR(m_pDreamUserApp->GetKeyboard()->Hide());
		m_fKeyboardUp = false;
		CR(m_pControlView->HandleKeyboardDown());
		m_pControlBar->Show();
		m_pDreamTabView->Show();
		auto pBrowser = dynamic_cast<DreamBrowser*>(m_pActiveSource.get());
		CNR(pBrowser, R_SKIPPED);
		CR(pBrowser->HandleUnfocusEvent());
	}

Error:
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

RESULT DreamUserControlArea::AddEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;
	
	//TODO: multi-content
	
	//it is not safe to set the environment asset until after the browser is finished initializing
	// this is because LoadRequest requires a URL to have been set (about:blank in InitializeWithBrowserManager)
	m_fHasOpenApp = true;
	m_pDreamUserApp->SetHasOpenApp(true);
	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
	if (pBrowser != nullptr) {	
		
		pBrowser->InitializeWithBrowserManager(m_pDreamUserApp->GetBrowserManager(), pEnvironmentAsset->GetURL());
		pBrowser->RegisterObserver(this);
		//m_pControlBar->SetTitleText(pBrowser->GetTitle());

		// TODO: may not be enough once browser typing is re-enabled
		m_strURL = "";

		pBrowser->SetEnvironmentAsset(pEnvironmentAsset);
	}
	else {
		// TODO: desktop setup
		m_pDreamDesktop->SetEnvironmentAsset(pEnvironmentAsset);
	}

	//m_pActiveBrowser->SetEnvironmentAsset(pEnvironmentAsset);
	//m_pActiveBrowser->SetURI(pEnvironmentAsset->GetURL());
	//m_pControlView->SetControlViewTexture(m_pActiveBrowser->GetScreenTexture());

	CR(Show());

	m_pControlBar->UpdateControlBarButtonsWithType(pEnvironmentAsset->GetContentType());

Error:
	return r;
}

RESULT DreamUserControlArea::OnReceiveAsset() {
	RESULT r = R_PASS;
	
	// new browser can't be the current content
	m_pControlBar->SetSharingFlag(false);

	return r;
}

RESULT DreamUserControlArea::StartSharing() {
	RESULT r = R_PASS;

	CR(m_pActiveSource->SendFirstFrame());

Error:
	return r;
}

RESULT DreamUserControlArea::ShutdownSource() {
	RESULT r = R_PASS;

	CNR(m_pActiveSource, R_SKIPPED);
	m_pActiveSource->CloseSource();
	
	if (m_pDreamDesktop == m_pActiveSource) {
		GetDOS()->ShutdownDreamApp<DreamDesktopApp>(m_pDreamDesktop);
		m_pDreamDesktop = nullptr;
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

	m_pDreamUIBar->HandleEvent(UserObserverEventType::DISMISS);

	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	/*
	CNR(m_pActiveSource, R_SKIPPED);
	if (m_pActiveSource->GetSourceTexture().get() == GetDOS()->GetSharedContentTexture().get()) {
	//	CRM(m_pEnvironmentControllerProxy->RequestStopSharing(m_pActiveSource->GetCurrentAssetID()), "Failed to share environment asset");
		GetDOS()->OnStopSending();
	}
	else {
		for (auto pSource : m_pDreamTabView->GetAllSources()) {
			if (pSource->GetSourceTexture().get() == GetDOS()->GetSharedContentTexture().get()) {
			//	CRM(m_pEnvironmentControllerProxy->RequestStopSharing(pSource->GetCurrentAssetID()), "Failed to share environment asset");
				GetDOS()->OnStopSending();
			}
		}
	}
	//*/

	GetDOS()->OnStopSending();
	GetDOS()->OnStopReceiving();

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
		CR(ShutdownSource());
		m_pActiveSource = m_pDreamTabView->RemoveContent();
		//m_pControlView->GetViewQuad()->SetVisible(false);
		// replace with top of tab bar
		if (m_pActiveSource != nullptr) {
			m_pControlBar->SetTitleText(m_pActiveSource->GetTitle());
			m_pControlBar->UpdateControlBarButtonsWithType(m_pActiveSource->GetContentType());
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
		CR(m_pDreamUIBar->HandleEvent(UserObserverEventType::DISMISS));
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
		
		else if (m_fHasOpenApp && (m_pControlView->IsVisible() || m_pControlBar->IsVisible())) {	// Pressing Menu while we have content open or minimized content
			ResetAppComposite();
			Hide();
			m_pDreamUIBar->ShowMenuLevel(MenuLevel::ROOT);
			m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get());
		}

		else {	// Pressing back when Menu has a level saved
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
				if (chkey == SVK_RETURN && fKeyDown) {
					if (m_pControlView->m_fIsShareURL) {
						CR(SetActiveBrowserURI());
					}
				}
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
		else {
			//*/
			if (chkey == 0x01) {	// dupe filters from UIKeyboard to properly build URL based on what is in Keyboards textbox
				m_strURL = "";		// could be scraped if we exposed keyboards textbox and pulled it via a keyboard handle
			}

			else if (chkey == SVK_BACK) {
				if (m_strURL.size() > 0) {
					m_strURL.pop_back();
				}
			}
			
			else if (chkey != SVK_CONTROL && chkey != SVK_SHIFT && fKeyDown) { // control and shift keycodes were being added to URLs
				m_strURL += chkey;
			}
		}

	}
	}

Error:
	return r;
}

RESULT DreamUserControlArea::Notify(UIEvent *pUIEvent) {
	RESULT r = R_PASS;
	
	WebBrowserPoint wptContact;
	point ptContact;

	CNR(m_pActiveSource, R_SKIPPED);

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
		CR(OnScroll(pUIEvent->m_vDelta.x(), pUIEvent->m_vDelta.y(), ptContact));
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
