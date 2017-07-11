#include "UIView.h"
#include "Primitives/ray.h"
#include "UIButton.h"
#include "UIMenuItem.h"
#include "UIScrollView.h"

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

std::shared_ptr<UIScrollView> UIView::MakeUIScrollView() {
	std::shared_ptr<UIScrollView> pScrollView(new UIScrollView(m_pHALImp, m_pDreamOS));

	return pScrollView;
}

std::shared_ptr<UIScrollView> UIView::AddUIScrollView() {
	RESULT r = R_PASS;

	std::shared_ptr<UIScrollView> pScrollView = MakeUIScrollView();
	CR(AddObject(pScrollView));
	return pScrollView;
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
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_BEGIN, pEvent->m_pObject, pEvent->m_pInteractionObject);
		CR(NotifySubscribers(UIEventType::UI_SELECT_BEGIN, pUIEvent));
	} break;
	case (InteractionEventType::ELEMENT_COLLIDE_MOVED): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_MOVED, pEvent->m_pObject, pEvent->m_pInteractionObject);
		CR(NotifySubscribers(UIEventType::UI_SELECT_MOVED, pUIEvent));
	} break;
	case (InteractionEventType::ELEMENT_COLLIDE_ENDED): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_SELECT_ENDED, pEvent->m_pObject, pEvent->m_pInteractionObject);
		CR(NotifySubscribers(UIEventType::UI_SELECT_ENDED, pUIEvent));
	} break;
	case (InteractionEventType::INTERACTION_EVENT_MENU): {
		UIEvent *pUIEvent = new UIEvent(UIEventType::UI_MENU, pEvent->m_pObject, pEvent->m_pInteractionObject);
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