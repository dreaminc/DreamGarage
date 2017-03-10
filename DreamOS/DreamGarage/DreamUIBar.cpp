#include "DreamUIBar.h"
#include "DreamOS.h"

DreamUIBar::DreamUIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat) :
	UIBar(pDreamOS, iconFormat, labelFormat, barFormat)
{
	RESULT r = R_PASS;

	CR(DreamUIBar::Initialize());

//Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

DreamUIBar::~DreamUIBar() 
{
	// empty
}

// TODO: Will be implemented or moved once registration architecture is fleshed out
RESULT DreamUIBar::RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback) {
	m_callbacks[type] = fnCallback;
	return R_PASS;
}

RESULT DreamUIBar::Initialize() {
	RESULT r = R_PASS;

	CR(SetVisible(false));

	auto fnStart = [&](void* pContext) {
		return R_PASS;
	};

	auto fnEnd = [&](void* pContext) {
		return R_PASS;
	};

	RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_BEGAN, fnStart);
	RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_ENDED, fnEnd);

Error:
	return r;
}

RESULT DreamUIBar::OnTouchStart(void* pContext) {
	RESULT r = R_PASS;

	UIMenuItem* pItem = reinterpret_cast<UIMenuItem*>(pContext);
	CN(pItem);
	pItem->GetQuad()->Scale(m_barFormat.itemScaleSelected);

Error:
	return r;
}

RESULT DreamUIBar::OnTouchMove(void* pContext) {
	return R_PASS;
}

RESULT DreamUIBar::OnTouchEnd(void* pContext) {
	RESULT r = R_PASS;

	UIMenuItem* pItem = reinterpret_cast<UIMenuItem*>(pContext);
	CN(pItem);
	pItem->GetQuad()->SetScale(m_barFormat.itemScale);

Error:
	return r;
}

RESULT DreamUIBar::Update() {
	RESULT r = R_PASS;
	CR(UpdateInteractionPrimitive(GetHandRay()));
Error:
	return r;
}

RESULT DreamUIBar::Notify(InteractionObjectEvent *event) {
	RESULT r = R_PASS;

	UIModule::Notify(event);

	std::function<RESULT(void*)> fnCallback;
	std::shared_ptr<UIMenuItem> pItem = GetMenuItem(event->m_pObject);

	CBR(m_callbacks.count(event->m_eventType) > 0, R_OBJECT_NOT_FOUND)
	CBR(pItem != nullptr, R_OBJECT_NOT_FOUND);

	fnCallback = m_callbacks[event->m_eventType];

	fnCallback(pItem.get());

Error:
	return r;
}