#ifndef DREAM_OBJECT_MODULE_H_
#define DREAM_OBJECT_MODULE_H_

// DREAM OS
// DreamOS/Modules/DreamObjectModule.h
// The Dream Object Module is responsible for async loading of assets 
// like models, objects, and textures

#include "DreamModule.h"

#include "Primitives/Publisher.h"

#include <vector>
#include <chrono>

struct PrimParams;

class DreamObjectModule : public DreamModule<DreamObjectModule>
{
	friend class DreamModuleManager;

private:
	struct PendingDimObject {
		PrimParams *pPrimParams = nullptr;
		std::function<RESULT(DimObj*, void*)> fnOnObjectReady = nullptr;
		void *pContext = nullptr;
		DimObj *pDimObj = nullptr;

		RESULT ClearObject();
	};

public:
	DreamObjectModule(DreamOS *pDreamOS, void *pContext);
	~DreamObjectModule();

	virtual RESULT InitializeModule(void *pContext = nullptr) override;
	virtual RESULT OnDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual RESULT ModuleProcess(void *pContext) override;

	RESULT QueueNewObject(PrimParams *pPrimParams, std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext);

protected:
	static DreamObjectModule* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::mutex m_dimObjQueueLock;
	std::queue<PendingDimObject> m_queuedDimObjects;

	std::mutex m_pendingDimObjLock;
	std::queue<PendingDimObject> m_pendingInitializationDimbjects;
};

#endif // !DREAM_OBJECT_MODULE_H_
