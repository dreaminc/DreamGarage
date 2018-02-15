#include "DreamUserControlArea.h"

#include "DreamOS.h"
#include "DreamUserApp.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamControlView/DreamControlView.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	

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

	point ptOrigin;
	quaternion qOrigin;

	m_aspectRatio = ((float)m_pxWidth / (float)m_pxHeight);
	m_baseWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	m_baseHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));

	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

	m_pActiveBrowser = GetDOS()->LaunchDreamApp<DreamBrowser>(this, false);
	CN(m_pActiveBrowser);
	CR(m_pActiveBrowser->InitializeWithBrowserManager(m_pWebBrowserManager));

	m_pDreamUserApp = GetDOS()->LaunchDreamApp<DreamUserApp>(this, false);
	WCRM(m_pDreamUserApp->SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_LEFT)), "Warning: Failed to set left hand");
	WCRM(m_pDreamUserApp->SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_RIGHT)), "Warning: Failed to set right hand");
	CN(m_pDreamUserApp);

	m_pControlBar = GetDOS()->LaunchDreamApp<DreamControlBar>(this, false);
	CN(m_pControlBar);
	m_pControlBar->InitializeWithParent(this);

	m_pControlView = GetDOS()->LaunchDreamApp<DreamControlView>(this, false);
	CN(m_pControlView);
	m_pControlView->InitializeWithParent(this);
	m_pControlView->m_pViewQuad->SetVisible(true);

	// DreamUserApp can call Update Composite in certain situations and automatically update the other apps
	//m_pDreamUserApp->GetComposite()->AddObject(std::shared_ptr<composite>(GetComposite()));
	//m_pDreamUserApp->GetComposite()->SetPosition(0.0f, 0.0f, 0.0f);
	GetComposite()->AddObject(std::shared_ptr<composite>(m_pControlBar->GetComposite()));
	GetComposite()->AddObject(std::shared_ptr<composite>(m_pControlView->GetComposite()));

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
	m_pDreamUserApp->GetAppBasisPosition(ptOrigin);
	m_pDreamUserApp->GetAppBasisOrientation(qOrigin);

	GetComposite()->SetPosition(ptOrigin);
	GetComposite()->SetOrientation(qOrigin);

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
		pMallet->GetMalletHead()->SetVisible(true);
		// Update using mallets, send relevant information to child apps
		auto pComposite = GetComposite();
		point ptBoxOrigin = pComposite->GetOrigin(true);
		point ptSphereOrigin = pMallet->GetMalletHead()->GetOrigin(true);
		ptSphereOrigin = (point)(inverse(RotationMatrix(pComposite->GetOrientation(true))) * (ptSphereOrigin - pComposite->GetOrigin(true)));

		// clear flags
		if (ptSphereOrigin.y() >= pMallet->GetRadius()) {
			m_pControlBar->ClearFlag(i);
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
		//CR(m_pBrowserHandle->SendBackEvent());
	} break;

	case ControlEventType::FORWARD: {
		// Send forward event to active browser
		//CR(m_pBrowserHandle->SendForwardEvent());
	} break;

	case ControlEventType::OPEN: {
		// pull up menu to select new piece of content
		// send hide events to control bar, control view, and tab bar
	} break;

	case ControlEventType::CLOSE: {
		// if active browser matches shared browser, send stop event
		// close active browser
		// replace with top of tab bar
		// update tab bar
		/*
		DreamShareViewHandle *pShareViewHandle = nullptr;
		pShareViewHandle = dynamic_cast<DreamShareViewHandle*>(GetDOS()->RequestCaptureAppUnique("DreamShareView", this));

		CBR(CanPressButton(pButtonContext), R_SKIPPED);
		CBR(!IsAnimating(), R_SKIPPED);
		pShareViewHandle->SendStopEvent();

		CN(m_pUserHandle);
		CR(m_pUserHandle->SendClearFocusStack());
		CR(Hide());

	Error:
		GetDOS()->RequestReleaseAppUnique(pShareViewHandle, this);
		//*/
	} break;

	case ControlEventType::MAXIMIZE: {
		// send show events to control view and tab bar
		/*
		auto fnStartCallback = [&](void *pContext) {
			GetViewQuad()->SetVisible(true);
			return R_PASS;
		};

		auto fnEndCallback = [&](void *pContext) {
			RESULT r = R_PASS;
			
			m_fMouseDown[0] = false;
			m_fMouseDown[1] = false;

			SetIsMinimizedFlag(false);
//		Error:
			return r;
		};

		CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
			m_pViewQuad.get(),
			m_pViewQuad->GetPosition(),
			m_pViewQuad->GetOrientation(),
			vector(m_visibleScale, m_visibleScale, m_visibleScale),
			0.1f,
			AnimationCurveType::SIGMOID,
			AnimationFlags(),
			fnStartCallback,
			fnEndCallback,
			this
		));
		//*/
	} break;

	case ControlEventType::MINIMIZE: {
		// send hide events to control view and tab bar
		/*
		auto fnStartCallback = [&](void *pContext) {
			return R_PASS;
		};

		auto fnEndCallback = [&](void *pContext) {
			GetViewQuad()->SetVisible(false);
			SetIsMinimizedFlag(true);
			m_strURL = "";
			return R_PASS;
		};

		CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
			m_pViewQuad.get(),
			m_pViewQuad->GetPosition(),
			m_pViewQuad->GetOrientation(),
			vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
			0.1f,
			AnimationCurveType::SIGMOID,
			AnimationFlags(),
			fnStartCallback,
			fnEndCallback,
			this
		));
		//*/
	} break;

	case ControlEventType::SHARE: {
		// send share event with active browser
	} break;

	case ControlEventType::STOP: {
		// send stop sharing event 
	} break;

	case ControlEventType::URL: {
		// dismiss everyting(?) and pull up the keyboard
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

	return r;
}

RESULT DreamUserControlArea::CanPressButton(UIButton *pButtonContext) {
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

	CBR(m_pControlBar->CanPressButton(dirtyIndex), R_SKIPPED);

//	CBR(m_pControlBar->IsVisible(), R_SKIPPED);

	CR(m_pDreamUserApp->CreateHapticImpulse(pButtonContext->GetInteractionObject()));

Error:
	return r;
}

int DreamUserControlArea::GetPXWidth() {
	return m_pxWidth;
}

int DreamUserControlArea::GetPXHeight() {
	return m_pxHeight;
}

RESULT DreamUserControlArea::SendContactAtPoint(WebBrowserPoint ptContact, bool fMouseDown) {
	RESULT r = R_PASS;

	CR(m_pActiveBrowser->ClickBrowser(ptContact, fMouseDown));

Error:
	return r;
}

RESULT DreamUserControlArea::SendKeyCharacter(char chkey, bool fKeyDown) {
	RESULT r = R_PASS;

	CR(m_pActiveBrowser->SendKeyPressed(chkey, fKeyDown));

Error:
	return r;
}

RESULT DreamUserControlArea::SendMalletMoveEvent(WebBrowserPoint mousePoint) {
	RESULT r = R_PASS;

	CR(m_pActiveBrowser->SendMouseMoveEvent(mousePoint));

Error:
	return r;
}

RESULT DreamUserControlArea::ScrollByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint) {
	RESULT r = R_PASS;

	CR(m_pActiveBrowser->ScrollBrowserByDiff(pxXDiff, pxYDiff, scrollPoint));

Error:
	return r;
}

RESULT DreamUserControlArea::SetScope(std::string strScope) {
	RESULT r = R_PASS;
	
	CR(m_pActiveBrowser->SetBrowserScope(strScope));
	
Error:
	return r;
}

RESULT DreamUserControlArea::SetPath(std::string strPath) {
	RESULT r = R_PASS;
	
	CR(m_pActiveBrowser->SetBrowserPath(strPath));
	
Error:
	return r;
}

RESULT DreamUserControlArea::SendURL(std::string strURL) {
	RESULT r = R_PASS;

	CR(m_pActiveBrowser->SendURL(strURL));

Error:
	return r;
}
