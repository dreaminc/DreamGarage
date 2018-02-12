#include "DreamUserControlArea.h"

#include "DreamOS.h"
#include "DreamUserApp.h"
#include "DreamGarage/DreamBrowser.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	

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

	//m_pDreamUserApp = GetDOS()->LaunchDreamApp<DreamUserApp>(this);
	//CN(m_pDreamUserApp);
	m_aspectRatio = ((float)1366 / (float)768);
	m_baseWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	m_baseHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));

	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

Error:
	return r;
}

RESULT DreamUserControlArea::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamUserControlArea::Update(void *pContext) {
	RESULT r = R_PASS;

	//CR(m_pWebBrowserManager->Update());

//Error:
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