#include "DreamSettingsApp.h"

#include "RESULT/EHM.h"

#include "DreamUserControlArea/DreamContentSource.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamBrowser.h"
#include "DreamUserApp.h"

DreamSettingsApp::DreamSettingsApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamSettingsApp>(pDreamOS, pContext)
{
	// empty
}

DreamSettingsApp::~DreamSettingsApp() 
{
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}


// DreamApp Interface
RESULT DreamSettingsApp::InitializeApp(void *pContext) {
	return R_PASS;
}

RESULT DreamSettingsApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamSettingsApp::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_pUserHandle == nullptr) {
		UID userAppUID = GetDOS()->GetUniqueAppUID("DreamUserApp");
		//m_pUserHandle = dynamic_cast<DreamUserHandle*>(GetDOS()->CaptureApp(userAppUID, this));
//		CN(m_pUserHandle);
	}

//Error:
	return r;
}

RESULT DreamSettingsApp::Shutdown(void *pContext) {
	return R_PASS;
}

DreamSettingsApp* DreamSettingsApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamSettingsApp *pDreamApp = new DreamSettingsApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamSettingsApp::InitializeSettingsForm(std::string strURL) {
	RESULT r = R_PASS;

//Error:
	return r;
}

RESULT DreamSettingsApp::Show() {
	RESULT r = R_PASS;

	CNR(m_pSettingsForm, R_SKIPPED);

//	m_pSettingsForm->

Error:
	return r;
}
