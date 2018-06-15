#include "DreamSettingsApp.h"

#include "RESULT/EHM.h"

#include "DreamUserControlArea/DreamContentSource.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamControlView/DreamControlView.h"
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

	if (m_pUserApp == nullptr) {
		UID userAppUID = GetDOS()->GetUniqueAppUID("DreamUserApp");
		m_pUserApp = dynamic_cast<DreamUserApp*>(GetDOS()->CaptureApp(userAppUID, this));
		CN(m_pUserApp);

		m_pFormView = GetDOS()->LaunchDreamApp<DreamControlView>(this);
	}

	// there's fancier code around this in DreamUserControlArea, 
	// but we assume that there is only one piece of content here

	CR(m_pFormView->SetViewQuadTexture(m_pForm->GetSourceTexture()));

Error:
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

	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CNM(pEnvironmentControllerProxy, "Failed to get environment controller proxy");
	CRM(pEnvironmentControllerProxy->RequestOpenAsset("WebsiteProviderScope.WebsiteProvider", strURL, "website"), "Failed to share environment asset");

	m_pForm = GetDOS()->LaunchDreamApp<DreamBrowser>(this);
	m_pForm->PendUpdateObjectTextures();

	m_pForm->SetScope("WebsiteProviderScope.WebsiteProvider");
	m_pForm->SetPath(strURL);

Error:
	return r;
}

RESULT DreamSettingsApp::Show() {
	RESULT r = R_PASS;

	CNR(m_pFormView, R_SKIPPED);

//	m_pSettingsForm->

Error:
	return r;
}
