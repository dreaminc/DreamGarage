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

RESULT DreamLoginApp::HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) {
	return R_PASS;
}

RESULT DreamLoginApp::HandleDreamFormSetEnvironmentId(int environmentId) {
	return R_PASS;
}
