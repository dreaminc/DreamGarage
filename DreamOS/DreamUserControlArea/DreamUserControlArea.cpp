#include "DreamUserControlArea.h"

#include "DreamOS.h"
#include "DreamUserApp.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	

DreamUserControlArea::DreamUserControlArea(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUserControlArea>(pDreamOS, pContext)
{
	// empty
}

DreamUserControlArea::~DreamUserControlArea() 
{

}

RESULT DreamUserControlArea::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	m_pDreamUserApp = GetDOS()->LaunchDreamApp<DreamUserApp>(this);
	CN(m_pDreamUserApp);

	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

Error:
	return r;
}

RESULT DreamUserControlArea::OnAppDidFinishInitializing(void *pContext = nullptr) {
	return R_PASS;
}

RESULT DreamUserControlArea::Update(void *pContext = nullptr) {
	RESULT r = R_PASS;

	CR(m_pWebBrowserManager->Update());

Error:
	return r;
}

RESULT DreamUserControlArea::Shutdown(void *pContext = nullptr) {
	RESULT r = R_PASS;

	CR(m_pWebBrowserManager->Shutdown());

Error:
	return r;
}

DreamUserControlArea* DreamUserControlArea::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUserControlArea *pDreamApp = new DreamUserControlArea(pDreamOS, pContext);
	return pDreamApp;
}
