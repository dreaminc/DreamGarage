#include "UIView.h"
#include "Primitives/ray.h"
#include "UIButton.h"
#include "UIMenuItem.h"
#include "UISpatialScrollView.h"
#include "UIFlatScrollView.h"
#include "UISurface.h"
#include "UIPointerLabel.h"

// TODO: move these files to DreamOS(?)
#include "DreamGarage/UserAreaControls.h"
#include "DreamGarage/UITabView.h"
#include "DreamControlView/UIControlView.h"

#include "DreamOS.h"

UIView::UIView(HALImp *pHALImp, DreamOS	*pDreamOS) :
composite(pHALImp),
m_pDreamOS(pDreamOS)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UIView::UIView(HALImp *pHALImp, DreamOS	*pDreamOS, float width, float height) :
composite(pHALImp),
m_pDreamOS(pDreamOS),
m_width(width),
m_height(height)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UIView::~UIView() {

}

RESULT UIView::Initialize() {
	RESULT r = R_PASS;

	for (int i = 0; i < (int)(UIEventType::UI_EVENT_INVALID); i++) {
		CR(RegisterEvent((UIEventType)(i)));
	}

Error:
	return r;
}

std::shared_ptr<UIView> UIView::MakeUIView() {
	std::shared_ptr<UIView> pView(new UIView(m_pHALImp, m_pDreamOS));

	return pView;
}

std::shared_ptr<UIView> UIView::AddUIView() {
	RESULT r = R_PASS;

	std::shared_ptr<UIView> pView = MakeUIView();
	CR(AddObject(pView));
	return pView;
Error:
	return nullptr;
}

std::shared_ptr<UIButton> UIView::MakeUIButton() {
	std::shared_ptr<UIButton> pButton(new UIButton(m_pHALImp, m_pDreamOS));

	return pButton;
}

std::shared_ptr<UIButton> UIView::AddUIButton() {
	RESULT r = R_PASS;

	std::shared_ptr<UIButton> pButton = MakeUIButton();
	CR(AddObject(pButton));
	return pButton;
Error:
	return nullptr;
}

std::shared_ptr<UIButton> UIView::MakeUIButton(float width, float height) {
	std::shared_ptr<UIButton> pButton(new UIButton(m_pHALImp, m_pDreamOS, width, height));

	return pButton;
}

std::shared_ptr<UIButton> UIView::AddUIButton(float width, float height) {
	RESULT r = R_PASS;

	std::shared_ptr<UIButton> pButton = MakeUIButton(width, height);
	CR(AddObject(pButton));
	return pButton;
Error:
	return nullptr;
}

std::shared_ptr<UIButton> UIView::MakeUIButton(std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture> pDisabledTexture, float width, float height) {
	std::shared_ptr<UIButton> pButton(new UIButton(m_pHALImp, m_pDreamOS, width, height, pEnabledTexture, pDisabledTexture));

	return pButton;
}

std::shared_ptr<UIButton> UIView::AddUIButton(std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture> pDisabledTexture, float width, float height) {
	RESULT r = R_PASS;

	std::shared_ptr<UIButton> pButton = MakeUIButton(pEnabledTexture, pDisabledTexture, width, height);
	CR(AddObject(pButton));
	return pButton;
Error:
	return nullptr;
}

std::shared_ptr<UIButton> UIView::AddButton(float offset, float width, float height, std::function<RESULT(UIButton*, void*)> fnCallback, std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture> pDisabledTexture) {
	RESULT r = R_PASS;

	std::shared_ptr<UIButton> pButton = AddUIButton(pEnabledTexture, pDisabledTexture, width, height);

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


std::shared_ptr<UIMenuItem> UIView::MakeUIMenuItem() {
	std::shared_ptr<UIMenuItem> pButton(new UIMenuItem(m_pHALImp, m_pDreamOS));

	return pButton;
}

std::shared_ptr<UIMenuItem> UIView::AddUIMenuItem() {
	RESULT r = R_PASS;

	std::shared_ptr<UIMenuItem> pButton = MakeUIMenuItem();
	CR(AddObject(pButton));
	return pButton;
Error:
	return nullptr;
}

std::shared_ptr<UIMenuItem> UIView::MakeUIMenuItem(float width, float height) {
	std::shared_ptr<UIMenuItem> pButton(new UIMenuItem(m_pHALImp, m_pDreamOS, width, height));

	return pButton;
}

std::shared_ptr<UIMenuItem> UIView::AddUIUIMenuItem(float width, float height) {
	RESULT r = R_PASS;

	std::shared_ptr<UIMenuItem> pButton = MakeUIMenuItem(width, height);
	CR(AddObject(pButton));
	return pButton;
Error:
	return nullptr;
}

std::shared_ptr<UserAreaControls> UIView::MakeUIContentControlBar() {
	std::shared_ptr<UserAreaControls> pControlBar(new UserAreaControls(m_pHALImp, m_pDreamOS));

	return pControlBar;
}

std::shared_ptr<UserAreaControls> UIView::AddUIContentControlBar() {
	RESULT r = R_PASS;

	std::shared_ptr<UserAreaControls> pControlBar = MakeUIContentControlBar();
	CR(AddObject(pControlBar));
	return pControlBar;
Error:
	return nullptr;
}

std::shared_ptr<UITabView> UIView::MakeUITabView() {
	std::shared_ptr<UITabView> pTabView(new UITabView (m_pHALImp, m_pDreamOS));

	return pTabView;
}

std::shared_ptr<UITabView> UIView::AddUITabView() {
	RESULT r = R_PASS;

	std::shared_ptr<UITabView> pTabView = MakeUITabView();
	CR(AddObject(pTabView));
	return pTabView;
Error:
	return nullptr;
}

std::shared_ptr<UISpatialScrollView> UIView::MakeUISpatialScrollView() {
	std::shared_ptr<UISpatialScrollView> pScrollView(new UISpatialScrollView(m_pHALImp, m_pDreamOS));

	return pScrollView;
}

std::shared_ptr<UISpatialScrollView> UIView::AddUISpatialScrollView() {
	RESULT r = R_PASS;

	std::shared_ptr<UISpatialScrollView> pScrollView = MakeUISpatialScrollView();
	CR(AddObject(pScrollView));
	return pScrollView;
Error:
	return nullptr;
}

std::shared_ptr<UIFlatScrollView> UIView::MakeUIFlatScrollView() {
	std::shared_ptr<UIFlatScrollView> pScrollView(new UIFlatScrollView(m_pHALImp, m_pDreamOS));

	return pScrollView;
}

std::shared_ptr<UIFlatScrollView> UIView::AddUIFlatScrollView() {
	RESULT r = R_PASS;

	std::shared_ptr<UIFlatScrollView> pScrollView = MakeUIFlatScrollView();
	CR(AddObject(pScrollView));
	return pScrollView;
Error:
	return nullptr;
}

std::shared_ptr<UIControlView> UIView::MakeUIControlView() {
	std::shared_ptr<UIControlView> pControlView(new UIControlView(m_pHALImp, m_pDreamOS));

	return pControlView;
}

std::shared_ptr<UIControlView> UIView::AddUIControlView() {
	RESULT r = R_PASS;

	std::shared_ptr<UIControlView> pControlView = MakeUIControlView();
	CR(AddObject(pControlView));
	return pControlView;
Error:
	return nullptr;
}

std::shared_ptr<UISurface> UIView::MakeUISurface() {
	std::shared_ptr<UISurface> pSurface(new UISurface(m_pHALImp, m_pDreamOS));

	return pSurface;
}

std::shared_ptr<UISurface> UIView::AddUISurface() {
	RESULT r = R_PASS;
	std::shared_ptr<UISurface> pSurface = MakeUISurface();
	CR(AddObject(pSurface));
	return pSurface;
Error:
	return nullptr;
}

std::shared_ptr<UIPointerLabel> UIView::MakeUIPointerLabel() {
	RESULT r = R_PASS;

	std::shared_ptr<UIPointerLabel> pPointerLabel(new UIPointerLabel(m_pHALImp, m_pDreamOS));
	CN(pPointerLabel);
	CR(pPointerLabel->Initialize());

	return pPointerLabel;
Error:
	return nullptr;
}

std::shared_ptr<UIPointerLabel> UIView::AddUIPointerLabel() {
	RESULT r = R_PASS;
	std::shared_ptr<UIPointerLabel> pLabel = MakeUIPointerLabel();
	CR(AddObject(pLabel));
	return pLabel;
Error:
	return nullptr;
}

RESULT UIView::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;
	//TODO: symbolic for now, change when there are multiple object types
	//auto pRay = std::dynamic_pointer_cast<std::shared_ptr<ray>>(pEvent->m_pInteractionRay);
//	auto pRay = pEvent->m_pInteractionObject;
	switch (pEvent->m_eventType) {
	case (InteractionEventType::ELEMENT_COLLIDE_BEGAN): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_BEGIN, pEvent->m_pObject, pEvent->m_pInteractionObject, pEvent->m_ptContact[0]);
		CR(NotifySubscribers(UIEventType::UI_SELECT_BEGIN, pUIEvent));
	} break;
	case (InteractionEventType::ELEMENT_COLLIDE_MOVED): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_MOVED, pEvent->m_pObject, pEvent->m_pInteractionObject, pEvent->m_ptContact[0]);
		CR(NotifySubscribers(UIEventType::UI_SELECT_MOVED, pUIEvent));
	} break;
	case (InteractionEventType::ELEMENT_COLLIDE_TRIGGER): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_TRIGGER, pEvent->m_pObject, pEvent->m_pInteractionObject, pEvent->m_ptContact[0]);
		CR(NotifySubscribers(UIEventType::UI_SELECT_TRIGGER, pUIEvent));
	} break;
	case (InteractionEventType::ELEMENT_COLLIDE_ENDED): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_ENDED, pEvent->m_pObject, pEvent->m_pInteractionObject, pEvent->m_ptContact[0]);
		CR(NotifySubscribers(UIEventType::UI_SELECT_ENDED, pUIEvent));
	} break;
	case (InteractionEventType::INTERACTION_EVENT_MENU): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_MENU, pEvent->m_pObject, pEvent->m_pInteractionObject, pEvent->m_ptContact[0]);
		this;
		CR(NotifySubscribers(UIEventType::UI_MENU, pUIEvent));
	} break;
		/*
	case (InteractionEventType::ELEMENT_INTERSECT_BEGAN): {
		if (pRay != nullptr) {
			UIEvent *pUIEvent = new UIEvent(UI_HOVER_BEGIN, pEvent->m_pObject);
			CR(NotifySubscribers(UI_HOVER_BEGIN, pUIEvent));
		}
		else {
			UIEvent *pUIEvent = new UIEvent(UI_SELECT_BEGIN, pEvent->m_pObject);
			CR(NotifySubscribers(UI_SELECT_BEGIN, pUIEvent));
		}
	} break;
	case (InteractionEventType::ELEMENT_INTERSECT_ENDED): {
		if (pRay != nullptr) {
			UIEvent *pUIEvent = new UIEvent(UI_HOVER_ENDED, pEvent->m_pObject);
			CR(NotifySubscribers(UI_HOVER_ENDED, pUIEvent));
		}
		else {
			UIEvent *pUIEvent = new UIEvent(UI_SELECT_ENDED, pEvent->m_pObject);
			CR(NotifySubscribers(UI_SELECT_ENDED, pUIEvent));
		}
	} break;
	case (InteractionEventType::ELEMENT_INTERSECT_MOVED): {
		if (pRay == nullptr) {
			UIEvent *pUIEvent = new UIEvent(UI_SELECT_MOVED, pEvent->m_pObject);
			CR(NotifySubscribers(UI_SELECT_MOVED, pUIEvent));
		}
	} break;
	case (InteractionEventType::INTERACTION_EVENT_SELECT_UP): {
		UIEvent *pUIEvent = new UIEvent(UI_SELECT_ENDED, pEvent->m_pObject);
		CR(NotifySubscribers(UI_SELECT_BEGIN, pUIEvent));
	} break;
	case (InteractionEventType::INTERACTION_EVENT_SELECT_DOWN): {
		UIEvent *pUIEvent = new UIEvent(UI_SELECT_BEGIN, pEvent->m_pObject);
		CR(NotifySubscribers(UI_SELECT_ENDED, pUIEvent));
	} break;
//*/
	}

Error:
	return r;
}