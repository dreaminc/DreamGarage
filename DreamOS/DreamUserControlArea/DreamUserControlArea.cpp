#include "DreamUserControlArea.h"
#include "DreamContent.h"

#include "DreamOS.h"
#include "DreamUserApp.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/DreamTabView.h"
#include "DreamControlView/DreamControlView.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	
#include "Cloud/Environment/EnvironmentAsset.h"	

#include "InteractionEngine/InteractionObjectEvent.h"

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

RESULT DreamUserControlArea::HandleControlBarEvent(ControlEventType type) {
	RESULT r = R_PASS;

	switch (type) {

	case ControlEventType::BACK: {
		// Send back event to active browser
		auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveContent);
		CNR(pBrowser, R_SKIPPED);
		CR(pBrowser->HandleBackEvent());
	} break;

	case ControlEventType::FORWARD: {
		auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveContent);
		CNR(pBrowser, R_SKIPPED);
		CR(pBrowser->HandleForwardEvent());
	} break;

	case ControlEventType::OPEN: {
		// pull up menu to select new piece of content
		// send hide events to control bar, control view, and tab bar
		CR(m_pDreamUIBar->ShowRootMenu());
		m_pControlBar->GetComposite()->SetVisible(false);
		m_pDreamTabView->GetComposite()->SetVisible(false);
		m_pControlView->GetComposite()->SetVisible(false);
	} break;

	case ControlEventType::CLOSE: {
		auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

//		long assetID = m_pActiveBrowser->Get
		CRM(m_pEnvironmentControllerProxy->RequestCloseAsset(m_pActiveContent->GetCurrentAssetID()), "Failed to share environment asset");

	} break;

	case ControlEventType::MAXIMIZE: {
		m_pDreamTabView->GetComposite()->SetVisible(true);
		m_pControlView->GetComposite()->SetVisible(true);
	} break;

	case ControlEventType::MINIMIZE: {
		m_pDreamTabView->GetComposite()->SetVisible(false);
		m_pControlView->GetComposite()->SetVisible(false);
	} break;

	case ControlEventType::SHARE: {
		// send share event with active browser
		auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
		CRM(m_pEnvironmentControllerProxy->RequestShareAsset(m_pActiveContent->GetCurrentAssetID()), "Failed to share environment asset");
	} break;

	case ControlEventType::STOP: {
		// send stop sharing event 
		auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");
		CRM(m_pEnvironmentControllerProxy->RequestStopSharing(m_pActiveContent->GetCurrentAssetID()), "Failed to share environment asset");
	} break;

	case ControlEventType::URL: {
		// dismiss everything(?) and pull up the keyboard
		/*
		auto pDreamOS = GetDOS();

		CBR(CanPressButton(pButtonContext), R_SKIPPED);

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

std::shared_ptr<DreamBrowser> DreamUserControlArea::GetActiveBrowser() {
	RESULT r = R_PASS;
	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveContent);
	CNR(pBrowser, R_SKIPPED);
	return pBrowser;
Error:
	return nullptr;
}

RESULT DreamUserControlArea::SetActiveBrowser(std::shared_ptr<DreamBrowser> pNewBrowser) {

	m_pActiveContent = pNewBrowser;
	m_pControlView->SetViewQuadTexture(m_pActiveContent->GetScreenTexture());

	bool fIsSharing = (m_pActiveContent->GetScreenTexture() == GetDOS()->GetSharedContentTexture());
	m_pControlBar->SetSharingFlag(fIsSharing);

	return R_PASS;
}

RESULT DreamUserControlArea::UpdateTextureForBrowser(std::shared_ptr<texture> pTexture, DreamBrowser* pContext) {
	if (pContext == m_pActiveContent.get()) {
		m_pControlView->SetViewQuadTexture(pTexture);
	}
	else {
		m_pDreamTabView->UpdateBrowserTexture(std::shared_ptr<DreamBrowser>(pContext));
	}
	return R_PASS;
}

RESULT DreamUserControlArea::UpdateControlBarText(std::string& strTitle) {
	return R_PASS;
}

RESULT DreamUserControlArea::ShowKeyboard(std::string strInitial, point ptTextBox) {
	return R_PASS;
}

bool DreamUserControlArea::IsContentVisible() {
	return true;
}

int DreamUserControlArea::GetPXWidth() {
	return m_pxWidth;
}

int DreamUserControlArea::GetPXHeight() {
	return m_pxHeight;
}

RESULT DreamUserControlArea::SendContactAtPoint(WebBrowserPoint ptContact, bool fMouseDown) {
	RESULT r = R_PASS;

	CNR(m_pActiveContent, R_SKIPPED);
	CR(m_pActiveContent->ClickContent(ptContact, fMouseDown));

Error:
	return r;
}

RESULT DreamUserControlArea::SendKeyCharacter(char chkey, bool fKeyDown) {
	RESULT r = R_PASS;

	CNR(m_pActiveContent, R_SKIPPED);
	CR(m_pActiveContent->SendKeyPressed(chkey, fKeyDown));

Error:
	return r;
}

RESULT DreamUserControlArea::SendMalletMoveEvent(WebBrowserPoint mousePoint) {
	RESULT r = R_PASS;

	CNR(m_pActiveContent, R_SKIPPED);
	CR(m_pActiveContent->SendMouseMoveEvent(mousePoint));

Error:
	return r;
}

RESULT DreamUserControlArea::ScrollByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint) {
	RESULT r = R_PASS;

	CNR(m_pActiveContent, R_OBJECT_NOT_FOUND);
	CR(m_pActiveContent->ScrollContentByDiff(pxXDiff, pxYDiff, scrollPoint));

Error:
	return r;
}

RESULT DreamUserControlArea::SetScope(std::string strScope) {
	RESULT r = R_PASS;
	
	CNR(m_pActiveContent, R_SKIPPED);
	CR(m_pActiveContent->SetScope(strScope));
	
Error:
	return r;
}

RESULT DreamUserControlArea::SetPath(std::string strPath) {
	RESULT r = R_PASS;
	
	CNR(m_pActiveContent, R_SKIPPED);
	CR(m_pActiveContent->SetPath(strPath));
	
Error:
	return r;
}

RESULT DreamUserControlArea::RequestOpenAsset(std::string strScope, std::string strPath, std::string strTitle) {
	RESULT r = R_PASS;

	std::shared_ptr<DreamBrowser> pBrowser = nullptr;
	auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

	if (m_pActiveContent != nullptr) {
		auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveContent);
		CNR(pBrowser, R_SKIPPED);
		m_pDreamTabView->AddBrowser(pBrowser);
	}

	CRM(m_pEnvironmentControllerProxy->RequestOpenAsset(strScope, strPath, strTitle), "Failed to share environment asset");

	pBrowser = GetDOS()->LaunchDreamApp<DreamBrowser>(this);
	pBrowser->InitializeWithBrowserManager(m_pWebBrowserManager); // , m_strURL);
	pBrowser->InitializeWithParent(this);
	pBrowser->SetScope(strScope);
	pBrowser->SetPath(m_strURL);

	m_pActiveContent = pBrowser;
	
	// new browser can't be the current content
	m_pControlBar->SetSharingFlag(false);

	// TODO: may not be enough once browser typing is re-enabled
	m_strURL = "";

Error:
	return r;
}

RESULT DreamUserControlArea::SendURL() {
	RESULT r = R_PASS;

	std::string strScope = "WebsiteProviderScope.WebsiteProvider";
	std::string strTitle = "website";


	CR(RequestOpenAsset(strScope, m_strURL, strTitle));



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
	//TODO: multi-content
	
	//it is not safe to set the environment asset until after the browser is finished initializing
	// this is because LoadRequest requires a URL to have been set (about:blank in InitializeWithBrowserManager)
	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveContent);
	if (pBrowser != nullptr) {
		pBrowser->PendEnvironmentAsset(pEnvironmentAsset);
	}
	else {
		// TODO: desktop setup
	}

	//m_pActiveBrowser->SetEnvironmentAsset(pEnvironmentAsset);
	//m_pActiveBrowser->SetURI(pEnvironmentAsset->GetURL());
	//m_pControlView->SetControlViewTexture(m_pActiveBrowser->GetScreenTexture());

	//m_pControlView->Show();
	m_pControlView->GetComposite()->SetVisible(true);
	//m_pControlView->
	m_pControlBar->GetComposite()->SetVisible(true);
	m_pDreamTabView->GetComposite()->SetVisible(true);

	return R_PASS;
}

RESULT DreamUserControlArea::CloseActiveAsset() {
	RESULT r = R_PASS;

	// close active browser

	m_pActiveContent->CloseContent();

	auto pBrowser = std::dynamic_pointer_cast<DreamBrowser>(m_pActiveContent);
	if (pBrowser != nullptr) {
		GetDOS()->ShutdownDreamApp<DreamBrowser>(pBrowser);
	}
	else {
		// TODO: desktop shutdown
	}

	// replace with top of tab bar
	m_pActiveContent = m_pDreamTabView->RemoveBrowser();
	if (m_pActiveContent != nullptr) {
		m_pControlView->SetViewQuadTexture(m_pActiveContent->GetScreenTexture());
	}
	else {
	//	m_pControlView->SetViewQuadTexture(m_p)
		m_pControlBar->GetComposite()->SetVisible(false);
		m_pDreamTabView->GetComposite()->SetVisible(false);
		m_pControlView->GetComposite()->SetVisible(false);
		m_fHasOpenApp = false;
		m_pDreamUserApp->SetHasOpenApp(m_fHasOpenApp);
		m_pDreamUserApp->SetEventApp(nullptr);
	}

	return r;
}

RESULT DreamUserControlArea::SetUIProgramNode(UIStageProgram *pUIProgramNode) {
	m_pDreamUIBar->SetUIStageProgram(pUIProgramNode);
	return R_PASS;
}

RESULT DreamUserControlArea::Notify(InteractionObjectEvent *pSubscriberEvent) {
	RESULT r = R_PASS;

	switch (pSubscriberEvent->m_eventType) {
	case INTERACTION_EVENT_MENU: {
		CNR(m_pDreamUIBar, R_SKIPPED);
		if (m_pDreamUIBar->IsEmpty()) {
			CR(m_pDreamUIBar->ShowRootMenu());

			ResetAppComposite();

			m_pDreamUserApp->SetHasOpenApp(true);
			m_pDreamUserApp->SetEventApp(m_pDreamUIBar.get());
		}
		else {
			m_pDreamUIBar->HandleEvent(UserObserverEventType::BACK);
			if (m_pDreamUIBar->IsEmpty()) {
				m_pDreamUserApp->SetHasOpenApp(m_fHasOpenApp);
				m_pDreamUserApp->SetEventApp(nullptr);
			}
			m_pControlBar->GetComposite()->SetVisible(false);
			m_pDreamTabView->GetComposite()->SetVisible(false);
			m_pControlView->GetComposite()->SetVisible(false);
		}
	} break;

	case INTERACTION_EVENT_KEY_DOWN: {
		char chkey = (char)(pSubscriberEvent->m_value);

		CBR(chkey != 0x00, R_SKIPPED);	// To catch empty chars used to refresh textbox	

		//TODO: re-enable typing in the browser
		/*
		if (m_pKeyboardHandle != nullptr && !m_fIsShareURL) {
			CBR(chkey != SVK_SHIFT, R_SKIPPED);		// don't send these key codes to browser (capital letters and such have different values already)
			CBR(chkey != 0, R_SKIPPED);
			CBR(chkey != SVK_CONTROL, R_SKIPPED);
			// CBR(chkey != SVK_RETURN, R_SKIPPED);		// might be necessary to prevent dupe returns being sent to browser.

			CR(SendKeyCharacter(chkey, true));
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
		//}

	}
	}

Error:
	return r;
}
