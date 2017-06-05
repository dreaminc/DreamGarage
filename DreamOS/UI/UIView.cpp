#include "UIView.h"
#include "Primitives/ray.h"
#include "UIButton.h"

UIView::UIView(HALImp *pHALImp) :
composite(pHALImp)
{
	for (int i = 0; i < UIEventType::UI_EVENT_INVALID; i++) {
		RegisterEvent((UIEventType)(i));
	}
}

UIView::~UIView() {

}

std::shared_ptr<UIButton> UIView::MakeUIButton() {
	std::shared_ptr<UIButton> pButton(new UIButton(m_pHALImp));

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

RESULT UIView::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;
	//TODO: symbolic for now, change when there are multiple object types
	//auto pRay = std::dynamic_pointer_cast<std::shared_ptr<ray>>(pEvent->m_pInteractionRay);
	auto pRay = pEvent->m_pInteractionRay;
	switch (pEvent->m_eventType) {
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
	}

Error:
	return r;
}