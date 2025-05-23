#ifndef DREAM_APP_MANAGER_H_
#define DREAM_APP_MANAGER_H_

#include "core/ehm/EHM.h"

// Dream App Manager
// dos/src/app/DreamAppManager.h

// The base Dream App object

#include <queue>
#include <map>
#include <vector>
#include <memory>
#include <chrono>

#include "os/app/DreamApp.h"
#include "os/app/DreamAppHandle.h"

#include "core/types/Manager.h"
#include "core/types/UID.h"

#include "core/primitives/composite.h"

// Some priority settings
#define SYSTEM_APP_PRIORITY 0
#define DREAM_APP_PRIORITY 2
#define DEFAULT_APP_PRIORITY DREAM_APP_PRIORITY
#define MAX_APP_PRIORITY 10

class DreamOS;
class PeerConnection;
class DreamAppMessage;

class DreamAppManager : public Manager {
	friend class SandboxApp;

public:
	DreamAppManager(DreamOS *pDreamOS);
	~DreamAppManager();

	virtual RESULT Initialize() override;
	virtual RESULT Update() override;
	virtual RESULT Shutdown() override;

	// TODO: This is here because of template silliness - but should be 
	// TODO: put into a .tpp file with an #include of said tpp file at the end
	// TODO: of the header
	template<class derivedAppType>
	std::shared_ptr<derivedAppType> CreateRegisterAndStartApp(void *pContext = nullptr, bool fAddToScene = true) {
		RESULT r = R_PASS;

		std::shared_ptr<derivedAppType> pDreamApp = nullptr;

		CN(m_pDreamOS);

		// TODO: Review whether this complexity is needed
		pDreamApp = std::shared_ptr<derivedAppType>(derivedAppType::SelfConstruct(m_pDreamOS, pContext));
		CN(pDreamApp);

		{
			DreamAppBase* pDreamAppBase = dynamic_cast<DreamAppBase*>(pDreamApp.get());
			if (pDreamAppBase != nullptr) {
				CRM(InitializeDreamAppComposite(pDreamAppBase), "Failed to initialize dream app composite");
			}
		}

		if (fAddToScene)
			pDreamApp->SetAddToSceneFlag();

		// Initialize the app
		CR(pDreamApp->InitializeApp(pDreamApp->GetAppContext()));
		CR(pDreamApp->SetPriority(DEFAULT_APP_PRIORITY));
		CR(pDreamApp->ResetTimeRun());

		m_pendingAppQueue.push(pDreamApp);
		
		/*
		// Push to priority queue
		//m_appPriorityQueue.push_front(pDreamApp);
		m_appPriorityQueue.push(pDreamApp);

		//TODO: may want to use get() at a different level, keeping the map with shared_ptrs
		m_appRegistry[pDreamApp->GetAppUID()] = pDreamApp.get();
		//*/
	
	Success:
		return pDreamApp;

	Error:
		if (pDreamApp != nullptr) {
			pDreamApp = nullptr;
		}

		return nullptr;
	}

	template<class derivedAppType>
	RESULT ShutdownApp(std::shared_ptr<derivedAppType> pDreamApp) {
		RESULT r = R_PASS;

		CR(pDreamApp->FlagShutdown("now"));

	Error:
		return r;
	}

	std::vector<UID> GetAppUID(std::string strName);

	DreamAppHandle* CaptureApp(UID uid, DreamAppBase* pRequestingApp);
	RESULT ReleaseApp(DreamAppHandle* pHandle, UID uid, DreamAppBase* pRequestingApp);
	RESULT InitializeDreamAppComposite(DreamAppBase* pDreamApp);

	RESULT SetMinFrameRate(double minFrameRate);
	bool FindDreamAppWithName(std::string strDreamAppName);

public:
	std::vector<DreamAppBase*> GetDreamApp(std::string strDreamAppName);
	std::shared_ptr<DreamAppBase> GetDreamAppFromUID(UID appUID);

public:
	RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage);

private:
	RESULT ClearPriorityQueue();

private:
	std::deque<std::shared_ptr<DreamAppBase>> m_appQueueAlreadyRun;
	std::priority_queue<std::shared_ptr<DreamAppBase>, std::vector<std::shared_ptr<DreamAppBase>>, DreamAppBaseCompare> m_appPriorityQueue;
	std::queue<std::shared_ptr<DreamAppBase>> m_pendingAppQueue;

	std::map<UID, std::vector<std::pair<DreamAppHandle*, DreamAppBase*>>> m_appHandleRegistry;
	std::map<UID, std::shared_ptr<DreamAppBase>> m_appRegistry;
	DreamOS *m_pDreamOS = nullptr;

	double m_minFrameRate = 90.0f;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tBeforeLoop;

};



#endif	// ! DREAM_APP_MANAGER_H_