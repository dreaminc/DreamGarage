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

	m_urlWidth *= width;

	SetPosition(0.0f, 0.0f, m_pParentApp->GetBaseHeight() / 2.0f + 2 * spacingSize + buttonWidth / 2.0f);
	RotateXByDeg(-90.0f);

	SetVisible(true);

	// this can be local, because all values are saved to specific buttons
	std::vector<std::shared_ptr<texture>> controlTextures;
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBack));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBackDisabled));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForward));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForwardDisabled));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszClose));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszURL));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszOpen));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszStopSharing));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShare));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszHide));
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShow));

	// TODO: desktop logic
	controlTextures.emplace_back(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszKeyboard));

	for (auto pTexture : controlTextures) {
		CN(pTexture);
	}

	CR(UIControlBar::Initialize());

	m_pBackButton = AddButton(ControlBarButtonType::BACK, backOffset, buttonWidth,
		std::bind(&UIContentControlBar::HandleBackPressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[0], controlTextures[1]);

	m_pForwardButton = AddButton(ControlBarButtonType::FORWARD, forwardOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleForwardPressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[2], controlTextures[3]);

	m_pCloseButton = AddButton(ControlBarButtonType::CLOSE, closeOffset, buttonWidth,
		std::bind(&UIContentControlBar::HandleClosePressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[4]);

// Re-enable for selectability of the URL button
//	CR(AddButton(ControlBarButtonType::URL, urlOffset, m_urlWidth * width, 
//		std::bind(&UIContentControlBar::HandleURLPressed, this, std::placeholders::_1, std::placeholders::_2)));

	m_pURLButton = AddButton(ControlBarButtonType::URL, urlOffset, m_urlWidth, nullptr, controlTextures[5]);

	m_pOpenButton = AddButton(ControlBarButtonType::OPEN, openOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleOpenPressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[6]);

	m_pShareButton = AddButton(ControlBarButtonType::SHARE, shareOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleShareTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[7], controlTextures[8]);

	m_pMinimizeButton = AddButton(ControlBarButtonType::MINIMIZE, hideOffset, buttonWidth, 
		std::bind(&UIContentControlBar::HandleShowTogglePressed, this, std::placeholders::_1, std::placeholders::_2),
		controlTextures[9], controlTextures[10]);

	CN(m_pBackButton);
	CN(m_pForwardButton);
	CN(m_pCloseButton);
	CN(m_pURLButton);
	CN(m_pOpenButton);
	CN(m_pShareButton);
	CN(m_pMinimizeButton);

	// create text for title
	CR(InitializeText());

Error:
	return r;
}

RESULT UIContentControlBar::InitializeText() {
	RESULT r = R_PASS;

	auto pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	pFont->SetLineHeight(m_itemSide - (2.0f*m_itemSpacing));

	auto textFlags = text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD;
	m_pURLText = std::shared_ptr<text>(m_pDreamOS->MakeText(pFont,
		"",
		m_urlWidth - m_itemSpacing,
		m_itemSide - (2.0f*m_itemSpacing),
		textFlags));

	m_pURLText->RotateXByDeg(90.0f);
	m_pURLText->SetPosition(point(0.0f, 0.0f, 0.001f));
	m_pURLButton->AddObject(m_pURLText);

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
	
	m_pShareButton->SwitchToTexture(fIsSharing);

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
	if (!m_pMinimizeButton->IsToggled()) {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::MAXIMIZE));
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::MINIMIZE));
	}

	CR(m_pMinimizeButton->Toggle());

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
	
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);

	if (m_pShareButton->IsToggled()) {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::STOP));
	}
	else {
		CR(m_pParentApp->HandleControlBarEvent(ControlBarButtonType::SHARE));
	}

	m_pShareButton->Toggle();

Error:
	return r;
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

	// TODO: if source is desktop, switch around buttons
	// doing things like settings visibility was removed from this function in UIControlBar

	if (m_pParentApp != nullptr) {
		bool fIsSharing = (m_pParentApp->GetActiveSource()->GetSourceTexture().get() == m_pDreamOS->GetSharedContentTexture());
		fIsSharing = (fIsSharing && m_pDreamOS->IsSharing());

		CR(SetSharingFlag(fIsSharing));

		auto pBrowser = dynamic_cast<DreamBrowser*>(m_pParentApp->GetActiveSource().get());
		if (pBrowser != nullptr) {
			CR(pBrowser->UpdateNavigationFlags());
		}
	}

Error:
	return r;
}

RESULT UIContentControlBar::UpdateNavigationButtons(bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	CR(m_pBackButton->SetInteractability(fCanGoBack));
	CR(m_pForwardButton->SetInteractability(fCanGoForward));

Error:
	return r;
}

std::shared_ptr<text> UIContentControlBar::GetURLText() {
	return m_pURLText;
}

RESULT UIContentControlBar::Show() {
	RESULT r = R_PASS;

	SetVisible(true, false);	

	m_pMinimizeButton->SwitchToTexture(true);

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