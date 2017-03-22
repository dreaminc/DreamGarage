#include "DreamApp.h"
#include "DreamOS.h"

DreamApp::DreamApp(DreamOS *pDreamOS, void *pContext) :
	m_pDreamOS(pDreamOS),
	m_pCompositeContext(nullptr),
	m_pContext(pContext)
{
	// Empty
}

DreamApp::~DreamApp() {
	// empty
}

RESULT DreamApp::Initialize() {
	RESULT r = R_PASS;

	// Grab the context composite from DreamOS
	CN(m_pDreamOS);
	m_pCompositeContext = m_pDreamOS->AddComposite();
	CN(m_pCompositeContext);

	// Initialize the OBB (collisions)
	CR(m_pCompositeContext->InitializeOBB());
	CR(m_pDreamOS->AddInteractionObject(m_pCompositeContext));

	// Initialize the App
	CR(InitializeApp(m_pContext));

Error:
	return r;
}

RESULT DreamApp::SetAppName(std::string strAppName) {
	m_strAppName = strAppName;
	return R_PASS;
}

RESULT DreamApp::SetAppDescription(std::string strAppDescription) {
	m_strAppDescription = strAppDescription;
	return R_PASS;
}

DreamOS* DreamApp::GetDOS() {
	return m_pDreamOS;
}