#include "DreamModuleManager.h"

#include "DreamOS.h"

DreamModuleManager::DreamModuleManager(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{

	// Success:
	Validate();
	return;

	// Error:
	Invalidate();
	return;
}

DreamModuleManager::~DreamModuleManager() {
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT DreamModuleManager::Shutdown() {
	RESULT r = R_PASS;

	CR(ClearPriorityQueue());

Error:
	return r;
}

RESULT DreamModuleManager::Initialize() {
	RESULT r = R_PASS;

	// TODO:
	CR(r);

Error:
	return r;
}

RESULT DreamModuleManager::SetMinFrameRate(double minFrameRate) {
	m_minFrameRate = minFrameRate;
	return R_PASS;
}

// TODO: Move to manager class
// TODO: This is not currently handling multiple apps with the same name
std::vector<DreamModuleBase*> DreamModuleManager::GetDreamModule(std::string strDreamModuleName) {
	std::vector<DreamModuleBase*> returnModuleVector;

	for (auto dreamModuleEntry : m_moduleRegistry) {
		auto &pDreamModule = dreamModuleEntry.second;

		if (pDreamModule->GetName() == strDreamModuleName) {
			returnModuleVector.push_back(pDreamModule.get());
		}
	}

	return returnModuleVector;
}

std::shared_ptr<DreamModuleBase> DreamModuleManager::GetDreamModuleFromUID(UID moduleUID) {
	for (auto dreamModuleEntry : m_moduleRegistry) {
		auto &pDreamModule = dreamModuleEntry.second;

		if (pDreamModule->GetUID() == moduleUID) {
			return pDreamModule;
		}
	}
	return nullptr;
}

// TODO: Move to manager class
bool DreamModuleManager::FindDreamModuleWithName(std::string strDreamModuleName) {
	for (auto dreamModuleEntry : m_moduleRegistry) {
		auto &pDreamModule = dreamModuleEntry.second;

		if (pDreamModule->GetName() == strDreamModuleName) {
			return true;
		}
	}

	return false;
}

// TODO: Move to manager class
RESULT DreamModuleManager::Update() {
	RESULT r = R_PASS;

	std::shared_ptr<DreamModuleBase> pDreamModule = nullptr;

	// Move modules from pending to pri queue 
	if (!m_pendingModuleQueue.empty()) {
		while (m_pendingModuleQueue.size() > 0) {
			std::shared_ptr<DreamModuleBase> pPendingModule = m_pendingModuleQueue.front();
			CN(pPendingModule);

			m_pendingModuleQueue.pop();
			m_modulePriorityQueue.push(pPendingModule);

			m_moduleRegistry[pPendingModule->GetUID()] = pPendingModule;
		}
	}

	CBR((!m_modulePriorityQueue.empty()), R_QUEUE_EMPTY);

	{
		// TODO: A lot of this can be alleviated with multi-threading 
		// and the update for an app is stuff that needs to be inside of the render thread
		auto tAfterLoop = std::chrono::high_resolution_clock::now();
		auto usLastLoopTime = std::chrono::duration_cast<std::chrono::microseconds>(tAfterLoop - m_tBeforeLoop).count();
		double usModuleTimeAvailable = (1.0e6 / m_minFrameRate) - (double)(usLastLoopTime);

		double usTimeLeft = 0.0f;
		auto tModuleUpdatesStart = std::chrono::high_resolution_clock::now();

		//while (m_appPriorityQueue.size() > 0 && 
		//	(usTimeLeft = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - tAppUpdatesStart).count()) < usAppTimeAvailable) 
		while (m_modulePriorityQueue.size() > 0)
		{
			pDreamModule = m_modulePriorityQueue.top();
			CN(pDreamModule);

			{
				// Remove the top item
				m_modulePriorityQueue.pop();

				// Check shutdown flag
				if (pDreamModule->IsShuttingDown()) {
					// On shut down, don't update or push into run queue
					
					CR(pDreamModule->Shutdown(nullptr));

					UID pDreamModuleUID = pDreamModule->GetUID();
					if (m_moduleRegistry.count(pDreamModuleUID) > 0) {
						m_moduleRegistry.erase(pDreamModuleUID);
					}

					continue;
				}

				// Capture time stamp
				auto tBeforeModule = std::chrono::high_resolution_clock::now();

				CR(pDreamModule->Update(pDreamModule->GetContext()));

				// Calculate time stamp and update (will affect priority
				auto tAfterModule = std::chrono::high_resolution_clock::now();
				auto usDiffModule = std::chrono::duration_cast<std::chrono::microseconds>(tAfterModule - tBeforeModule).count();
				pDreamModule->IncrementTimeRun(usDiffModule);

				m_moduleQueueAlreadyRun.push_front(pDreamModule);
			}
		}

		// Push all run apps back into pri-queue
		while (m_moduleQueueAlreadyRun.size() > 0) {
			pDreamModule = m_moduleQueueAlreadyRun.front();
			m_moduleQueueAlreadyRun.pop_front();

			m_modulePriorityQueue.push(pDreamModule);
		}

		//CBRM((usTimeLeft >= 0.0f), R_SKIPPED, "App Manager ran out of time %f", usTimeLeft);

		//*/

		// TODO: Create a time slice mechanism
		// TODO: Threads?
	}

Error:
	m_tBeforeLoop = std::chrono::high_resolution_clock::now();

	return r;
}

// TODO: Move to manager class
RESULT DreamModuleManager::ClearPriorityQueue() {
	RESULT r = R_PASS;

	while (!m_modulePriorityQueue.empty()) {

		///*
		std::shared_ptr<DreamModuleBase> pDreamModule = m_modulePriorityQueue.top();

		if (pDreamModule != nullptr) {
			CR(pDreamModule->Shutdown());
		}

		m_modulePriorityQueue.pop();
		//*/
	}

Error:
	return r;
}

// TODO: Move to manager class
std::vector<UID> DreamModuleManager::GetModuleUID(std::string strName) {
	std::vector<UID> moduleUIDs;

	for (auto pModule : m_moduleRegistry) {
		if (pModule.second->GetName() == strName) {
			moduleUIDs.emplace_back(pModule.first);
		}
	}
	return moduleUIDs;
}
