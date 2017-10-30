#ifndef DREAM_APP_MANAGER_H_
#define DREAM_APP_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamApp.h
// The base Dream App object

#include "Primitives/Types/UID.h"

#include <queue>
#include <map>
#include <vector>
#include <memory>
#include <chrono>

// Some priority settings
#define SYSTEM_APP_PRIORITY 0
#define DREAM_APP_PRIORITY 2
#define DEFAULT_APP_PRIORITY DREAM_APP_PRIORITY
#define MAX_APP_PRIORITY 10

#include "DreamApp.h"
#include "Primitives/Manager.h"
#include "DreamAppHandle.h"

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
	// put into a .tpp file with an #include of said tpp file at the end
	// of the header
	template<class derivedAppType>
	std::shared_ptr<derivedAppType> CreateRegisterAndStartApp(void *pContext = nullptr, bool fAddToScene = true) {
		RESULT r = R_PASS;

		std::shared_ptr<derivedAppType> pDreamApp = nullptr;

		CN(m_pDreamOS);

		// TODO: Review whether this complexity is needed
		pDreamApp = std::shared_ptr<derivedAppType>(derivedAppType::SelfConstruct(m_pDreamOS, pContext));
		CN(pDreamApp);

		// Assign the app a composite
		//if (fAddToScene) {
		//	CRM(pDreamApp->SetComposite(m_pDreamOS->AddComposite()), "Failed to create Dream App composite");
		//}
		//else {
		//	CRM(pDreamApp->SetComposite(m_pDreamOS->MakeComposite()), "Failed to create Dream App composite");
		//}
		CRM(pDreamApp->SetComposite(m_pDreamOS->MakeComposite()), "Failed to create Dream App composite");

		if (fAddToScene)
			pDreamApp->SetAddToSceneFlag();

		// Initialize the app
		CR(pDreamApp->InitializeApp(pDreamApp->GetAppContext()));
		CR(pDreamApp->SetPriority(DEFAULT_APP_PRIORITY));
		CR(pDreamApp->ResetTimeRun());

		// Push to priority queue
		//m_appPriorityQueue.push_front(pDreamApp);
		m_appPriorityQueue.push(pDreamApp);

		//TODO: may want to use get() at a different level, keeping the map with shared_ptrs
		m_appRegistry[pDreamApp->GetAppUID()] = pDreamApp.get();

		// Success::
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

	RESULT SetMinFrameRate(double minFrameRate);
	bool FindDreamAppWithName(std::string strDreamAppName);

private:
	std::vector<DreamAppBase*> GetDreamApp(std::string strDreamAppName);

protected:
	RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage);

private:
	RESULT ClearPriorityQueue();

private:
	std::deque<std::shared_ptr<DreamAppBase>> m_appQueueAlreadyRun;
	std::priority_queue<std::shared_ptr<DreamAppBase>, std::vector<std::shared_ptr<DreamAppBase>>, DreamAppBaseCompare> m_appPriorityQueue;

	std::map<UID, std::vector<std::pair<DreamAppHandle*, DreamAppBase*>>> m_appHandleRegistry;
	std::map<UID, DreamAppBase*> m_appRegistry;
	DreamOS *m_pDreamOS;

	double m_minFrameRate = 90.0f;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tBeforeLoop;

};



#endif	// ! DREAM_APP_MANAGER_H_