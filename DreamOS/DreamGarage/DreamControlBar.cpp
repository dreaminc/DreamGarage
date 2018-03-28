#include "DreamControlBar.h"
#include "DreamOS.h"
#include "DreamUserControlArea/DreamUserControlArea.h"

#include "UI/UIButton.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

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

	//GetComposite()->SetVisible(true);

Error:
	return r;
}

RESULT DreamControlBar::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamControlBar::Update(void *pContext) {

	if (m_fUpdateTitle) {
		m_pUIControlBar->GetURLText()->SetText(m_strUpdateTitle);
		m_pUIControlBar->GetURLText()->SetDirty();
		m_fUpdateTitle = false;
	}

	return R_PASS;
}

RESULT DreamControlBar::Shutdown(void *pContext) {
	return R_PASS;
}

DreamControlBar* DreamControlBar::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamControlBar *pDreamControlBar = new DreamControlBar(pDreamOS, pContext);
	return pDreamControlBar;
}

RESULT DreamControlBar::SetSharingFlag(bool fIsSharing) {
	RESULT r = R_PASS;
	m_fIsSharing = fIsSharing;

	if (m_fIsSharing) {
		m_pUIControlBar->GetShareButton()->GetSurface()->SetDiffuseTexture(m_pUIControlBar->GetStopTexture());
	}
	else {
		m_pUIControlBar->GetShareButton()->GetSurface()->SetDiffuseTexture(m_pUIControlBar->GetShareTexture());
	}

	return r;
}

RESULT DreamControlBar::SetTitleText(std::string& strTitle) {
	RESULT r = R_PASS;

	m_fUpdateTitle = true;
	m_strUpdateTitle = strTitle;

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
		m_pUIControlBar->GetToggleButton()->GetSurface()->SetDiffuseTexture(m_pUIControlBar->GetHideTexture());
		m_fIsMinimized = false;
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlEventType::MINIMIZE));
		m_fIsMinimized = true;
		m_pUIControlBar->GetToggleButton()->GetSurface()->SetDiffuseTexture(m_pUIControlBar->GetShowTexture());
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

	if (m_fIsSharing) {
		CR(m_pParentApp->HandleControlBarEvent(ControlEventType::STOP));
		m_pUIControlBar->GetShareButton()->GetSurface()->SetDiffuseTexture(m_pUIControlBar->GetShareTexture());
		m_fIsSharing = false;
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlEventType::SHARE));
		m_pUIControlBar->GetShareButton()->GetSurface()->SetDiffuseTexture(m_pUIControlBar->GetStopTexture());
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

RESULT DreamControlBar::HandleKeyboardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CB(m_pParentApp->CanPressButton(pButtonContext));
	CR(m_pParentApp->HandleControlBarEvent(ControlEventType::KEYBOARD));
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

RESULT DreamControlBar::UpdateControlBarButtonsWithType(std::string strContentType) {
	m_barType = m_pUIControlBar->ControlBarTypeFromString(strContentType);
	return (m_pUIControlBar->UpdateButtonsWithType(m_barType));
}

RESULT DreamControlBar::Show() {
	RESULT r = R_PASS;

	GetComposite()->SetVisible(true);
	m_pUIControlBar->UpdateButtonsWithType(m_barType);

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT DreamControlBar::Hide() {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		GetComposite()->SetVisible(false);

		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		color(1.0f, 1.0f, 1.0f, 0.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}