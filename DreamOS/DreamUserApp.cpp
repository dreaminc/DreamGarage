#include "DreamUserApp.h"
#include "DreamOS.h"

DreamUserApp::DreamUserApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUserApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamUserApp* DreamUserApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUserApp *pDreamApp = new DreamUserApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamUserApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	// Subscribers (children)
	//for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
	//	CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	//}

	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		for (int j = 0; j < 3; j++) {
			CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
		}
	}

	CR(r);

	SetAppName("DreamUserApp");
	SetAppDescription("A Dream User App");

	// Set up the user asset
	/*
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pScreenQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pScreenQuad->SetMaterialAmbient(0.8f);
	*/

Error:
	return r;
}

RESULT DreamUserApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamUserApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamUserApp::Update(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamUserApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}