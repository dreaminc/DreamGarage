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
}

UIControlBar::~UIControlBar() {
	// empty
}

RESULT UIControlBar::Initialize() {
	RESULT r = R_PASS;

	m_buttonTextures[ControlBarButtonType::BACK] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBack);
	m_buttonTextures[ControlBarButtonType::FORWARD] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForward);
	m_buttonTextures[ControlBarButtonType::CANT_BACK] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBackDisabled);
	m_buttonTextures[ControlBarButtonType::CANT_FORWARD] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszForwardDisabled);
	m_buttonTextures[ControlBarButtonType::MINIMIZE] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszHide);
	m_buttonTextures[ControlBarButtonType::MAXIMIZE] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShow);
	m_buttonTextures[ControlBarButtonType::OPEN] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszOpen);
	m_buttonTextures[ControlBarButtonType::CLOSE] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszClose);
	m_buttonTextures[ControlBarButtonType::URL] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszURL);
	m_buttonTextures[ControlBarButtonType::KEYBOARD] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszKeyboard);
	m_buttonTextures[ControlBarButtonType::SHARE] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszShare);
	m_buttonTextures[ControlBarButtonType::STOP] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszStopSharing);

	m_buttonTextures[ControlBarButtonType::TAB] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszTab);
	m_buttonTextures[ControlBarButtonType::CANT_TAB] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszCantTab);
	m_buttonTextures[ControlBarButtonType::BACKTAB] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszBackTab);
	m_buttonTextures[ControlBarButtonType::CANT_BACKTAB] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszCantBackTab);
	m_buttonTextures[ControlBarButtonType::DONE] = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszDone);

	for (auto pTexturePair : m_buttonTextures) {
		CN(pTexturePair.second);
	}

Error:
	return r;
}

RESULT UIControlBar::InitializeText() {
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
	GetButton(ControlBarButtonType::URL)->AddObject(m_pURLText);

Error:
	return r;
}

RESULT UIControlBar::UpdateNavigationButtons(bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;

	auto pBackButton = GetButton(ControlBarButtonType::BACK);
	auto pForwardButton = GetButton(ControlBarButtonType::FORWARD);

	CN(pBackButton);
	CN(pForwardButton);

	if (fCanGoBack) {
		pBackButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::BACK));
	}
	else {
		pBackButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::CANT_BACK));
	}

	if (fCanGoForward) {
		pForwardButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::FORWARD));
	}
	else {
		pForwardButton->GetSurface()->SetDiffuseTexture(GetTexture(ControlBarButtonType::CANT_FORWARD));
	}

Error:
	return r;
}

RESULT UIControlBar::AddButton(ControlBarButtonType type, float offset, float width, std::function<RESULT(UIButton*, void*)> fnCallback) {
	RESULT r = R_PASS;
	
	// TODO: texture, width from type
	std::shared_ptr<UIButton> pButton = AddUIButton(width, m_itemSide);

	pButton->SetPosition(point(offset, 0.0f, 0.0f));

	CBR(type != ControlBarButtonType::INVALID, R_SKIPPED);
	CR(pButton->GetSurface()->SetDiffuseTexture(m_buttonTextures[type]));

	CR(pButton->RegisterToInteractionEngine(m_pDreamOS));

	if (type != ControlBarButtonType::URL) {
		CR(pButton->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
			std::bind(&UIControlBar::HandleTouchStart, this, std::placeholders::_1, std::placeholders::_2)));
		CR(pButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnCallback));
	}
	
	m_buttons[type] = pButton;

Error:
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
	CBR(pButtonContext->GetSurface()->GetTextureDiffuse() != m_buttonTextures[ControlBarButtonType::CANT_BACK] &&
		pButtonContext->GetSurface()->GetTextureDiffuse() != m_buttonTextures[ControlBarButtonType::CANT_FORWARD], R_SKIPPED);

	CBR(pButtonContext->GetSurface()->GetTextureDiffuse() != m_buttonTextures[ControlBarButtonType::CANT_TAB] &&
		pButtonContext->GetSurface()->GetTextureDiffuse() != m_buttonTextures[ControlBarButtonType::CANT_BACKTAB], R_SKIPPED);

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

std::shared_ptr<UIButton> UIControlBar::GetButton(ControlBarButtonType type) {
	RESULT r = R_PASS;

	CB(m_buttons.count(type) > 0);
	return m_buttons[type];

Error:
	return nullptr;
}

texture* UIControlBar::GetTexture(ControlBarButtonType type) {
	RESULT r = R_PASS;

	CB(m_buttonTextures.count(type) > 0);
	return m_buttonTextures[type];

Error:
	return nullptr;
}

RESULT UIControlBar::SetItemSide(float itemSide) {
	m_itemSide = itemSide;
	return R_PASS;
}

RESULT UIControlBar::SetItemSpacing(float itemSpacing) {
	m_itemSpacing = itemSpacing;
	return R_PASS;
}

RESULT UIControlBar::SetURLWidth(float urlWidth) {
	m_urlWidth = urlWidth;
	return R_PASS;
}

std::shared_ptr<text> UIControlBar::GetURLText() {
	return m_pURLText;
}