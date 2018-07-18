#include "DreamLoginApp.h"

DreamLoginApp::DreamLoginApp(DreamOS *pDreamOS, void *pContext) :
	DreamFormApp(pDreamOS, pContext)
{
	// empty
}

DreamLoginApp::~DreamLoginApp() {
	RESULT r = R_PASS;

	CR(DreamFormApp::Shutdown());
	CR(Shutdown());

Error:
	return;
}

DreamLoginApp* DreamLoginApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamLoginApp *pDreamApp = new DreamLoginApp(pDreamOS, pContext);
	return pDreamApp;
}
