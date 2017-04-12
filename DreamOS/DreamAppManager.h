#ifndef DREAM_APP_MANAGER_H_
#define DREAM_APP_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamApp.h
// The base Dream App object

#include "Primitives/Types/UID.h"

#include <queue>
#include <vector>
#include <memory>

// Some priority settings
#define SYSTEM_APP_PRIORITY 0
#define DREAM_APP_PRIORITY 2
#define DEFAULT_APP_PRIORITY DREAM_APP_PRIORITY
#define MAX_APP_PRIORITY 10

#include "DreamApp.h"
#include "Primitives/Manager.h"

class DreamOS;

class DreamAppManager : public Manager {
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
	std::shared_ptr<derivedAppType> CreateRegisterAndStartApp(void *pContext = nullptr) {
		RESULT r = R_PASS;

		std::shared_ptr<derivedAppType> pDreamApp = nullptr;

		CN(m_pDreamOS);

		// TODO: Review whether this complexity is needed
		pDreamApp = std::shared_ptr<derivedAppType>(derivedAppType::SelfConstruct(m_pDreamOS, pContext));
		CN(pDreamApp);

		// Assign the app a composite
		CRM(pDreamApp->SetComposite(m_pDreamOS->AddComposite()), "Failed to create Dream App composite");

		// Initialize the app
		CR(pDreamApp->InitializeApp(pDreamApp->GetAppContext()));
		CR(pDreamApp->SetPriority(DEFAULT_APP_PRIORITY));
		CR(pDreamApp->ResetTimeRun());

		// Push to priority queue
		m_appPriorityQueue.push_front(pDreamApp);
		//m_appPriorityQueue.push(pDreamApp);

	// Success::
		return pDreamApp;

	Error:
		if (pDreamApp != nullptr) {
			pDreamApp = nullptr;
		}

		return nullptr;
	}

private:
	RESULT ClearPriorityQueue();

private:
	std::deque<std::shared_ptr<DreamAppBase>> m_appPriorityQueue;
	//std::priority_queue<std::shared_ptr<DreamAppBase>, std::vector<std::shared_ptr<DreamAppBase>>, DreamAppBaseCompare> m_appPriorityQueue;
	DreamOS *m_pDreamOS;
};



#endif	// ! DREAM_APP_MANAGER_H_