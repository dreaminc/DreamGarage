#include "UIView.h"

UIView::UIView(HALImp *pHALImp) :
composite(pHALImp)
{
	for (int i = 0; i < UIEventType::UI_EVENT_INVALID; i++) {
		RegisterEvent((UIEventType)(i));
	}
}

UIView::~UIView() {

}

RESULT UIView::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;
	switch (pEvent->m_eventType) {
	case (InteractionEventType::ELEMENT_INTERSECT_BEGAN): {
		UIEvent *pUIEvent = new UIEvent(UI_EVENT_INTERSECT_BEGAN, pEvent->m_pObject);
		CR(NotifySubscribers(UI_EVENT_INTERSECT_BEGAN, pUIEvent));
	} break;
	case (InteractionEventType::ELEMENT_INTERSECT_MOVED): {
		UIEvent *pUIEvent = new UIEvent(UI_EVENT_INTERSECT_MOVED, pEvent->m_pObject);
		CR(NotifySubscribers(UI_EVENT_INTERSECT_MOVED, pUIEvent));
	} break;
	case (InteractionEventType::ELEMENT_INTERSECT_ENDED): {
		UIEvent *pUIEvent = new UIEvent(UI_EVENT_INTERSECT_ENDED, pEvent->m_pObject);
		CR(NotifySubscribers(UI_EVENT_INTERSECT_ENDED, pUIEvent));
	} break;
	}
Error:
	return r;
}