#include "DreamAppManager.h"

#include "DreamOS.h"

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
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT DreamAppManager::Shutdown() {
	RESULT r = R_PASS;

	CR(ClearPriorityQueue());

Error:
	return r;
}

RESULT DreamAppManager::Initialize() {
	RESULT r = R_PASS;

	// TODO:
	CR(r);

Error:
	return r;
}

RESULT DreamAppManager::SetMinFrameRate(double minFrameRate) {
	m_minFrameRate = minFrameRate;
	return R_PASS;
}

RESULT DreamAppManager::Update() {
	RESULT r = R_PASS;

	std::shared_ptr<DreamAppBase> pDreamApp = nullptr;

	CBR((!m_appPriorityQueue.empty()), R_QUEUE_EMPTY);

	{
		// TODO: A lot of this can be alleviated with multi-threading 
		// and the update for an app is stuff that needs to be inside of the render thread
		auto tAfterLoop = std::chrono::high_resolution_clock::now();
		auto usLastLoopTime = std::chrono::duration_cast<std::chrono::microseconds>(tAfterLoop - m_tBeforeLoop).count();
		double usAppTimeAvailable = (1.0e6 / m_minFrameRate) - (double)(usLastLoopTime);

		//CBRM((usAppTimeAvailable > 0.0f), R_SKIPPED ,"Sandbox took longer than min frame rate time available %f", (double)(usAppTimeAvailable));

		double usTimeLeft = 0.0f;
		auto tAppUpdatesStart = std::chrono::high_resolution_clock::now();

		//while (m_appPriorityQueue.size() > 0 && 
		//	(usTimeLeft = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - tAppUpdatesStart).count()) < usAppTimeAvailable) 
		while (m_appPriorityQueue.size() > 0)
		{
			pDreamApp = m_appPriorityQueue.top();
			CN(pDreamApp);

			{
				// Remove the top item
				m_appPriorityQueue.pop();

				// Check shutdown flag
				if (pDreamApp->IsAppShuttingDown()) {
					// On shut down, don't update or push into run queue
					CR(pDreamApp->Shutdown(nullptr));
					CR(m_pDreamOS->RemoveObject(pDreamApp->GetComposite()));
					continue;
				}

				// Check Add to Scene Flag
				// TODO: This is really unspecific and will likely get torn up
				if (pDreamApp->CheckAndCleanAddToSceneFlag()) {
					m_pDreamOS->AddObject(pDreamApp->GetComposite());
				}

				// Capture time stamp
				auto tBeforeApp = std::chrono::high_resolution_clock::now();

				CR(pDreamApp->Update(pDreamApp->GetAppContext()));

				// Calculate time stamp and update (will affect priority
				auto tAfterApp = std::chrono::high_resolution_clock::now();
				auto usDiffApp = std::chrono::duration_cast<std::chrono::microseconds>(tAfterApp - tBeforeApp).count();
				pDreamApp->IncrementTimeRun(usDiffApp);

				m_appQueueAlreadyRun.push_front(pDreamApp);
			}
		}

		// Push all run apps back into pri-queue
		while (m_appQueueAlreadyRun.size() > 0) {
			pDreamApp = m_appQueueAlreadyRun.front();
			m_appQueueAlreadyRun.pop_front();

			m_appPriorityQueue.push(pDreamApp);
		}

		//CBRM((usTimeLeft >= 0.0f), R_SKIPPED, "App Manager ran out of time %f", usTimeLeft);

		//*/

		// TODO: Update the time run
		// TODO: Create a time slice mechanism
		// TODO: Threads?
	}

Error:
	m_tBeforeLoop = std::chrono::high_resolution_clock::now();

	return r;
}

RESULT DreamAppManager::ClearPriorityQueue() {
	RESULT r = R_PASS;

	while (!m_appPriorityQueue.empty()) {
		//std::shared_ptr<DreamAppBase> pDreamApp = m_appPriorityQueue.top();
		//m_appPriorityQueue.pop_front();

		///*
		std::shared_ptr<DreamAppBase> pDreamApp = m_appPriorityQueue.top();

		if (pDreamApp != nullptr) {
			CR(pDreamApp->Shutdown());
		}

		m_appPriorityQueue.pop();
		//*/
	}

Error:
	return r;
}

std::vector<UID> DreamAppManager::GetAppUID(std::string strName) {
	std::vector<UID> appUIDs;
	for (auto pApp : m_apps) {
		if (pApp.second->GetAppName() == strName) {
			appUIDs.emplace_back(pApp.first);
		}
	}
	return appUIDs;
}

DreamAppHandle* DreamAppManager::CaptureApp(UID uid, DreamAppBase* pHoldingApp) {
	DreamAppHandle* pAppHandle = nullptr;
	if (m_apps.count(uid) > 0) {
		auto pApp = m_apps[uid];
		if (pApp->GetAppUID() == uid) {
			//TODO: the real thing limiting getting a handle is whether there
			//stored in the map, not whether the 'AppState' is true or not
			if (m_capturedApps.count(uid) == 0) {
				pAppHandle = pApp->GetAppHandle();
				pAppHandle->SetAppState(true);
				if (pAppHandle->GetAppState()) {
					m_capturedApps[uid] = std::pair<DreamAppBase*, DreamAppBase*>(pApp, pHoldingApp);
					return pAppHandle;
				}
			}
			else {
				return pAppHandle;
			}
		}
	}
	return pAppHandle;
}

RESULT DreamAppManager::ReleaseApp(DreamAppHandle* pHandle, UID uid, DreamAppBase* pHoldingApp) {
	RESULT r = R_PASS;

	//TODO: does it make more sense to have DreamAppHandle as the key to the map?
	if (pHandle != nullptr && m_capturedApps.count(uid) > 0) {
		//currently only allowing one captured app
		pHandle->SetAppState(false);
		m_capturedApps.erase(uid);
	}

//Error:
	return r;
}
