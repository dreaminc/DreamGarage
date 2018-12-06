#ifndef DREAM_MODULE_MANAGER_H_
#define DREAM_MODULE_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamModuleManager.h
// The module manager for Dream Modules

#include "Primitives/Types/UID.h"

#include <queue>
#include <map>
#include <vector>
#include <memory>
#include <chrono>

// Some priority settings
#define SYSTEM_MODULE_PRIORITY 0
#define DREAM_MODULE_PRIORITY 2
#define DEFAULT_MODULE_PRIORITY DREAM_APP_PRIORITY
#define MAX_MODULE_PRIORITY 10

#include "DreamModule.h"
#include "Primitives/Manager.h"

class DreamOS;
class PeerConnection;

class DreamModuleManager : public Manager {
	friend class SandboxApp;

public:
	DreamModuleManager(DreamOS *pDreamOS);
	~DreamModuleManager();

	virtual RESULT Initialize() override;
	virtual RESULT Update() override;
	virtual RESULT Shutdown() override;

	// TODO: This is here because of template silliness - but should be 
	// put into a .tpp file with an #include of said tpp file at the end
	// of the header
	template<class derivedModuleType>
	std::shared_ptr<derivedModuleType> CreateRegisterAndStartModule(void *pContext = nullptr) {
		RESULT r = R_PASS;

		std::shared_ptr<derivedModuleType> pDreamModule = nullptr;

		CN(m_pDreamOS);

		// TODO: Review whether this complexity is needed
		pDreamModule = std::shared_ptr<derivedModuleType>(derivedModuleType::SelfConstruct(m_pDreamOS, pContext));
		CN(pDreamModule);

		// Initialize the app
		CR(pDreamModule->Initialize());
		CR(pDreamModule->SetPriority(DEFAULT_MODULE_PRIORITY));
		CR(pDreamModule->ResetTimeRun());

		m_pendingModuleQueue.push(pDreamModule);

	Success:
		return pDreamModule;

	Error:
		if (pDreamModule != nullptr) {
			pDreamModule = nullptr;
		}

		return nullptr;
	}

	template<class derivedAppType>
	RESULT ShutdownModule(std::shared_ptr<derivedAppType> pDreamModule) {
		RESULT r = R_PASS;

		CR(pDreamModule->FlagShutdown("now"));

	Error:
		return r;
	}

	std::vector<UID> GetModuleUID(std::string strName);

	RESULT SetMinFrameRate(double minFrameRate);
	bool FindDreamModuleWithName(std::string strDreamModuleName);

public:
	std::shared_ptr<DreamModuleBase> GetDreamModuleFromUID(UID moduleUID);

private:
	std::vector<DreamModuleBase*> GetDreamModule(std::string strDreamModuleName);

private:
	RESULT ClearPriorityQueue();

private:
	std::deque<std::shared_ptr<DreamModuleBase>> m_moduleQueueAlreadyRun;
	std::priority_queue<std::shared_ptr<DreamModuleBase>, std::vector<std::shared_ptr<DreamModuleBase>>, DreamModuleBaseCompare> m_modulePriorityQueue;
	std::queue<std::shared_ptr<DreamModuleBase>> m_pendingModuleQueue;

	std::map<UID, std::shared_ptr<DreamModuleBase>> m_moduleRegistry;
	
	DreamOS *m_pDreamOS = nullptr;

	double m_minFrameRate = 90.0f;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tBeforeLoop;
};

#endif // ! DREAM_MODULE_MANAGER_H_