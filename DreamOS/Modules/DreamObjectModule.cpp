#include "DreamObjectModule.h"

#include "Primitives/PrimParams.h"

DreamObjectModule::DreamObjectModule(DreamOS *pDreamOS, void *pContext) :
	DreamModule<DreamObjectModule>(pDreamOS, pContext)
{
	RESULT r = R_PASS;

	Validate();
	return;

Error:
	Invalidate();
	return;
}

DreamObjectModule::~DreamObjectModule() {
	Shutdown();
}

// The Self Construct
DreamObjectModule* DreamObjectModule::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamObjectModule *pDreamModule = new DreamObjectModule(pDreamOS, pContext);
	return pDreamModule;
}

RESULT DreamObjectModule::InitializeModule(void *pContext) {
	RESULT r = R_PASS;

	SetName("DreamObjectModule");
	SetModuleDescription("The Dream Object Module is responsible for async loading of assets");

	// Pass in a context if needed in future
	CRM(StartModuleProcess(), "Failed to start module process");

Error:
	return r;
}

RESULT DreamObjectModule::OnDidFinishInitializing(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamObjectModule::Shutdown(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamObjectModule::Update(void *pContext) {
	RESULT r = R_PASS;

	// This is GPU protected 

Error:
	return R_PASS;
}

RESULT DreamObjectModule::ModuleProcess(void *pContext) {
	RESULT r = R_PASS;

	bool fRunning = true;

	while (fRunning) {



		Sleep(1);
	}

Error:
	return r;
}

RESULT DreamObjectModule::QueueNewObject(PrimParams *pPrimParams, std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext) {
	RESULT r = R_PASS;

	PendingDimObject newPendingObject;

	newPendingObject.pPrimParams = pPrimParams;
	newPendingObject.fnOnObjectReady = fnOnObjectReady;
	newPendingObject.pContext = pContext;

	m_queuedObjects.push(newPendingObject);

Error:
	return r;
}