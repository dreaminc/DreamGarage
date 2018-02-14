#include "DreamUserControlArea.h"

#include "DreamOS.h"
#include "DreamUserApp.h"
#include "DreamGarage/DreamBrowser.h"

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

	m_aspectRatio = ((float)1366 / (float)768);
	m_baseWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	m_baseHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));

	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

	m_pDreamUserApp = GetDOS()->LaunchDreamApp<DreamUserApp>(this, false);
	WCRM(m_pDreamUserApp->SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_LEFT)), "Warning: Failed to set left hand");
	WCRM(m_pDreamUserApp->SetHand(GetDOS()->GetHand(HAND_TYPE::HAND_RIGHT)), "Warning: Failed to set right hand");

	//CN(m_pDreamUserApp);


//	GetComposite()->SetPosition(0.0f, 1.25f, 4.6f);
	GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(vector(60.0f * -(float)M_PI / 180.0f, 0.0f, 0.0f)));

Error:
	return r;
}

RESULT DreamUserControlArea::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamUserControlArea::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_pControlBar == nullptr) {
		m_pControlBar = GetDOS()->LaunchDreamApp<DreamControlBar>(this);
		CN(m_pControlBar);
		m_pControlBar->InitializeWithParent(this);
	}

	//CR(m_pWebBrowserManager->Update());
	CNR(m_pDreamUserApp, R_SKIPPED);
	UIMallet* pLMallet = m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);
	UIMallet* pRMallet = m_pDreamUserApp->GetMallet(HAND_TYPE::HAND_RIGHT);
	CNR(pRMallet, R_SKIPPED);	

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

	CR(m_pDreamUserApp->RequestHapticImpulse(pButtonContext->GetInteractionObject()));

Error:
	return r;
}