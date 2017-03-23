#include "DreamAppManager.h"

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

	pDreamApp = m_appPriorityQueue.top();
	CN(pDreamApp);

	//CR(pDreamApp->Print());
	CR(pDreamApp->Update(pDreamApp->GetAppContext()));

	// TODO: Update the time run 
	// TODO: Create a time slice mechanism
	// TODO: Threads?

Error:
	return r;
}

RESULT DreamAppManager::ClearPriorityQueue() {
	while (!m_appPriorityQueue.empty()) {
		std::shared_ptr<DreamAppBase> pDreamApp = m_appPriorityQueue.top();
		m_appPriorityQueue.pop();
	}

	return R_PASS;
}