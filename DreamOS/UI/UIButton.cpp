#include "UIButton.h"
#include "DreamOS.h"

UIButton::UIButton(HALImp *pHALImp) :
UIView(pHALImp)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UIButton::~UIButton() {

}

RESULT UIButton::Initialize() {
	RESULT r = R_PASS;

	for (int i = 0; i < UIEventType::UI_EVENT_INVALID; i++) {
		CR(RegisterSubscriber((UIEventType)(i), this));
	}

	m_pSurface = AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector::kVector());

	CN(m_pSurface);
	CR(InitializeOBB());

Error:
	return r;
}

RESULT UIButton::RegisterToInteractionEngine(DreamOS *pDreamOS) {
	RESULT r = R_PASS;
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(pDreamOS->AddAndRegisterInteractionObject(GetSurface().get(), (InteractionEventType)(i), this));
	}
Error:
	return r;
}

RESULT UIButton::RegisterEvent(UIEventType type, std::function<RESULT(void*)> fnCallback) {
	m_callbacks[type] = fnCallback;
	return R_PASS;
}

RESULT UIButton::Notify(UIEvent *pEvent) {
	RESULT r = R_PASS;

	std::function<RESULT(void*)> fnCallback;

	CBR(pEvent->m_pObj == m_pSurface.get(), R_SKIPPED);
	CBR(m_callbacks.count(pEvent->m_eventType) > 0, R_OBJECT_NOT_FOUND);

	fnCallback = m_callbacks[pEvent->m_eventType];
	CN(fnCallback);

	CR(fnCallback(this));

Error:
	return r;
}

std::shared_ptr<quad> UIButton::GetSurface() {
	return m_pSurface;
}
