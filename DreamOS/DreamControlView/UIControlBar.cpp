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

Error:
	return r;
}


std::shared_ptr<UIButton> UIControlBar::AddButton(ControlBarButtonType type, float offset, float width, std::function<RESULT(UIButton*, void*)> fnCallback, std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture> pDisabledTexture) {
	RESULT r = R_PASS;
	
	std::shared_ptr<UIButton> pButton = nullptr;

	CBR(type != ControlBarButtonType::INVALID, R_SKIPPED);

	if (pEnabledTexture == nullptr) {
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
	
	return pButton;
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