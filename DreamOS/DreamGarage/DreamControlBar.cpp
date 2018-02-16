#include "DreamControlBar.h"
#include "DreamOS.h"
#include "DreamUserControlArea/DreamUserControlArea.h"

#include "UI/UIButton.h"

DreamControlBar::DreamControlBar(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamControlBar>(pDreamOS)
{

}

DreamControlBar::~DreamControlBar() {
	// empty
}

// DreamApp
RESULT DreamControlBar::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	GetDOS()->AddObjectToUIGraph(GetComposite());

	m_pView = GetComposite()->AddUIView(GetDOS());
	CN(m_pView);

	m_pUIControlBar = m_pView->AddUIControlBar();
	CN(m_pUIControlBar);
	m_pUIControlBar->SetObserver(this);

	GetComposite()->SetVisible(true);
	//GetComposite()->SetPosition(0.0f, 0.5f, 4.6f);
	//GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(vector(60.0f * -(float)M_PI / 180.0f, 0.0f, 0.0f)));

Error:
	return r;
}

RESULT DreamControlBar::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
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

bool DreamControlBar::CanPressButton(int dirtyIndex) {
	//TODO: get some info from pParentApp, maintain some here
	//*
	RESULT r = R_PASS;
	auto pDreamOS = GetDOS();

	CBR(dirtyIndex != -1, R_SKIPPED);

	CBR(m_fCanPressButton[dirtyIndex], R_SKIPPED);

	// avoids pressing two control bar buttons at once
	m_fCanPressButton[0] = false;
	m_fCanPressButton[1] = false;

	return true;
Error:
	return false;
}

RESULT DreamControlBar::ClearFlag(int index) {
	RESULT r = R_PASS;

	CBR(index == 0 || index == 1, R_SKIPPED);
	m_fCanPressButton[index] = true;

Error:
	return r;
}

// ControlBarObserver
RESULT DreamControlBar::HandleBackPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(m_pParentApp->CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::BACK));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleForwardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(m_pParentApp->CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::FORWARD));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(m_pParentApp->CanPressButton(pButtonContext));

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
	CB(m_pParentApp->CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::OPEN));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleClosePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(m_pParentApp->CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::CLOSE));
Error:
	return R_PASS;
}

RESULT DreamControlBar::HandleShareTogglePressed(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;
	CB(m_pParentApp->CanPressButton(pButtonContext));

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
	CB(m_pParentApp->CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::URL));
Error:
	return R_PASS;
}

RESULT DreamControlBar::InitializeWithParent(DreamUserControlArea *pParentApp) {
	RESULT r = R_PASS;

	CN(pParentApp);
	CBM(m_pParentApp == nullptr, "parent app already set");
	m_pParentApp = pParentApp;

	float width = m_pParentApp->GetBaseWidth();
	float buttonWidth = m_buttonWidth * width;
	float spacingSize = m_pParentApp->GetSpacingSize() * width;

	m_pUIControlBar->SetTotalWidth(width);
	m_pUIControlBar->SetItemSide(m_buttonWidth * width);
	m_pUIControlBar->SetURLWidth(m_urlWidth * width);
	m_pUIControlBar->SetItemSpacing(m_pParentApp->GetSpacingSize() * width);

	CR(m_pUIControlBar->Initialize());

	GetComposite()->SetPosition(0.0f, 0.0f, m_pParentApp->GetBaseHeight() / 2.0f + 2*spacingSize + buttonWidth / 2.0f);
	GetComposite()->RotateXByDeg(-90.0f);

Error:
	return r;
}