#include "UIContentControlBar.h"
#include "DreamOS.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamGarage/DreamBrowser.h"

#include "UI/UIButton.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "Primitives/font.h"

UIContentControlBar::UIContentControlBar(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIControlBar(pHALImp,pDreamOS)
{
	RESULT r = R_PASS;
}

UIContentControlBar::~UIContentControlBar() {
	// empty
}

// DreamApp
RESULT UIContentControlBar::Initialize(DreamUserControlArea *pParent) {
	RESULT r = R_PASS;

	m_pParentApp = pParent;

	// ui positioning	
	float width = m_pParentApp->GetBaseWidth();
	float buttonWidth = m_buttonWidth * width;
	float spacingSize = m_pParentApp->GetSpacingSize() * width;

	// button positioning
	float start = -width / 2.0f - spacingSize/2.0f;

	float backOffset = start + buttonWidth / 2.0f;
	float forwardOffset = backOffset + spacingSize + buttonWidth;
	float closeOffset = forwardOffset + spacingSize + buttonWidth;

	float urlOffset = 0.0f;
	float shareOffset = urlOffset + (m_urlWidth * width / 2.0f) + buttonWidth / 2.0f + spacingSize;
	float openOffset = shareOffset + spacingSize + buttonWidth;
	float hideOffset = openOffset + spacingSize + buttonWidth;

	//GetDOS()->AddObjectToUIGraph(GetComposite());

	SetTotalWidth(width);
	SetItemSide(m_buttonWidth * width);
	SetURLWidth(m_urlWidth * width);
	SetItemSpacing(m_pParentApp->GetSpacingSize() * width);

	SetPosition(0.0f, 0.0f, m_pParentApp->GetBaseHeight() / 2.0f + 2 * spacingSize + buttonWidth / 2.0f);
	RotateXByDeg(-90.0f);

	SetVisible(true);

	CR(InitializeGeneral());

	CR(AddButton(ControlBarButtonType::BACK, backOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleBackPressed, this, std::placeholders::_1, std::placeholders::_2)));
	CR(AddButton(ControlBarButtonType::FORWARD, forwardOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleForwardPressed, this, std::placeholders::_1, std::placeholders::_2)));
	CR(AddButton(ControlBarButtonType::CLOSE, closeOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleClosePressed, this, std::placeholders::_1, std::placeholders::_2)));
	CR(AddButton(ControlBarButtonType::URL, urlOffset, m_urlWidth * width, 
		std::bind(&UIContentControlBar::HandleURLPressed, this, std::placeholders::_1, std::placeholders::_2)));
	CR(AddButton(ControlBarButtonType::OPEN, openOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleOpenPressed, this, std::placeholders::_1, std::placeholders::_2)));
	CR(AddButton(ControlBarButtonType::SHARE, shareOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleShareTogglePressed, this, std::placeholders::_1, std::placeholders::_2)));
	CR(AddButton(ControlBarButtonType::MINIMIZE, hideOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleShowTogglePressed, this, std::placeholders::_1, std::placeholders::_2)));

	// create text for title
	{
		auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
		pFont->SetLineHeight(buttonWidth - (2.0f*spacingSize));

		auto textFlags = text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD;
		m_pURLText = std::shared_ptr<text>(m_pDreamOS->MakeText(pFont,
			"",
			0.84375 - spacingSize,
			buttonWidth - (2.0f*spacingSize),
			textFlags));

		m_pURLText->RotateXByDeg(90.0f);
		m_pURLText->SetPosition(point(0.0f, 0.0f, 0.001f));

		CR(GetButton(ControlBarButtonType::URL)->AddObject(m_pURLText));
	}

Error:
	return r;
}

RESULT UIContentControlBar::Update() {

	if (m_fUpdateTitle) {

		GetURLText()->SetText(m_strUpdateTitle);
		GetURLText()->SetDirty();
		m_fUpdateTitle = false;
	}

	return R_PASS;
}

RESULT UIContentControlBar::SetSharingFlag(bool fIsSharing) {
	RESULT r = R_PASS;
	m_fIsSharing = fIsSharing;
	
	std::shared_ptr<UIButton> pButton = GetButton(ControlBarButtonType::SHARE);
	CN(pButton);

	if (m_fIsSharing) {
		pButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::STOP));
	}
	else {
		pButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::SHARE));
	}

Error:
	return r;
}

RESULT UIContentControlBar::SetTitleText(const std::string& strTitle) {
	RESULT r = R_PASS;

	m_fUpdateTitle = true;
	m_strUpdateTitle = strTitle;

	return r;
}

// ControlBarObserver
RESULT UIContentControlBar::HandleBackPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::BACK));
Error:
	return R_PASS;
}

RESULT UIContentControlBar::HandleForwardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::FORWARD));
Error:
	return R_PASS;
}

RESULT UIContentControlBar::HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);

	if (m_fIsMinimized) {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::MAXIMIZE));
		CR(ClearMinimizedState());
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::MINIMIZE));
		m_fIsMinimized = true;

		GetButton(ControlBarButtonType::MINIMIZE)->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::MAXIMIZE));
	}

Error:
	return R_PASS;
}

RESULT UIContentControlBar::HandleOpenPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::OPEN));
	
	// This call triggers a move to menu, hiding the control bar, so the button needs to be forcefully released
	CR(m_pDreamOS->GetInteractionEngineProxy()->ResetObjects(pButtonContext->GetInteractionObject()));
	CR(m_pDreamOS->GetInteractionEngineProxy()->ReleaseObjects(pButtonContext->GetInteractionObject()));

Error:
	return R_PASS;
}

RESULT UIContentControlBar::HandleClosePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::CLOSE));
Error:
	return R_PASS;
}

RESULT UIContentControlBar::HandleShareTogglePressed(UIButton *pButtonContext, void *pContext) {
	RESULT r = R_PASS;
	
	std::shared_ptr<UIButton> pButton = GetButton(ControlBarButtonType::SHARE);

	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);

	if (m_fIsSharing) {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::SHARE));
		pButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::SHARE));
		m_fIsSharing = false;
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::STOP));
		pButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::STOP));
		m_fIsSharing = true;
	}

Error:
	return R_PASS;
}

RESULT UIContentControlBar::HandleURLPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::URL));
Error:
	return R_PASS;
}

RESULT UIContentControlBar::HandleKeyboardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::KEYBOARD));
Error:
	return R_PASS;
}

RESULT UIContentControlBar::UpdateControlBarButtonsWithType(std::string strContentType) {
	RESULT r = R_PASS;

	m_barType = ControlBarTypeFromString(strContentType);
	CR(UpdateButtonsWithType(m_barType));

	if (m_pParentApp != nullptr) {
		bool fIsSharing = (m_pParentApp->GetActiveSource()->GetSourceTexture().get() == m_pDreamOS->GetSharedContentTexture());
		fIsSharing = (fIsSharing && m_pDreamOS->IsSharing());

		CR(SetSharingFlag(fIsSharing));
		if (m_barType == BarType::BROWSER) {
			auto pBrowser = dynamic_cast<DreamBrowser*>(m_pParentApp->GetActiveSource().get());
			CR(pBrowser->UpdateNavigationFlags());
		}
	}

Error:
	return r;
}

RESULT UIContentControlBar::ClearMinimizedState() {
	RESULT r = R_PASS;

	auto pButton = GetButton(ControlBarButtonType::MINIMIZE);
	auto pTexture = GetTexture(ControlBarButtonType::MINIMIZE);
	CN(pButton);
	CN(pTexture);
	pButton->GetSurface()->SetDiffuseTexture(pTexture);
	m_fIsMinimized = false;

Error:
	return r;
}

RESULT UIContentControlBar::Show() {
	RESULT r = R_PASS;

	SetVisible(true, false);	
	CR(ClearMinimizedState());

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		this,
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT UIContentControlBar::Hide() {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		SetVisible(false, false);

		return r;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		this,
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