#include "DreamTestingApp.h"

#include "DreamOS.h"

DreamTestingApp::DreamTestingApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamTestingApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamTestingApp* DreamTestingApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamTestingApp *pDreamApp = new DreamTestingApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamTestingApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	// Subscribers (children)
	//for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
	//	CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	//}

	CR(r);

	SetAppName("DreamTestApp");
	SetAppDescription("A Dream Test App");

	// Set up the quad
	/*
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pScreenQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pScreenQuad->SetMaterialAmbient(0.8f);
	*/

Error:
	return r;
}

RESULT DreamTestingApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamTestingApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamTestingApp::Update(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

	//DEBUG_LINEOUT("Dream Test App Update ID:%ju cnt:%d timerun: %f us value: %d", GetUIDValue(), m_counter++, GetTimeRun(), GetTestingValue());

	// TODO: Better management stuff here

Error:
	return r;
}

RESULT DreamTestingApp::SetTestingValue(int val) {
	m_testingValue = val;
	return R_PASS;
}

int DreamTestingApp::GetTestingValue() {
	return m_testingValue;
}