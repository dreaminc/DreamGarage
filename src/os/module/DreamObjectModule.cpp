#include "DreamObjectModule.h"

#include "Primitives/PrimParams.h"

#include "Primitives/sphere.h"
#include "DreamOS.h"

DreamObjectModule::PendingObject::PendingObject(PrimParams *pPrimParams, void *pContext) :
	pPrimParams(pPrimParams),
	pContext(pContext)
{ }

DreamObjectModule::PendingDimObject::PendingDimObject(PrimParams *pPrimParams, std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext) :
	PendingObject(pPrimParams, pContext),
	fnOnObjectReady(fnOnObjectReady)
{ }

DreamObjectModule::PendingTextureObject::PendingTextureObject(PrimParams *pPrimParams, std::function<RESULT(texture*, void*)> fnOnTextureReady, void *pContext) :
	PendingObject(pPrimParams, pContext),
	fnOnTextureReady(fnOnTextureReady)
{ }

RESULT DreamObjectModule::PendingObject::ClearObject() {
	RESULT r = R_PASS;

	if (pPrimParams != nullptr) {
		delete pPrimParams;
		pPrimParams = nullptr;
	}

	CR(ClearObjectImp());

Error:
	return r;
}

RESULT DreamObjectModule::PendingDimObject::ClearObjectImp() {
	if (pDimObj != nullptr) {
		delete pDimObj;
		pDimObj = nullptr;
	}

	return R_PASS;
};

RESULT DreamObjectModule::PendingTextureObject::ClearObjectImp() {
	if (pTexture != nullptr) {
		delete pTexture;
		pTexture = nullptr;
	}

	return R_PASS;
};

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
	RESULT r = R_PASS;

	while (m_queuedObjects.empty() != true) {
		PendingObject *pPendingObject = m_queuedObjects.front();
		m_queuedObjects.pop();

		pPendingObject->ClearObject();
	}

	while (m_pendingInitializationObjects.empty() != true) {
		PendingObject *pPendingObject = m_pendingInitializationObjects.front();
		m_pendingInitializationObjects.pop();

		pPendingObject->ClearObject();
	}

Error:
	return r;
}

RESULT DreamObjectModule::Update(void *pContext) {
	RESULT r = R_PASS;

	// This is GPU protected 
	if (m_pendingInitializationObjects.empty() == false) {
		m_pendingObjLock.lock();
			PendingObject *pPendingObject = m_pendingInitializationObjects.front();
			m_pendingInitializationObjects.pop();
		m_pendingObjLock.unlock();

		// Fork for texture vs. Object etc

		PendingDimObject *pPendingDimObject = dynamic_cast<PendingDimObject*>(pPendingObject);
		if (pPendingDimObject != nullptr) {
			CN(pPendingDimObject->pDimObj);
			
			CR(GetDOS()->InitializeObject(pPendingDimObject->pDimObj));
			CN(pPendingDimObject->pDimObj);

			// Execute the call back
			CR(pPendingDimObject->fnOnObjectReady(pPendingDimObject->pDimObj, pPendingDimObject->pContext));
		}

		PendingTextureObject *pPendingTexture = dynamic_cast<PendingTextureObject*>(pPendingObject);
		if (pPendingTexture != nullptr) {
			CN(pPendingTexture->pTexture);

			CR(GetDOS()->InitializeTexture(pPendingTexture->pTexture));
			CN(pPendingTexture->pTexture);

			// Execute the call back
			CR(pPendingTexture->fnOnTextureReady(pPendingTexture->pTexture, pPendingTexture->pContext));
		}
		

		// Clean out the params
		if ((pPendingTexture != nullptr || pPendingDimObject != nullptr)) {
			if (pPendingObject->pPrimParams != nullptr) {
				delete pPendingObject->pPrimParams;
				pPendingObject->pPrimParams = nullptr;
			}
		}
		else {
			DEBUG_LINEOUT("Pending HAL object is invalid");
		}
	}

Error:
	return R_PASS;
}

RESULT DreamObjectModule::ModuleProcess(void *pContext) {
	RESULT r = R_PASS;

	bool fRunning = true;

	while (fRunning) {
		if (m_queuedObjects.empty() == false) {
			
			m_objQueueLock.lock();
				PendingObject *pPendingObject = m_queuedObjects.front();
				m_queuedObjects.pop();
			m_objQueueLock.unlock();

			PendingDimObject *pPendingDimObject = dynamic_cast<PendingDimObject*>(pPendingObject);
			if (pPendingDimObject != nullptr) {
				pPendingDimObject->pDimObj = GetDOS()->MakeObject(pPendingDimObject->pPrimParams, false);
				CN(pPendingDimObject->pDimObj);
			}
			
			PendingTextureObject *pPendingTexture = dynamic_cast<PendingTextureObject*>(pPendingObject);
			if (pPendingTexture != nullptr) {
				pPendingTexture->pTexture = GetDOS()->MakeTexture(pPendingTexture->pPrimParams, false);
				CN(pPendingTexture->pTexture);
			}

			if ((pPendingTexture != nullptr || pPendingObject != nullptr)) {
				// Push to the GPU pending queue
				m_pendingObjLock.lock();
					m_pendingInitializationObjects.push(pPendingObject);
				m_pendingObjLock.unlock();
			}
			else {
				DEBUG_LINEOUT("Pending object is invalid");
			}
		}

		Sleep(1);
	}

Error:
	return r;
}

RESULT DreamObjectModule::QueueNewObject(PrimParams *pPrimParams, std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext) {
	RESULT r = R_PASS;

	PendingDimObject *pNewPendingObject = new PendingDimObject(pPrimParams, fnOnObjectReady, pContext);
	CN(pNewPendingObject);
	
	m_objQueueLock.lock();
		m_queuedObjects.push(pNewPendingObject);
	m_objQueueLock.unlock();

Error:
	return r;
}

RESULT DreamObjectModule::QueueNewTexture(PrimParams *pPrimParams, std::function<RESULT(texture*, void*)> fnOnTextureReady, void *pContext) {
	RESULT r = R_PASS;

	PendingTextureObject *pNewPendingTexture = new PendingTextureObject(pPrimParams, fnOnTextureReady, pContext);
	CN(pNewPendingTexture);

	m_objQueueLock.lock();
		m_queuedObjects.push(pNewPendingTexture);
	m_objQueueLock.unlock();

Error:
	return r;
}

