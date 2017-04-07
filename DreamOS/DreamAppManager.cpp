#include "DreamAppManager.h"
#include <chrono>

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
	// empty
}

RESULT DreamAppManager::Initialize() {
	RESULT r = R_PASS;

	// TODO: 
	CR(r);

Error:
	return r;
}

RESULT DreamAppManager::Update() {
	RESULT r = R_PASS;

	std::shared_ptr<DreamAppBase> pDreamApp = nullptr;

	CBR((!m_appPriorityQueue.empty()), R_QUEUE_EMPTY);

	for (auto &pDreamApp : m_appPriorityQueue) {
		CR(pDreamApp->Update(pDreamApp->GetAppContext()));
	}
/*
	pDreamApp = m_appPriorityQueue.top();
	CN(pDreamApp);

	//CR(pDreamApp->Print());
	{
		auto tBefore = std::chrono::high_resolution_clock::now();

		CR(pDreamApp->Update(pDreamApp->GetAppContext()));
		auto tAfter = std::chrono::high_resolution_clock::now();
		auto usDiff = std::chrono::duration_cast<std::chrono::microseconds>(tAfter - tBefore).count();
		pDreamApp->IncrementTimeRun(usDiff);
	}
	//*/

	// TODO: Update the time run 
	// TODO: Create a time slice mechanism
	// TODO: Threads?

Error:
	return r;
}

RESULT DreamAppManager::ClearPriorityQueue() {
	while (!m_appPriorityQueue.empty()) {
		//std::shared_ptr<DreamAppBase> pDreamApp = m_appPriorityQueue.top();
		m_appPriorityQueue.pop_front();
	}

	return R_PASS;
}