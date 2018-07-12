#include "DreamFormApp.h"

DreamFormApp::DreamFormApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamFormApp>(pDreamOS, pContext)
{
	// empty
}

DreamFormApp::~DreamFormApp()
{
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

// DreamApp Interface
RESULT DreamFormApp::InitializeApp(void *pContext) {
	return R_PASS;
}

RESULT DreamFormApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamFormApp::Update(void *pContext) {
	return R_PASS;
}

RESULT DreamFormApp::Shutdown(void *pContext) {
	return R_PASS;
}
