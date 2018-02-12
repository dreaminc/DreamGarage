#include "DreamControlBar.h"
#include "DreamOS.h"
#include "DreamUserControlArea/DreamUserControlArea.h"

DreamControlBar::DreamControlBar(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamControlBar>(pDreamOS)
{

}

DreamControlBar::~DreamControlBar() {
	// empty
}

// DreamApp
RESULT DreamControlBar::InitializeApp(void *pContext) {
	return R_PASS;
}

RESULT DreamControlBar::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	m_pView = GetComposite()->AddUIView(GetDOS());
	CN(m_pView);

	m_pUIControlBar = m_pView->AddUIControlBar();
	CN(m_pUIControlBar);

Error:
	return r;
}

RESULT DreamControlBar::Update(void *pContext) {
	return R_PASS;
}

RESULT DreamControlBar::Shutdown(void *pContext) {
	return R_PASS;
}

DreamControlBar* DreamControlBar::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamControlBar *pDreamControlBar = new DreamControlBar(pDreamOS, pContext);
	return pDreamControlBar;
}

bool DreamControlBar::CanPressButton(UIButton *pButtonContext) {
	//TODO: get some info from pParentApp, maintain some here
	/*
	auto pDreamOS = GetDOS();

	auto pInteractionObj = pButtonContext->GetInteractionObject();
	int dirtyIndex = -1;
	if (pInteractionObj == m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT)->GetMalletHead()) {
		dirtyIndex = 0;
	}
	else if (pInteractionObj == m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT)->GetMalletHead()) {
		dirtyIndex = 1;
	}
	CBR(dirtyIndex != -1, R_SKIPPED);

	CBR(!m_fCanPressButton[dirtyIndex].IsDirty(), R_SKIPPED);

	CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pView.get()), R_SKIPPED);
	CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pViewQuad.get()), R_SKIPPED);

	// avoids pressing two control bar buttons at once
	CR(m_fCanPressButton[0].SetDirty());
	CR(m_fCanPressButton[1].SetDirty());

	//only allow button presses while keyboard isn't active
	CBR(m_pKeyboardHandle == nullptr, R_SKIPPED);

	CBR(m_pControlBar->IsVisible(), R_SKIPPED);

	CR(m_pUserHandle->RequestHapticImpulse(pButtonContext->GetInteractionObject()));

	return true;
Error:
	return false;
	//*/
	return true;
}

// ControlBarObserver
RESULT DreamControlBar::HandleBackPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::BACK));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleForwardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::FORWARD));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(CanPressButton(pButtonContext));

	if (m_fIsMinimized) {
		CR(m_pParentApp->HandleControlBarEvent(ControlEventType::MAXIMIZE));
		m_fIsMinimized = false;
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlEventType::MINIMIZE));
		m_fIsMinimized = true;
	}

Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleOpenPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::OPEN));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleClosePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::CLOSE));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleShareTogglePressed(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;
	CB(CanPressButton(pButtonContext));

	if (m_fIsMinimized) {
		CR(m_pParentApp->HandleControlBarEvent(ControlEventType::STOP));
		m_fIsSharing = false;
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlEventType::SHARE));
		m_fIsSharing = true;
	}

Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleURLPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::URL));
Error:
	return R_PASS;
}

RESULT DreamControlBar::InitializeWithParent(std::shared_ptr<DreamUserControlArea> pParentApp) {
	RESULT r = R_PASS;

	CN(pParentApp);
	CBM(m_pParentApp == nullptr, "parent app already set");
	m_pParentApp = pParentApp;

	float width = m_pParentApp->GetBaseWidth();
	m_pUIControlBar->SetTotalWidth(width);
	m_pUIControlBar->SetItemSide(m_buttonWidth * width);
	m_pUIControlBar->SetURLWidth(m_urlWidth * width);
	m_pUIControlBar->SetItemSpacing(m_pParentApp->GetSpacingSize() * width);

	CR(m_pUIControlBar->Initialize());

Error:
	return r;
}