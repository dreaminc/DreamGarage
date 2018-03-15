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
#include "Cloud/Environment/EnvironmentAsset.h"	

#include "InteractionEngine/InteractionObjectEvent.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UIButton.h"

DreamUserControlArea::DreamUserControlArea(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUserControlArea>(pDreamOS, pContext)
{
	// empty
}

DreamUserControlArea::~DreamUserControlArea() 
{

}

RESULT DreamUserControlArea::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	m_aspectRatio = ((float)m_pxWidth / (float)m_pxHeight);
	m_baseWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	m_baseHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));

	float viewAngleRad = VIEW_ANGLE * (float)(M_PI) / 180.0f;
	quaternion qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(viewAngleRad, 0.0f, 0.0f);
	point ptOrigin = point(0.0f, VIEW_POS_HEIGHT, VIEW_POS_DEPTH);
	
	GetComposite()->SetOrientation(qViewQuadOrientation);
	GetComposite()->SetPosition(ptOrigin);

	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

	//m_pActiveBrowser = GetDOS()->LaunchDreamApp<DreamBrowser>(this, false);
	//CN(m_pActiveBrowser);
	//CR(m_pActiveBrowser->InitializeWithBrowserManager(m_pWebBrowserManager));
	//CR(m_pActiveBrowser->SetURI("www.reddit.com")); // for testing

	m_pDreamUserApp = GetDOS()->LaunchDreamApp<DreamUserApp>(this, false);
	WCRM(m_pDreamUserApp->SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_LEFT)), "Warning: Failed to set left hand");
	WCRM(m_pDreamUserApp->SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_RIGHT)), "Warning: Failed to set right hand");
	CN(m_pDreamUserApp);

	m_pDreamUIBar = GetDOS()->LaunchDreamApp<DreamUIBar>(this, false);
	CN(m_pDreamUIBar);
	m_pDreamUIBar->InitializeWithParent(this);

	m_pControlBar = GetDOS()->LaunchDreamApp<DreamControlBar>(this, false);
	CN(m_pControlBar);
	m_pControlBar->InitializeWithParent(this);

	m_pControlView = GetDOS()->LaunchDreamApp<DreamControlView>(this, false);
	CN(m_pControlView);
	m_pControlView->InitializeWithParent(this);

	m_pDreamTabView = GetDOS()->LaunchDreamApp<DreamTabView>(this, false);
	CN(m_pDreamTabView);
	m_pDreamTabView->InitializeWithParent(this);

	//m_pActiveBrowser = GetDOS()->LaunchDreamApp<DreamBrowser>(this);
	//CN(m_pActiveBrowser);
	//m_pActiveBrowser->InitializeWithBrowserManager(m_pWebBrowserManager);

	// DreamUserApp can call Update Composite in certain situations and automatically update the other apps
	m_pDreamUserApp->GetComposite()->AddObject(std::shared_ptr<composite>(GetComposite()));
	//m_pDreamUserApp->GetComposite()->SetPosition(0.0f, 0.0f, 0.0f);

	//DreamUserControlArea is a friend of these classes to add the composite
	GetComposite()->AddObject(std::shared_ptr<composite>(m_pControlBar->GetComposite()));
	GetComposite()->AddObject(std::shared_ptr<composite>(m_pControlView->GetComposite()));
	GetComposite()->AddObject(std::shared_ptr<composite>(m_pDreamTabView->GetComposite()));

	m_pControlBar->GetComposite()->SetVisible(false);
	m_pDreamTabView->GetComposite()->SetVisible(false);
	m_pControlView->GetComposite()->SetVisible(false);

	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), INTERACTION_EVENT_MENU, this));
	CR(GetDOS()->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this));

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

	//CR(m_pWebBrowserManager->Update());
	CNR(m_pDreamUserApp, R_SKIPPED);
	UIMallet* pLMallet = m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);
	UIMallet* pRMallet = m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_RIGHT);
	CNR(pRMallet, R_SKIPPED);	

	//m_pDreamUserApp->UpdateCompositeWithHands(-0.16f);
	//m_pDreamUserApp->GetAppBasisPosition(ptOrigin);
	//m_pDreamUserApp->GetAppBasisOrientation(qOrigin);

	//GetComposite()->SetPosition(ptOrigin);
	//GetComposite()->SetOrientation(qOrigin);

	for (int i = 0; i < 2; i++)
	{
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

		// TODO: Update Control View

		
	}

Error:
	return r;
}

RESULT DreamUserControlArea::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	//CR(m_pWebBrowserManager->Shutdown());

//Error:
	return r;
}

DreamUserControlArea* DreamUserControlArea::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUserControlArea *pDreamApp = new DreamUserControlArea(pDreamOS, pContext);
	return pDreamApp;
}

float DreamUserControlArea::GetBaseWidth() {
	return m_baseWidth;
}

float DreamUserControlArea::GetBaseHeight() {
	return m_baseHeight;
}

float DreamUserControlArea::GetSpacingSize() {
	return m_spacingSize;
}

RESULT DreamUserControlArea::Show() {
	RESULT r = R_PASS;

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
		CR(m_pDreamUIBar->ShowRootMenu());
		CR(m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get()));
		CR(Hide());
	} break;

	case ControlEventType::CLOSE: {
		auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

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
		m_animationDuration,
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
		bool fIsSharing = (m_pActiveSource->GetSourceTexture() == GetDOS()->GetSharedContentTexture());
		m_pControlBar->SetSharingFlag(fIsSharing);
		
		CR(ShowControlView());

	Error:
		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pView.get(),
		pView->GetPosition(),
		pView->GetOrientation(),
		//vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
		m_animationScale,
		m_animationDuration,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamUserControlArea::UpdateTextureForBrowser(std::shared_ptr<texture> pTexture, DreamBrowser* pContext) {
	if (pContext == m_pActiveSource.get()) {
		m_pControlView->SetViewQuadTexture(pTexture);
	}
	else {
		m_pDreamTabView->UpdateContentTexture(std::shared_ptr<DreamContentSource>(pContext));
	}
	return R_PASS;
}

RESULT DreamUserControlArea::UpdateTextureForDesktop(std::shared_ptr<texture> pTexture, DreamDesktopApp* pContext) {
	if (pContext == m_pActiveSource.get()) {
		m_pControlView->SetViewQuadTexture(pTexture);
	}
	else {
		m_pDreamTabView->UpdateContentTexture(std::shared_ptr<DreamContentSource>(pContext));
	}
	return R_PASS;
}

RESULT DreamUserControlArea::UpdateControlBarText(std::string& strTitle) {
	return R_PASS;
}

RESULT DreamUserControlArea::ShowKeyboard(std::string strInitial, point ptTextBox) {
	RESULT r = R_PASS;

	m_fKeyboardUp = true;
	//CR(m_pDreamUserApp->GetKeyboard()->Show());
	point ptLastEvent = m_pControlView->GetLastEvent();
	if ((ptLastEvent.x() == -1 && ptLastEvent.y() == -1) ||
		(ptTextBox.x() == -1 && ptTextBox.y() == -1)) {
		OnClick(ptLastEvent, false);
		OnClick(ptLastEvent, true);
	}
	else {
		// TODO: this should probably be moved into the menu kb_enter
		m_pDreamUserApp->SetEventApp(m_pControlView.get());
		CR(m_pControlView->HandleKeyboardUp(strInitial, ptTextBox));
		CR(m_pControlBar->GetComposite()->SetVisible(false));
		CR(m_pControlBar->Hide());
	}

Error:
	return r;
}

bool DreamUserControlArea::IsContentVisible() {
	return true;
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
	CRM(pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");

	if (m_pActiveSource != nullptr) {													// If content is already open
		if (strTitle == m_strDesktopTitle && m_pDreamDesktop != nullptr) {						// and we're trying to share the desktop for not the first time
			if (m_pDreamDesktop != m_pActiveSource) {									// and desktop is in the tabview
				m_pDreamTabView->SelectByContent(m_pDreamDesktop);						// pull desktop out of tabview
			}	
		}
		else {
			m_pDreamTabView->AddContent(m_pActiveSource);
		}
		
	}

	if (strTitle == m_strDesktopTitle) {
		if (m_pDreamDesktop == nullptr) {
			m_pDreamDesktop = GetDOS()->LaunchDreamApp<DreamDesktopApp>(this);
			m_pDreamDesktop->InitializeWithParent(this);
			m_pActiveSource = m_pDreamDesktop;
			// new desktop can't be the current content
			m_pControlBar->SetSharingFlag(false);
		}
		else {
			//empty
		}
	}

	else {
		std::shared_ptr<DreamBrowser> pBrowser = nullptr;
		
		pBrowser = GetDOS()->LaunchDreamApp<DreamBrowser>(this);
		pBrowser->InitializeWithBrowserManager(m_pWebBrowserManager); // , m_strURL);
		pBrowser->InitializeWithParent(this);
		pBrowser->SetScope(strScope);
		pBrowser->SetPath(m_strURL);

		m_pActiveSource = pBrowser;

		// new browser can't be the current content
		m_pControlBar->SetSharingFlag(false);

		// TODO: may not be enough once browser typing is re-enabled
		m_strURL = "";
	}

Error:
	return r;
}

RESULT DreamUserControlArea::CreateBrowserSource() {
	RESULT r = R_PASS;

	std::string strScope = "WebsiteProviderScope.WebsiteProvider";
	std::string strTitle = m_strWebsiteTitle;


	CR(RequestOpenAsset(strScope, m_strURL, strTitle));



Error:
	return r;
}

RESULT DreamUserControlArea::SetActiveBrowserURI() {
	RESULT r = R_PASS;

	std::string strScope = "WebsiteProviderScope.WebsiteProvider";
	std::string strTitle = m_strWebsiteTitle;

	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
	CNR(pBrowser, R_SKIPPED);

	CR(Show());

	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	CRM(m_pEnvironmentControllerProxy->RequestOpenAsset(strScope, m_strURL, strTitle), "Failed to share environment asset");

	m_strURL = "";

Error:
	return r;
}

RESULT DreamUserControlArea::HideWebsiteTyping() {
	RESULT r = R_PASS;

	if (m_fKeyboardUp) {
		CR(m_pDreamUserApp->GetKeyboard()->Hide());
		CR(m_pControlView->HandleKeyboardDown());
		m_fKeyboardUp = false;
		CR(Show());
	}

Error:
	return r;
}

RESULT DreamUserControlArea::ResetAppComposite() {
	RESULT r = R_PASS;

	CR(m_pDreamUserApp->ResetAppComposite());
	CR(m_pDreamUIBar->ResetAppComposite());

Error:
	return r;
}

RESULT DreamUserControlArea::AddEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;
	
	//TODO: multi-content
	
	//it is not safe to set the environment asset until after the browser is finished initializing
	// this is because LoadRequest requires a URL to have been set (about:blank in InitializeWithBrowserManager)
	m_fHasOpenApp = true;
	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
	if (pBrowser != nullptr) {
		pBrowser->PendEnvironmentAsset(pEnvironmentAsset);
	}
	else {
		// TODO: desktop setup
		m_pDreamDesktop->SetEnvironmentAsset(pEnvironmentAsset);
	}

	//m_pActiveBrowser->SetEnvironmentAsset(pEnvironmentAsset);
	//m_pActiveBrowser->SetURI(pEnvironmentAsset->GetURL());
	//m_pControlView->SetControlViewTexture(m_pActiveBrowser->GetScreenTexture());

	CR(Show());

Error:
	return r;
}

RESULT DreamUserControlArea::OnReceiveAsset() {
	RESULT r = R_PASS;
	
	// new browser can't be the current content
	m_pControlBar->SetSharingFlag(false);

	return r;
}

RESULT DreamUserControlArea::ShutdownSource() {
	RESULT r = R_PASS;

	m_pActiveSource->CloseSource();
	
	if (m_pDreamDesktop == m_pActiveSource) {
		GetDOS()->ShutdownDreamApp<DreamDesktopApp>(m_pDreamDesktop);
		m_pDreamDesktop = nullptr;
	}
	else {	
		auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveSource);
		GetDOS()->ShutdownDreamApp<DreamBrowser>(pBrowser);
	}

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
			m_pControlView->SetViewQuadTexture(m_pActiveSource->GetSourceTexture());
			CR(ShowControlView());
		}
		else {
			m_pControlView->GetViewQuad()->SetVisible(false);
			Hide();
			m_fHasOpenApp = false;
			m_pDreamUserApp->SetHasOpenApp(m_fHasOpenApp);
			m_pDreamUserApp->SetEventApp(nullptr);
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
		m_animationDuration,
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
	m_pDreamUIBar->SetUIStageProgram(pUIProgramNode);
	return R_PASS;
}

float DreamUserControlArea::GetAnimationDuration() {
	return m_animationDuration;
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

RESULT DreamUserControlArea::Notify(InteractionObjectEvent *pSubscriberEvent) {
	RESULT r = R_PASS;

	switch (pSubscriberEvent->m_eventType) {
	case INTERACTION_EVENT_MENU: {
		CNR(m_pDreamUIBar, R_SKIPPED);

		if (m_fKeyboardUp) {
			HideWebsiteTyping();
		}
		else if (m_pDreamUIBar->IsEmpty()) {
			CR(m_pDreamUIBar->ShowRootMenu());
			m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get());

			ResetAppComposite();

			if (m_fHasOpenApp) {
				Hide();
			}
			else {
				m_pDreamUserApp->SetHasOpenApp(true);
			}
		}
		else {
			m_pDreamUIBar->HandleEvent(UserObserverEventType::BACK);
			if (m_pDreamUIBar->IsEmpty()) {
				if (m_fHasOpenApp) {
					Show();
				}
				else {
					m_pDreamUserApp->SetHasOpenApp(false);
					m_pDreamUserApp->SetEventApp(nullptr);
				}
			}
		}
	} break;

	case INTERACTION_EVENT_KEY_DOWN: {
		char chkey = (char)(pSubscriberEvent->m_value);

		CBR(chkey != 0x00, R_SKIPPED);	// To catch empty chars used to refresh textbox	

		//TODO: re-enable typing in the browser
		//*
		if (m_fKeyboardUp) {
			CBR(chkey != SVK_SHIFT, R_SKIPPED);		// don't send these key codes to browser (capital letters and such have different values already)
			CBR(chkey != 0, R_SKIPPED);
			CBR(chkey != SVK_CONTROL, R_SKIPPED);
			// CBR(chkey != SVK_RETURN, R_SKIPPED);		// might be necessary to prevent dupe returns being sent to browser.

			CR(m_pActiveSource->OnKeyPress(chkey, true));
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
			
			else {
				m_strURL += chkey;
			}
		}

	}
	}

Error:
	return r;
}
