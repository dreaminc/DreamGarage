
// Apps

template<class derivedAppType>
std::shared_ptr<derivedAppType> DreamOS::LaunchDreamApp(void *pContext, bool fAddToScene) {
	RESULT r = R_PASS;
	
	std::shared_ptr<derivedAppType> pDreamApp = m_pSandbox->m_pDreamAppManager->CreateRegisterAndStartApp<derivedAppType>(pContext, fAddToScene);
	CNM(pDreamApp, "Failed to create app");

	return pDreamApp;

Error:
	if (pDreamApp != nullptr) {
		pDreamApp = nullptr;
	}

	return nullptr;
}
//*

template<class derivedAppType>
RESULT DreamOS::ShutdownDreamApp(std::shared_ptr<derivedAppType> pDreamApp) {
	RESULT r = R_PASS;

	CR(m_pSandbox->m_pDreamAppManager->ShutdownApp<derivedAppType>(pDreamApp));

Error:
	return r;
}

// Modules

template<class derivedModuleType>
std::shared_ptr<derivedModuleType> DreamOS::LaunchDreamModule(void *pContext) {
	RESULT r = R_PASS;
	
	std::shared_ptr<derivedModuleType> pDreamModule = m_pSandbox->m_pDreamModuleManager->CreateRegisterAndStartModule<derivedModuleType>(pContext);
	CNM(pDreamModule, "Failed to create module");

	return pDreamModule;

Error:
	if (pDreamModule != nullptr) {
		pDreamModule = nullptr;
	}

	return nullptr;
}
//*

template<class derivedModuleType>
RESULT DreamOS::ShutdownDreamModule(std::shared_ptr<derivedModuleType> pDreamModule) {
	RESULT r = R_PASS;

	CR(m_pSandbox->m_pDreamModuleManager->ShutdownModule<derivedModuleType>(pDreamModule));

Error:
	return r;
}