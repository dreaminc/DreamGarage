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
class DimObj;
class texture;

class DreamObjectModule : public DreamModule<DreamObjectModule>
{
	friend class DreamModuleManager;

private:
	struct PendingObject {
		PendingObject(PrimParams *pPrimParams, void *pContext);

		PrimParams *pPrimParams = nullptr;
		void *pContext = nullptr;
		
		RESULT ClearObject();
		virtual RESULT ClearObjectImp() = 0;
	};

	struct PendingDimObject : public PendingObject {
		PendingDimObject(PrimParams *pPrimParams, std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext);

		std::function<RESULT(DimObj*, void*)> fnOnObjectReady = nullptr;
		DimObj *pDimObj = nullptr;

		virtual RESULT ClearObjectImp() override;
	};

	struct PendingTextureObject : public PendingObject {
		PendingTextureObject(PrimParams *pPrimParams, std::function<RESULT(texture*, void*)> fnOnTextureReady, void *pContext);

		std::function<RESULT(texture*, void*)> fnOnTextureReady = nullptr;
		texture *pTexture = nullptr;

		virtual RESULT ClearObjectImp() override;
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
	RESULT QueueNewTexture(PrimParams *pPrimParams, std::function<RESULT(texture*, void*)> fnOnTextureReady, void *pContext);

protected:
	static DreamObjectModule* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::mutex m_objQueueLock;
	std::queue<PendingObject*> m_queuedObjects;

	std::mutex m_pendingObjLock;
	std::queue<PendingObject*> m_pendingInitializationObjects;
};

#endif // !DREAM_OBJECT_MODULE_H_
