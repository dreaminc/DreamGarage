#include "UIControlBar.h"
#include "DreamOS.h"
#include "UI/UIButton.h"
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
	m_pBackTexture = m_pDreamOS->MakeTexture(k_wszBack, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pForwardTexture = m_pDreamOS->MakeTexture(k_wszForward, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pHideTexture = m_pDreamOS->MakeTexture(k_wszHide, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pShowTexture = m_pDreamOS->MakeTexture(k_wszShow, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pOpenTexture = m_pDreamOS->MakeTexture(k_wszOpen, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pCloseTexture = m_pDreamOS->MakeTexture(k_wszClose, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pURLTexture = m_pDreamOS->MakeTexture(k_wszURL, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pShareTexture = m_pDreamOS->MakeTexture(k_wszShare, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pStopSharingTexture = m_pDreamOS->MakeTexture(k_wszStopSharing, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

	// create buttons

	m_pBackButton = AddUIButton(m_itemSide, m_itemSide);
	m_pBackButton->GetSurface()->SetDiffuseTexture(m_pBackTexture);

	m_pForwardButton = AddUIButton(m_itemSide, m_itemSide);
	m_pForwardButton->GetSurface()->SetDiffuseTexture(m_pForwardTexture);

	m_pToggleButton = AddUIButton(m_itemSide, m_itemSide);
	m_pToggleButton->GetSurface()->SetDiffuseTexture(m_pHideTexture);

	m_pOpenButton = AddUIButton(m_itemSide, m_itemSide);
	m_pOpenButton->GetSurface()->SetDiffuseTexture(m_pOpenTexture);

	m_pCloseButton = AddUIButton(m_itemSide, m_itemSide);
	m_pCloseButton->GetSurface()->SetDiffuseTexture(m_pCloseTexture);

	m_pShareToggleButton = AddUIButton(m_itemSide, m_itemSide);
	m_pShareToggleButton->GetSurface()->SetDiffuseTexture(m_pShareTexture);

	m_pURLButton = AddUIButton(m_urlWidth, m_itemSide);
	m_pURLButton->GetSurface()->SetDiffuseTexture(m_pURLTexture);


	for (auto pButton : GetControlButtons()) {
		CN(pButton);
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));

		CR(pButton->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
			std::bind(&UIControlBar::HandleTouchStart, this, std::placeholders::_1, std::placeholders::_2)));
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

		// update button trigger events to match the observer
		CR(m_pCloseButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnCloseCallback));
		CR(m_pToggleButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnToggleCallback));
		CR(m_pForwardButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnForwardCallback));
		CR(m_pBackButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnBackCallback));
		CR(m_pShareToggleButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnShareCallback));
		CR(m_pOpenButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnOpenCallback));
		CR(m_pURLButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnURLCallback));
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

	auto pFont = m_pDreamOS->MakeFont(L"Basis_grotesque_pro.fnt", true);
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


	// set buttons positions based on spec
	point ptStart = point(-m_totalWidth / 2.0f, 0.0f, 0.0f);

	// could be wrong
	ptStart = ptStart - point(m_itemSpacing, 0.0f, 0.0f);
	//*
	point ptBack = ptStart + point(m_itemSide / 2.0f, 0.0f, 0.0f);
	m_pBackButton->SetPosition(ptBack);

	point ptForward = ptBack + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
	m_pForwardButton->SetPosition(ptForward);

	point ptClose = ptForward + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
	m_pCloseButton->SetPosition(ptClose);
	//*/

	point ptURL = point(0.0f, 0.0f, 0.0f);// ptStart + point(m_urlWidth / 2.0f, 0.0f, 0.0f);
	m_pURLButton->SetPosition(ptURL);

	point ptShare = ptURL + point(m_urlWidth / 2.0f + m_itemSide / 2.0f + m_itemSpacing, 0.0f, 0.0f);
	m_pShareToggleButton->SetPosition(ptShare);
	
	point ptOpen = ptShare + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
	m_pOpenButton->SetPosition(ptOpen);

	point ptHide = ptOpen + point(m_itemSide + m_itemSpacing, 0.0f, 0.0f);
	m_pToggleButton->SetPosition(ptHide);

//Error:
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
	CNR(pButtonContext, R_SKIPPED);
	pSurface = pButtonContext->GetSurface();

	//vector for captured object movement
	qSurface = pButtonContext->GetOrientation() * (pSurface->GetOrientation());
	qSurface.Reverse();
	vSurface = qSurface.RotateVector(pSurface->GetNormal() * -1.0f);

	//vector for captured object collisions
	qRotation = pSurface->GetOrientation(true);
	qRotation.Reverse();
	vRotation = qRotation.RotateVector(pSurface->GetNormal() * -1.0f);

	auto pInteractionProxy = m_pDreamOS->GetInteractionEngineProxy();
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

std::shared_ptr<UIButton> UIControlBar::GetToggleButton() {
	return m_pToggleButton;
}

std::shared_ptr<UIButton> UIControlBar::GetStopButton() {
	return m_pCloseButton;
}

std::shared_ptr<UIButton> UIControlBar::GetURLButton() {
	return m_pURLButton;
}

std::vector<std::shared_ptr<UIButton>> UIControlBar::GetControlButtons() {
	return { m_pBackButton, m_pForwardButton, m_pToggleButton, m_pCloseButton, m_pOpenButton, m_pShareToggleButton, m_pURLButton };
}

texture *UIControlBar::GetHideTexture() {
	return m_pHideTexture;
}

texture *UIControlBar::GetShowTexture() {
	return m_pShowTexture;
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

RESULT UIControlBar::SetObserver(ControlBarObserver *pObserver) {
	m_pObserver = pObserver;
	return R_PASS;
}

BarType UIControlBar::ControlBarTypeFromString(const std::string& strContentType) {

	//TODO: use static map
	if (strContentType == "ContentControlType.Website") {
		return BarType::BROWSER;
	}
	else if (strContentType == "") {
		return BarType::DEFAULT;
	}
	else {
		return BarType::INVALID;
	}
}

std::shared_ptr<UIControlBar> UIControlBar::MakeControlBarWithType(BarType type, std::shared_ptr<UIView> pViewContext) {

	switch (type) {

	case BarType::BROWSER:
	case BarType::DEFAULT: {
		return pViewContext->MakeUIControlBar();
	}

	case BarType::INVALID: {
		return nullptr;
	}

	}

	return nullptr;
}