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


RESULT UIControlBar::AddButton(ControlBarButtonType type, float offset, float width, std::function<RESULT(UIButton*, void*)> fnCallback, std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture> pDisabledTexture) {
	RESULT r = R_PASS;
	
	std::shared_ptr<UIButton> pButton = nullptr;

	CBR(type != ControlBarButtonType::INVALID, R_SKIPPED);

	if (pEnabledTexture == nullptr || pDisabledTexture == nullptr) {
		pButton = AddUIButton(width, m_itemSide);
		CR(pButton->GetSurface()->SetDiffuseTexture(m_buttonTextures[type]));
	}
	else {
		pButton = AddUIButton(pEnabledTexture, pDisabledTexture, width, m_itemSide);
	}

	pButton->SetPosition(point(offset, 0.0f, 0.0f));


	// if there isn't a trigger callback provided, 
	// the button doesn't need to be interactable at all
	if (fnCallback != nullptr) {
		CR(pButton->RegisterToInteractionEngine(m_pDreamOS));
		CR(pButton->RegisterTouchStart());
		CR(pButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER, fnCallback));
	}
	
	// Assumption: a control bar's buttons have unique types
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