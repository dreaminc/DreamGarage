#include "DreamContentView.h"
#include "DreamOS.h"

DreamContentView::DreamContentView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamContentView>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

RESULT DreamContentView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	// Subscribers (children)
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

	SetAppName("DreamContentView");
	SetAppDescription("A Shared Content View");

	// Set up the quad
	

Error:
	return r;
}

RESULT DreamContentView::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamContentView::Update(void *pContext) {
	RESULT r = R_PASS;

	int a = 5;
	CR(r);

Error:
	return r;
}
	   
RESULT DreamContentView::Notify(InteractionObjectEvent *event) {
	RESULT r = R_PASS;

	/*
	std::shared_ptr<UIMenuItem> pItem = GetMenuItem(event->m_pObject);
	CBR(pItem != nullptr, R_OBJECT_NOT_FOUND);

	//TODO stupid hack, can be fixed by incorporating 
	// SenseController into the Interaction Engine
	if (event->m_eventType == InteractionEventType::ELEMENT_INTERSECT_ENDED)
		m_pCurrentItem = nullptr;
	else
		m_pCurrentItem = pItem;

	*/

	CR(r);

Error:
	return r;
}

DreamContentView* DreamContentView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamContentView *pDreamApp = new DreamContentView(pDreamOS, pContext);
	return pDreamApp;
}