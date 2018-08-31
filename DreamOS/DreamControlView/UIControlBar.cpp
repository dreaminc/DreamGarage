#include "UIControlBar.h"
#include "DreamOS.h"
#include "UI/UIButton.h"
#include "DreamUserControlArea/DreamContentSource.h"
#include "Primitives/text.h"
#include "Primitives/font.h"

UIControlBar::UIControlBar(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp,pDreamOS)
{
	RESULT r = R_PASS;

	/*
	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
//*/
}

UIControlBar::~UIControlBar() {
	// empty
}

RESULT UIControlBar::Initialize() {
	RESULT r = R_PASS;

	CN(m_pDreamOS);
	m_pDreamOS->AddObjectToInteractionGraph(this);

	// create textures 
	m_pBackTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBack);
	m_pForwardTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForward);
	m_pBackDisabledTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBackDisabled);
	m_pForwardDisabledTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForwardDisabled);
	m_pHideTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszHide);
	m_pShowTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShow);
	m_pOpenTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszOpen);
	m_pCloseTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszClose);
	m_pURLTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszURL);
	m_pShareTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShare);
	m_pStopSharingTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszStopSharing);
	m_pKeyboardTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszKeyboard);
	m_pTabTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszTab);
	m_pCantTabTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszCantTab);
	m_pBackTabTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBackTab);
	m_pCantBackTabTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszCantBackTab);
	m_pDoneTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszDone);

	// create buttons

	m_pBackButton = AddUIButton(m_itemSide, m_itemSide);
	m_pBackButton->GetSurface()->SetDiffuseTexture(m_pBackTexture);

	m_pForwardButton = AddUIButton(m_itemSide, m_itemSide);
	m_pForwardButton->GetSurface()->SetDiffuseTexture(m_pForwardTexture);

	m_pKeyboardButton = AddUIButton((2.0f * m_itemSide + m_itemSpacing), m_itemSide);
	m_pKeyboardButton->GetSurface()->SetDiffuseTexture(m_pKeyboardTexture);

	m_pToggleButton = AddUIButton(m_itemSide, m_itemSide);
	m_pToggleButton->GetSurface()->SetDiffuseTexture(m_pHideTexture);

	m_pOpenButton = AddUIButton(m_itemSide, m_itemSide);
	m_pOpenButton->GetSurface()->SetDiffuseTexture(m_pOpenTexture);

	m_pCloseButton = AddUIButton(m_itemSide, m_itemSide);
	m_pCloseButton->GetSurface()->SetDiffuseTexture(m_pCloseTexture);

	m_pShareToggleButton = AddUIButton(m_itemSide, m_itemSide);
	m_pShareToggleButton->GetSurface()->SetDiffuseTexture(m_pShareTexture);

	m_pTabButton = AddUIButton(m_itemSide*2.0f + m_itemSpacing, m_itemSide);
	m_pTabButton->GetSurface()->SetDiffuseTexture(m_pTabTexture);

	m_pBackTabButton = AddUIButton(m_itemSide*2.0f + m_itemSpacing, m_itemSide);
	m_pBackTabButton->GetSurface()->SetDiffuseTexture(m_pBackTabTexture);

	m_pDoneButton = AddUIButton(m_itemSide*2.0f + m_itemSpacing, m_itemSide);
	m_pDoneButton->GetSurface()->SetDiffuseTexture(m_pDoneTexture);

	m_pURLButton = AddUIButton(m_urlWidth, m_itemSide);
	m_pURLButton->GetSurface()->SetDiffuseTexture(m_pURLTexture);

	// register all of the buttons (except for the URL button) for the selection event
	// URL button has become less useful with the addition of the open button
	for (auto pButton : { m_pBackButton, m_pForwardButton, m_pKeyboardButton, m_pToggleButton, m_pCloseButton, m_pOpenButton, m_pShareToggleButton, m_pTabButton, m_pBackTabButton, m_pDoneButton/*, m_pURLButton*/ }) {
		CN(pButton);
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));

		CR(pButton->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
			std::bind(&UIControlBar::HandleTouchStart, this, std::placeholders::_1, std::placeholders::_2)));
	}

	// Set-up text for url/title
	{
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
	}

	CR(UpdateButtonsWithType(m_barType));

	// Register wrapper functions to button events
	{
		auto fnOpenCallback = [&](UIButton *pButtonContext, void *pContext) {
			return OpenPressed(pButtonContext, pContext);
		};
		auto fnCloseCallback = [&](UIButton *pButtonContext, void *pContext) {
			return ClosePressed(pButtonContext, pContext);
		};
		auto fnToggleCallback = [&](UIButton *pButtonContext, void *pContext) {
			return TogglePressed(pButtonContext, pContext);
		};
		auto fnShareCallback = [&](UIButton *pButtonContext, void *pContext) {
			return SharePressed(pButtonContext, pContext);
		};
		auto fnBackCallback = [&](UIButton *pButtonContext, void *pContext) {
			return BackPressed(pButtonContext, pContext);
		};
		auto fnForwardCallback = [&](UIButton *pButtonContext, void *pContext) {
			return ForwardPressed(pButtonContext, pContext);
		};
		auto fnURLCallback = [&](UIButton *pButtonContext, void *pContext) {
			return URLPressed(pButtonContext, pContext);
		};
		auto fnKeyboardCallback = [&](UIButton *pButtonContext, void *pContext) {
			return KeyboardPressed(pButtonContext, pContext);
		};
		auto fnTabCallback = [&](UIButton *pButtonContext, void *pContext) {
			return TabPressed(pButtonContext, pContext);
		};
		auto fnBackTabCallback = [&](UIButton *pButtonContext, void *pContext) {
			return BackTabPressed(pButtonContext, pContext);
		};

		auto fnDoneCallback = [&](UIButton *pButtonContext, void *pContext) {
			return DonePressed(pButtonContext, pContext);
		};

		// update button trigger events to match the observer
		CR(m_pCloseButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnCloseCallback));
		CR(m_pToggleButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnToggleCallback));
		CR(m_pForwardButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnForwardCallback));
		CR(m_pBackButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnBackCallback));
		CR(m_pShareToggleButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnShareCallback));
		CR(m_pOpenButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnOpenCallback));
		CR(m_pURLButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnURLCallback));
		CR(m_pKeyboardButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnKeyboardCallback));
		CR(m_pTabButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnTabCallback));
		CR(m_pBackTabButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnBackTabCallback));
		CR(m_pDoneButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnDoneCallback));
	}

Error:
	return r;
}

RESULT UIControlBar::BackPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleBackPressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::ForwardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleForwardPressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::KeyboardPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleKeyboardPressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::TogglePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleShowTogglePressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::OpenPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleOpenPressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::ClosePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleClosePressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::SharePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleShareTogglePressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::TabPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleTabPressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::BackTabPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleBackTabPressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::DonePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleDonePressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::URLPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	CN(m_pObserver);
	CR(m_pObserver->HandleURLPressed(pButtonContext, pContext));
Error:
	return r;
}

RESULT UIControlBar::UpdateButtonsWithType(BarType type) {
	RESULT r = R_PASS;

	m_barType = type;

	if (m_barType == BarType::BROWSER || m_barType == BarType::DESKTOP) {
		// set buttons positions based on spec
		point ptStart = point(-m_totalWidth / 2.0f, 0.0f, 0.0f);

		ptStart = ptStart - point(m_itemSpacing, 0.0f, 0.0f);

		//*
		point ptBack = ptStart + point(m_itemSide / 2.0f, 0.0f, 0.0f);
		m_pBackButton->SetPosition(ptBack);

		point ptForward = ptBack + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
		m_pForwardButton->SetPosition(ptForward);

		point ptClose = ptForward + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
		m_pCloseButton->SetPosition(ptClose);

		point ptKeyboard = ptStart + point(m_itemSide + (m_itemSpacing / 2.0), 0.0f, 0.0f);
		m_pKeyboardButton->SetPosition(ptKeyboard);
		//*/

		point ptURL = point(0.0f, 0.0f, 0.0f);// ptStart + point(m_urlWidth / 2.0f, 0.0f, 0.0f);
		m_pURLButton->SetPosition(ptURL);

		point ptShare = ptURL + point(m_urlWidth / 2.0f + m_itemSide / 2.0f + m_itemSpacing, 0.0f, 0.0f);
		m_pShareToggleButton->SetPosition(ptShare);

		point ptOpen = ptShare + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
		m_pOpenButton->SetPosition(ptOpen);

		point ptHide = ptOpen + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
		m_pToggleButton->SetPosition(ptHide);

		if (m_barType == BarType::BROWSER) {
			m_pKeyboardButton->SetVisible(false);
			m_pBackButton->SetVisible(true);
			m_pForwardButton->SetVisible(true);
		}
		else if (m_barType == BarType::DESKTOP) {
			m_pKeyboardButton->SetVisible(true);
			m_pBackButton->SetVisible(false);
			m_pForwardButton->SetVisible(false);
		}

		m_pBackTabButton->SetVisible(false);
		m_pTabButton->SetVisible(false);
		m_pDoneButton->SetVisible(false);
	}
	else if (m_barType == BarType::KEYBOARD) {
		point ptLeft = point(-m_totalWidth / 2.0f + m_itemSpacing / 2.0f, 0.0f, 0.0f);
		point ptBackTab = ptLeft + point(m_pBackTabButton->GetSurface()->GetWidth()/2.0f, 0.0f, 0.0f);
		m_pBackTabButton->SetPosition(ptBackTab);

		point ptTab = ptBackTab + point(m_pTabButton->GetSurface()->GetWidth() + m_itemSpacing, 0.0f, 0.0f);
		m_pTabButton->SetPosition(ptTab);

		point ptRight = point(m_totalWidth / 2.0f - m_itemSpacing / 2.0f, 0.0f, 0.0f);
		point ptClose = ptRight + point(-m_pDoneButton->GetSurface()->GetWidth()/2.0f, 0.0f, 0.0f);
		m_pDoneButton->SetPosition(ptClose);

		m_pKeyboardButton->SetVisible(false);
		m_pBackButton->SetVisible(false);
		m_pForwardButton->SetVisible(false);
		m_pURLButton->SetVisible(false);
		m_pShareToggleButton->SetVisible(false);
		m_pOpenButton->SetVisible(false);
		m_pToggleButton->SetVisible(false);
		m_pCloseButton->SetVisible(false);
	}
//Error:
	return r;
}

RESULT UIControlBar::UpdateNavigationButtons(bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	if (fCanGoBack) {
		m_pBackButton->GetSurface()->SetDiffuseTexture(m_pBackTexture);
	}
	else {
		m_pBackButton->GetSurface()->SetDiffuseTexture(m_pBackDisabledTexture);
	}

	if (fCanGoForward) {
		m_pForwardButton->GetSurface()->SetDiffuseTexture(m_pForwardTexture);
	}
	else {
		m_pForwardButton->GetSurface()->SetDiffuseTexture(m_pForwardDisabledTexture);
	}

	return r;
}

RESULT UIControlBar::HandleTouchStart(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	//TODO: very similar to the code in DreamUIBar::HandleTouchStart
	std::shared_ptr<quad> pSurface = nullptr;
	vector vSurface;
	vector vRotation;
	quaternion qSurface;
	quaternion qRotation;
	CBR(IsVisible(), R_SKIPPED);
	CNR(pButtonContext, R_SKIPPED);
	CBR(pButtonContext->IsVisible(), R_SKIPPED);

	//TODO: this only works if these textures are used for no other purpose
	CBR(pButtonContext->GetSurface()->GetTextureDiffuse() != m_pBackDisabledTexture &&
		pButtonContext->GetSurface()->GetTextureDiffuse() != m_pForwardDisabledTexture, R_SKIPPED);

	CBR(pButtonContext->GetSurface()->GetTextureDiffuse() != m_pCantBackTabTexture &&
		pButtonContext->GetSurface()->GetTextureDiffuse() != m_pCantTabTexture, R_SKIPPED);

	pSurface = pButtonContext->GetSurface();

	//vector for captured object movement
	qSurface = pButtonContext->GetOrientation() * (pSurface->GetOrientation());
	qSurface.Reverse();
	vSurface = qSurface.RotateVector(pSurface->GetNormal() * -1.0f);

	//vector for captured object collisions
	qRotation = pSurface->GetOrientation(true);
	qRotation.Reverse();
	vRotation = qRotation.RotateVector(pSurface->GetNormal() * -1.0f);

	InteractionEngineProxy* pInteractionProxy;
	pInteractionProxy = m_pDreamOS->GetInteractionEngineProxy();
	pInteractionProxy->ResetObjects(pButtonContext->GetInteractionObject());
	pInteractionProxy->ReleaseObjects(pButtonContext->GetInteractionObject());

	pInteractionProxy->CaptureObject(
		pButtonContext,
		pButtonContext->GetInteractionObject(), 
		pButtonContext->GetContactPoint(), 
		vRotation,
		vSurface,
		m_actuationDepth);
Error:
	return r;
}


float UIControlBar::GetSpacingOffset() {
	return -(m_itemSpacing + m_itemSide / 2.0f);
}

std::shared_ptr<UIButton> UIControlBar::GetBackButton() {
	return m_pBackButton;
}

std::shared_ptr<UIButton> UIControlBar::GetForwardButton() {
	return m_pForwardButton;
}

std::shared_ptr<UIButton> UIControlBar::GetKeyboardButton() {
	return m_pKeyboardButton;
}

std::shared_ptr<UIButton> UIControlBar::GetToggleButton() {
	return m_pToggleButton;
}

std::shared_ptr<UIButton> UIControlBar::GetShareButton() {
	return m_pShareToggleButton;
}

std::shared_ptr<UIButton> UIControlBar::GetStopButton() {
	return m_pCloseButton;
}

std::shared_ptr<UIButton> UIControlBar::GetTabButton() {
	return m_pTabButton;
}

std::shared_ptr<UIButton> UIControlBar::GetBackTabButton() {
	return m_pBackTabButton;
}

std::shared_ptr<UIButton> UIControlBar::GetDoneButton() {
	return m_pDoneButton;
}

std::shared_ptr<UIButton> UIControlBar::GetURLButton() {
	return m_pURLButton;
}

texture *UIControlBar::GetHideTexture() {
	return m_pHideTexture;
}

texture *UIControlBar::GetShowTexture() {
	return m_pShowTexture;
}

texture *UIControlBar::GetShareTexture() {
	return m_pShareTexture;
}

texture *UIControlBar::GetStopTexture() {
	return m_pStopSharingTexture;
}

texture *UIControlBar::GetTabTexture() {
	return m_pTabTexture;
}

texture *UIControlBar::GetCantTabTexture() {
	return m_pCantTabTexture;
}

texture *UIControlBar::GetBackTabTexture() {
	return m_pBackTabTexture;
}

texture *UIControlBar::GetCantBackTabTexture() {
	return m_pCantBackTabTexture;
}

RESULT UIControlBar::SetTotalWidth(float totalWidth) {
	m_totalWidth = totalWidth;
	return R_PASS;
}

RESULT UIControlBar::SetItemSide(float itemSide) {
	m_itemSide = itemSide;
	return R_PASS;
}

RESULT UIControlBar::SetURLWidth(float urlWidth) {
	m_urlWidth = urlWidth;
	return R_PASS;
}

RESULT UIControlBar::SetItemSpacing(float itemSpacing) {
	m_itemSpacing = itemSpacing;
	return R_PASS;
}

std::shared_ptr<text> UIControlBar::GetURLText() {
	return m_pURLText;
}

RESULT UIControlBar::RegisterObserver(ControlBarObserver *pObserver) {
	RESULT r = R_PASS;

	CNM((pObserver), "Observer cannot be nullptr");
	CBM((m_pObserver == nullptr), "Can't overwrite control bar observer");
	m_pObserver = pObserver;

Error:
	return r;
}

RESULT UIControlBar::UnregisterObserver(ControlBarObserver *pObserver) {
	RESULT r = R_PASS;

	CN(pObserver);
	CBM((m_pObserver == pObserver), "Control Bar Observer is not set to this object");

	m_pObserver = nullptr;

Error:
	return r;
}

BarType UIControlBar::ControlBarTypeFromString(const std::string& strContentType) {

	//TODO: use static map
	if (strContentType == CONTENT_TYPE_BROWSER) {
		return BarType::BROWSER;
	}
	else if (strContentType == CONTENT_TYPE_DESKTOP) {
		return BarType::DESKTOP;
	}
	else if (strContentType == CONTENT_TYPE_DEFAULT) {
		return BarType::DEFAULT;
	}
	else {
		return BarType::INVALID;
	}
}
