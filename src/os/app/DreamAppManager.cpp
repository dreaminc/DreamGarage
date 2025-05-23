#include "DreamAppManager.h"

#include "os/DreamOS.h"

#include "DreamAppMessage.h"

DreamAppManager::DreamAppManager(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{

Success:
	Validate();
	return;

Error:
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

Error:
	return r;
}

RESULT DreamAppManager::SetMinFrameRate(double minFrameRate) {
	m_minFrameRate = minFrameRate;
	return R_PASS;
}

RESULT DreamAppManager::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	auto dreamApps = GetDreamApp(pDreamAppMessage->GetDreamAppName());
	CB((dreamApps.size() > 0));

	for (auto &pDreamApp : dreamApps) {
		CR(pDreamApp->HandleDreamAppMessage(pPeerConnection, pDreamAppMessage));
	}

Error:
	return r;
}

// TODO: This is not currently handling multiple apps with the same name
std::vector<DreamAppBase*> DreamAppManager::GetDreamApp(std::string strDreamAppName) {
	std::vector<DreamAppBase*> returnAppVector;

	for (auto dreamAppEntry : m_appRegistry) {
		auto &pDreamApp = dreamAppEntry.second;

		if (pDreamApp->GetAppName() == strDreamAppName) {
			returnAppVector.push_back(pDreamApp.get());
		}
	}

	return returnAppVector;
}

std::shared_ptr<DreamAppBase> DreamAppManager::GetDreamAppFromUID(UID appUID) {		// return nullptr if no app found, app may be in pending queue though
	for (auto dreamAppEntry : m_appRegistry) {
		auto &pDreamApp = dreamAppEntry.second;

		if (pDreamApp->GetAppUID() == appUID) {
			return pDreamApp;
		}
	}
	return nullptr;
}

bool DreamAppManager::FindDreamAppWithName(std::string strDreamAppName) {
	for (auto dreamAppEntry : m_appRegistry) {
		auto &pDreamApp = dreamAppEntry.second;

		if (pDreamApp->GetAppName() == strDreamAppName) {
			return true;
		}
	}

	return false;
}

RESULT DreamAppManager::Update() {
	RESULT r = R_PASS;

	std::shared_ptr<DreamAppBase> pDreamApp = nullptr;

	if (!m_pendingAppQueue.empty()) {
		while (m_pendingAppQueue.size() > 0) {
			std::shared_ptr<DreamAppBase> pPendingApp = m_pendingAppQueue.front();
			CN(pPendingApp);

			m_pendingAppQueue.pop();
			m_appPriorityQueue.push(pPendingApp);

			m_appRegistry[pPendingApp->GetAppUID()] = pPendingApp;
		}
	}

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

					UID pDreamAppUID = pDreamApp->GetAppUID();
					if (m_appRegistry.count(pDreamAppUID) > 0) {
						m_appRegistry.erase(pDreamAppUID);
					}

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
	for (auto pApp : m_appRegistry) {
		if (pApp.second->GetAppName() == strName) {
			appUIDs.emplace_back(pApp.first);
		}
	}
	return appUIDs;
}

DreamAppHandle* DreamAppManager::CaptureApp(UID uid, DreamAppBase* pRequestingApp) {
	DreamAppHandle* pAppHandle = nullptr;
	RESULT r = R_PASS;

	std::shared_ptr<DreamAppBase> pApp = nullptr;

	CN(pRequestingApp);
//	CBM(m_appRegistry.count(pRequestingApp->GetAppUID()) > 0,"requesting app not in DreamAppManager");
	CB(m_appRegistry.count(uid) > 0);
	pApp = m_appRegistry[uid];
	CN(pApp);
	CB(pApp->GetAppUID() == uid);

	//TODO: the real thing limiting getting a handle is whether there
	//stored in the map, not whether the 'AppState' is true or not
	//CB(m_appHandleRegistry.count(uid) == 0);
	CB(	m_appHandleRegistry.count(uid) == 0 || 
		pApp->GetHandleLimit() == -1 ||
		m_appHandleRegistry[uid].size() < pApp->GetHandleLimit());
	
	pAppHandle = pApp->GetAppHandle();
	pAppHandle->SetAppState(true);

	CB(pAppHandle->GetAppState());
	m_appHandleRegistry[uid].emplace_back(std::pair<DreamAppHandle*, DreamAppBase*>(pAppHandle, pRequestingApp));

	return pAppHandle;
Error:
	return nullptr;
}

//TODO: ReleaseApp could function with only the uid argument
RESULT DreamAppManager::ReleaseApp(DreamAppHandle* pHandle, UID uid, DreamAppBase* pRequestingApp) {
	RESULT r = R_PASS;

	//currently only allowing one captured app
	CN(pHandle);
	pHandle->SetAppState(false);

	{
		CB(m_appHandleRegistry.count(uid) > 0);

		auto regBegin = m_appHandleRegistry[uid].begin();
		auto regEnd = m_appHandleRegistry[uid].end();
		auto appPair = std::pair<DreamAppHandle*, DreamAppBase*>(pHandle, pRequestingApp);

		auto appPairIt = std::find(regBegin, regEnd, appPair);
		CB(appPairIt != regEnd);
		m_appHandleRegistry[uid].erase(appPairIt);
	}

Error:
	return r;
}

RESULT DreamAppManager::InitializeDreamAppComposite(DreamAppBase* pDreamApp) {
	RESULT r = R_PASS;

	composite* pAppComposite = m_pDreamOS->MakeComposite();
	CNM(pAppComposite, "Failed to create dream app composite");

	CRM(pDreamApp->SetComposite(pAppComposite), "Failed to set Dream App composite");

Error:
	return r;
}