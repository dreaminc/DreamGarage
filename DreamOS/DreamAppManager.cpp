#include "DreamAppManager.h"

DreamAppManager::DreamAppManager(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{

// Success:
	Validate();
	return;

// Error:
	Invalidate();
	return;
}

DreamAppManager::~DreamAppManager() {
	// empty
}

RESULT DreamAppManager::Initialize() {
	RESULT r = R_PASS;

	// TODO: 
	CR(r);

Error:
	return r;
}

RESULT DreamAppManager::Update() {
	RESULT r = R_PASS;

	// TODO: 
	CR(r);

Error:
	return r;
}

template<class derivedAppType> 
RESULT DreamAppManager::CreateRegisterAndStartApp(void *pContext) {
	RESULT r = R_PASS;

	CN(m_pDreamOS);

	// TODO: Review whether this complexity is needed
	std::shared_ptr<derivedAppType> pDreamApp = std::shared_ptr(derivedAppType::SelfConstruct(m_pDreamOS, pContext));
	CN(pDreamApp);

	// Initialize the app
	CR(pDreamApp->InitializeApp(pDreamApp->GetAppContext()));
	CR(pDreamApp->SetPriority(DEFAULT_APP_PRIORITY));
	CR(pDreamApp->ResetTimeRun());

Error:
	return r;
}