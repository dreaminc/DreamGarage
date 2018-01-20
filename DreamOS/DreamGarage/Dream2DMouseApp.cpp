#include "Dream2DMouseApp.h"

#include "DreamOS.h"

Dream2DMouseApp::Dream2DMouseApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<Dream2DMouseApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

Dream2DMouseApp* Dream2DMouseApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	Dream2DMouseApp *pDreamApp = new Dream2DMouseApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT Dream2DMouseApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	SetAppName("Dream2DMouseApp");
	SetAppDescription("A Dream App for 2D use of the mouse");

	CR(r);

Error:
	return r;
}

RESULT Dream2DMouseApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT Dream2DMouseApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

// Currently creating objects in Update to follow the pattern in DreamPeer
// TODO: Reconsider this arch, as there should be a safe place for apps to create objects in the engine
RESULT Dream2DMouseApp::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_pMouseRay == nullptr) {
		m_pMouseRay = GetDOS()->AddRay(point(0.0), vector::kVector(1.0f));
		CN(m_pMouseRay);

		CR(GetDOS()->AddInteractionObject(m_pMouseRay));
	}

	if (m_pMouseRay != nullptr) {
		ray rMouseCast;
		GetDOS()->GetMouseRay(rMouseCast);

		m_pMouseRay->UpdateFromRay(rMouseCast);
	}

Error:
	return r;
}