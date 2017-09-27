#include "UIButton.h"
#include "DreamOS.h"

UIButton::UIButton(HALImp *pHALImp, DreamOS *pDreamOS) :
UIView(pHALImp, pDreamOS)
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

	for (int i = 0; i < (int)(UIEventType::UI_EVENT_INVALID); i++) {
		CR(RegisterSubscriber((UIEventType)(i), this));
	}

	m_pContextComposite = AddComposite();
	m_pSurfaceComposite = AddComposite();
	m_pSurface = AddQuad(0.25f, 0.25f * (9.0f / 16.0f), 1, 1, nullptr, vector::kVector());
	//m_pSurface = m_pSurfaceComposite->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector::kVector());

	CN(m_pSurface);
	CR(InitializeOBB());

Error:
	return r;
}

RESULT UIButton::RegisterToInteractionEngine(DreamOS *pDreamOS) {
	RESULT r = R_PASS;
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(pDreamOS->AddAndRegisterInteractionObject(this, (InteractionEventType)(i), this));
	}
Error:
	return r;
}

RESULT UIButton::RegisterEvent(UIEventType type, std::function<RESULT(UIButton*,void*)> fnCallback, void *pContext) {
	m_callbacks[type] = std::make_pair(fnCallback,pContext);
	return R_PASS;
}

RESULT UIButton::Notify(UIEvent *pEvent) {
	RESULT r = R_PASS;

	m_pInteractionObject = pEvent->m_pInteractionObject;
	m_ptContact = pEvent->m_ptContact;

	std::function<RESULT(UIButton*,void*)> fnCallback;

	CBR(m_callbacks.count(pEvent->m_eventType) > 0, R_OBJECT_NOT_FOUND);

	fnCallback = m_callbacks[pEvent->m_eventType].first;
	void *pContext = m_callbacks[pEvent->m_eventType].second;

	CN(fnCallback);

	CR(fnCallback(this,pContext));

Error:
	return r;
}

std::shared_ptr<quad> UIButton::GetSurface() {
	return m_pSurface;
}

std::shared_ptr<composite> UIButton::GetContextComposite() {
	return m_pContextComposite;
}

std::shared_ptr<composite> UIButton::GetSurfaceComposite() {
	return m_pSurfaceComposite;
}

VirtualObj *UIButton::GetInteractionObject() {
	return m_pInteractionObject;
}

point UIButton::GetContactPoint() {
	return m_ptContact;
}